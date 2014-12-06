#pragma once

/** 
 * @file LineParser.hpp
  * @author garipova.nailya@tgtoil.com
  * @author Copyright(C) 2012 TGT Oil & Gas Services. All right reserved.
 */


#include <boost/shared_ptr.hpp>
#include <stdexcept>

namespace parsers {

    class ParseError : public std::runtime_error {
    public:
        ParseError(char const* msg) : std::runtime_error(msg) {}
        ParseError(std::string const& msg) : std::runtime_error(msg) {}
    };

    class  LineParser {
    public:
        typedef boost::shared_ptr<LineParser> s_ptr;
        enum { 
            e_format_error         = -1, 
            e_ok                   =  0, 
            e_doublicate_attribute =  1, 
            e_number_parse_error   =  2
        };

    public:
        /** Parse a line
        * @param line a string to parse
        * @return true on success
        */
        virtual int parseLine(char const* line) = 0;
        virtual ~LineParser() {}
    };

    class MultilineParser : public LineParser {
    public:
        typedef boost::shared_ptr<MultilineParser> s_ptr;

    public:
        /** Designates the start of the multi-line parsing */
        virtual void begin() = 0;
        /** Designates the end of the multi-line parsing */
        virtual void end() = 0;
    };
}
