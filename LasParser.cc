/** 
 * @file LasParser.cpp
  * @author garipova.nailya@tgtoil.com
  * @author Copyright(C) 2012 TGT Oil & Gas Services. All right reserved.
 */


#include <algorithm>
#include <boost/xpressive/xpressive.hpp>
#include <boost/make_shared.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>


#include "LasParser.h"
#define MK_MSG(msg)  msg

namespace parsers {
    using namespace data::las;

    namespace {
        using namespace boost::xpressive;

        /////////////////////////////////////////////////////////////////
        cregex const EMPTY_RX = *_s >> ((as_xpr('#') >> *_) | *_s);

        cregex const ATTR_RX = *_s >> ( s1 = -+~as_xpr('.') ) >> *_s >> '.' >> ( s2 = *~_s) >> 
                               +_s >> ( s3 = -*~as_xpr(':') ) >> *_s >> ':' >> *_s >> (s4 = -*_) >> *_s;

        cregex const SEC_RX = *_s >> (s1 = '~' >> alpha >> *_) >> *_s;

        cregex const DEC_RX = !(set = '+','-') >> (( as_xpr('.') >> +_d ) | (+_d >> !(as_xpr('.') >> *_d))) >> 
                              !( (set = 'e','E') >> !(set = '+','-') >> *_d );

        cregex const DECS_RX = *_s >> DEC_RX >> *( +_s >> DEC_RX ) >> *_s;

        /////////////////////////////////////////////////////////////////
        /** @class LasHolder 
         *  @brief It's the auxiliary class 
         *  used just for performance reason 
         *  it holds an instance of Las and pointer to current selected section.
         */
        class LasHolder {
        public:
            LasHolder(las_t& las) 
                : las_(las) 
                , cur_sec_(0)
            {}
            inline section_t* currentSection() {
                return cur_sec_;
            }
            inline void setCurrentSection(section_t* sec) {
                cur_sec_ = sec;
            }
            inline las_t& getLas() { return las_; }

        protected:
            las_t& las_;
            section_t* cur_sec_;
        };
        /////////////////////////////////////////////////////////////////
        /** 
         * Simple implementation of the MultilineParser. 
         */
        class AbstractMatcher : public MultilineParser {
        public:
            AbstractMatcher(cregex const& rx, LasHolder& holder) 
                : rx_(rx)
                , holder_(holder)
            {}
            /** 
             * Called in case of the successful matching (recognition) of 
             * the current line. 
             * @param what is the match result 
             * @param holder is the LasHolder 
             * @return true if result is ok, false otherwise 
             */
            virtual int setResult(cmatch const& what, LasHolder& holder) = 0;

            int parseLine(char const* line) {
                return regex_match( line, what_, rx_) ? setResult(what_, holder_) : e_format_error;
            }
            void begin() {}        
            void end() {}

        private:
            cmatch what_;
            cregex const& rx_;
            LasHolder& holder_;
        };
        /////////////////////////////////////////////////////////////////
        /** 
         * Attributes recognizer. 
         * @throw ParseError when current section isn't set.
         */
        class AttrMatcher : public AbstractMatcher {
        public:
            AttrMatcher(LasHolder& las_p) 
                : AbstractMatcher(ATTR_RX, las_p)
            {}
            int setResult(cmatch const& what, LasHolder& holder) {
                section_t* sec = holder.currentSection();
                if (!sec) throw ParseError(MK_MSG("Current section can't be null"));

                std::string const& name = what[1].str();
                std::string const& unit = what[2].str();
                std::string const& val  = what[3].str();
                std::string const& desc = what[4].str();
                
                if ( sec->find_attribute(name) ) {
                    int n = 1;
                    std::string new_name;
                    do {
                      new_name = boost::str(boost::format("%1%_%2%") % name % n++);
                    } while (sec->find_attribute(new_name));                                      
                    sec->add_attribute(new_name, unit, val, desc);
                    return e_doublicate_attribute;
                }
                sec->add_attribute( name, unit, val, desc);
                return e_ok;
            }
        };
        /////////////////////////////////////////////////////////////////
        /** 
         * Sections recognizer. 
         */
        class SecMatcher : public AbstractMatcher {
        public:
            SecMatcher(LasHolder& las_p) 
                : AbstractMatcher(SEC_RX, las_p)
            {}
            int setResult(cmatch const& what, LasHolder& holder) {
                las_t& las = holder.getLas();
                std::string const& name = what[1].str();
                section_t* sec = las.find_section(name);
                if ( !sec ) 
                    sec = las.add_section(name);                   

                holder.setCurrentSection(sec);
                return e_ok;
            }
        };
        /////////////////////////////////////////////////////////////////
        template <typename Iterator>
        bool parse_doubles(Iterator first, Iterator last, std::vector<double>& v)  {
            using boost::spirit::qi::double_;
            using boost::spirit::qi::phrase_parse;
            using boost::spirit::qi::_1;
            using boost::spirit::ascii::space;
            using boost::phoenix::push_back;
            using boost::phoenix::ref;

            bool r = phrase_parse(first, last,
                ( *double_[push_back(ref(v), _1)] ), space);

            if (first != last) 
                return false;

            return r;
        }        
        ///////////////////////////////////////////////////////////////// 
        /** 
         * CurveParser parses ASCII section of the las file. 
         */
        class CurveParser : public MultilineParser {
            enum { DEF_CURVE_SIZE = 10240, DEF_NUMS_N = 128 };
        public:
            CurveParser(LasHolder& las_p) 
                : holder_(las_p)
                , curves_(0)
                , depth_(0)
				, cur_attr_(-1)
            {}  
            
            int parseLine(char const* line) {
                dv_.clear();

                if (!parse_doubles(line, line + strlen(line), dv_))
                    return e_number_parse_error;
                
                if ( !dv_.size() )
                    return e_ok;

                int wrap_around = curves_->size();
                int depth_index = depth_->size() - 1;                

                BOOST_FOREACH(double const& v, dv_ ) {
                    switch (cur_attr_) {
                    default:                             
                        if ( v != null_ ) {
                            data_log_t::curve_t& curve = (*curves_)[cur_attr_].curve;
                            curve.push_back(data_log_t::row_t(depth_index, v));
                        }
                        break;
                    case -1:
                        depth_->push_back(v);
                        ++depth_index;
                        break;
                    }                   
                    if (++cur_attr_ == wrap_around) cur_attr_ = -1;
                }
                return e_ok;
            }
                        
            // Initialize parser
            void begin() {
                las_t& las = holder_.getLas();           

                if ( section_t* sec = las.find_section('W') ) {
                    if ( attribute_t* attr = sec->find_attribute("NULL") ) 
                      null_ = atof(attr->data.c_str());
                    else
                        throw ParseError(MK_MSG("Unable to find NULL attribute"));
                } else 
                    throw ParseError(MK_MSG("Unable to find Well section"));
                
                logs_section_t& curve_sec = las.logs_section();
    
                if ( curve_sec.is_empty() )
                    throw ParseError(MK_MSG("Curve section is empty!"));
                
                curves_ = &curve_sec.logs_list();
                for ( int i = 0, n = curves_->size(); i < n; ++i ) 
                    ((*curves_)[i]).curve.reserve(DEF_CURVE_SIZE);

                depth_  = &curve_sec.depth_log().curve;
                cur_attr_ = -1;
                dv_.reserve(DEF_NUMS_N);
            }

            // Finalize parser
            void end() {
                if (depth_ && curves_) {
                    if (depth_->front() > depth_->back() )
                        reverse_logs();
                }
                curves_ = 0;
                depth_ = 0;
                cur_attr_ = -1;
            }

        private:
            // reverse logs 
            void reverse_logs() {
                int last_index = depth_->size() - 1;
                std::reverse(depth_->begin(), depth_->end());
                BOOST_FOREACH(data_log_t& log, *curves_) {
                    data_log_t::curve_t& curve = log.curve;
                    reverse_curve(curve, last_index);
                }
            }            

            static void reverse_curve( data_log_t::curve_t& curve, int shift ) {
                int fst = 0, lst = curve.size() - 1;
                while ( fst < lst ) {
                    data_log_t::row_t& fst_row = curve[fst++];
                    data_log_t::row_t& lst_row = curve[lst--];
                    fst_row.index = shift - fst_row.index;
                    lst_row.index = shift - lst_row.index;

                    using std::swap;
                    swap(fst_row, lst_row);
                }
                if ( fst == lst ) {
                    data_log_t::row_t& fst_row = curve[fst];
                    fst_row.index = shift - fst_row.index;
                }
            }

        private:
            LasHolder& holder_;
            logs_section_t::logs_list_t* curves_;
            depth_log_t::curve_t* depth_;
            int cur_attr_;
            double null_;
            std::vector<double> dv_;
        };
        /////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////
        class LasParserImpl : public LasHolder, public MultilineParser {
        public:
            LasParserImpl(las_t& las) 
                : LasHolder(las) 
                , attrParser_(*this)
                , secParser_(*this)
                , asciiParser_(*this)
                , rx_(EMPTY_RX)
            {
                cur_parser_ = &secParser_;
            }
            
            int parseLine( char const* line ) {
                if ( regex_match(line, rx_) )
                    return e_ok;
                else {
                    int ret = getParser()->parseLine(line);
                    return ret != e_format_error ? ret : getNextParser()->parseLine(line);
                }
            }
            void begin() {                
                //qDebug("Start las parser.");
                
            }

            void end() {
                // finalize the latest working parser
                getParser()->end();
                //qDebug("End las parser.");
            }

        private:
            MultilineParser* getParser() {
                return cur_parser_;
            }

            void setAttributeParser() {
                if (cur_sec_) {
                    char ch = cur_sec_->key();
                    switch(ch) {
                    case 'A':
                        cur_parser_ = &asciiParser_;
                        break;
                    default:
                        cur_parser_ = &attrParser_;
                    }
                }
            }

            MultilineParser* getNextParser() {
                cur_parser_->end();
                if (&attrParser_ == cur_parser_) {
                    cur_parser_ = &secParser_;
                } else if (&secParser_ == cur_parser_) {
                    setAttributeParser();
                } else { 
                    // switch from asciiParser
                    cur_parser_ = &attrParser_;
                }
                cur_parser_->begin();
                return cur_parser_;
            }

        private:
            MultilineParser* cur_parser_;        

            AttrMatcher attrParser_;
            SecMatcher  secParser_;
            CurveParser asciiParser_;
            cregex const& rx_;

        };

    } //namespace
    namespace las {
        MultilineParser::s_ptr create(data::las::las_t& las) {
            return boost::make_shared<LasParserImpl>(boost::ref(las));
        }
    }
} //namespace
