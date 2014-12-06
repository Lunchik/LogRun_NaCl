#pragma once 

/** 
 * @file Las.h
 * @author garipova.nailya@tgtoil.com
 * @author Copyright(C) 2012 TGT Oil & Gas Services. All right reserved.
 */

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/swap.hpp>

namespace data { namespace las  {
    ///////////////////////////////////////////////
    struct named_t {
        named_t() {}
        named_t(std::string const& nm ) : name(nm) {}
        
        void clear() { name.clear(); }
        void swap( named_t& other ) {
            using boost::swap;
            swap(name, other.name);
        }
        bool is_empty() const { return name.empty(); }

        std::string name;
    };

    /** @class attribute_t 
     *  @brief Represents Las file attribute
     * @var name - name of attribute
     * @var unit - measurement unit
     * @var desc - description
     * @var data - data
     */
    struct attribute_t : public named_t {
        attribute_t() {}
        attribute_t(std::string const& n, std::string const& u,  std::string const& v, std::string const& d) 
            : named_t(n)
            , unit(u)
            , data(v)
            , desc(d)
        {}
        /** clear attribute */
        void clear() {
            named_t::clear();
            unit.clear();
            desc.clear();
            data.clear();
        }
        /** check if attribute is empty */
        bool is_empty() const { 
            return named_t::is_empty() && unit.empty() && desc.empty() && data.empty(); 
        }
        /** nothrow swap */
        void swap(attribute_t& other) throw() {
            using boost::swap;
            named_t::swap(other);
            swap(unit, other.unit);
            swap(desc, other.desc);
            swap(data, other.data);
        }

        std::string  unit;
        std::string  data;
        std::string  desc;

    };
    ///////////////////////////////////////////////
    /** @class Section
     *  @brief Represents Las file's section.
     *         Consists of the name and list of attributes.
     */
    class section_t : public named_t {
    protected:
        section_t() {}
        section_t(std::string const& n) : named_t(n) {}
        section_t(section_t const& other) : named_t(other.name) {}
        section_t& operator = (section_t const& rhs) { 
            name = rhs.name; 
            return *this;
        }

    public:
        typedef std::vector<attribute_t> attr_list_t;

    public:
        
        virtual ~section_t() {}

        /** Returns section's key 
        *  @brief by format specification, the section's 1'st character 
        *         is the section's key. 
        *  @return one character key
        */
        static char key(std::string const& name) {
            return name.size() < 2 ? char(0) : name.at(1);
        }
        /** Returns section's key 
        *  @brief by format specification, the section's nams 1'st character 
        *         is the section's key. 
        *  @return one character key
        */
        char key() const {
            return key(name);
        }  
        /** Create  new attribute
         * @brief create new attribute and add it to the section
         */
        virtual attribute_t* add_attribute(std::string const& name, std::string const& unit, std::string const& val, std::string const& desc) = 0;
        /** Returns list of attributes contained in given section
         * @return QList<attribute_t const*>
         */
        virtual attr_list_t attr_vector() const = 0;
        /** Find attribute by name
        *  @brief find attribute by name
        *  @param name the name of the attribute
        *  @return pointer to the attribute if found, 0 otherwise
        */
        virtual attribute_t* find_attribute(std::string const& name) = 0;
        virtual attribute_t const* find_attribute(std::string const& name) const = 0;

        /** Returns size of the section
        *  @return number of the attributes in the section
        */
        virtual int size() const = 0;
        /** Check if the section is empty
        * @return true when the section is empty
        */
        virtual bool is_empty() const = 0;
        /** Clear all attributes
        */
        virtual void clear() = 0;
        
    };
    ///////////////////////////////////////////////
    /** @class depth_log_t 
     * @brief Represents 'depth' attribute with a bounded curve
     * @var curve -  an array of double
     */
    struct depth_log_t : public attribute_t {
        typedef std::vector<double> curve_t;
        depth_log_t(std::string const& n, std::string const& u,  std::string const& v, std::string const& d) 
            : attribute_t(n, u, v, d)
        {}
        
        /** Constructor */
        depth_log_t() {}
        
        /** clear data */
        void clear() {
            attribute_t::clear();
            curve.clear();
        }
        /** Check if empty */
        bool is_empty() const {
            return attribute_t::is_empty() && curve.empty();
        }
        void swap(depth_log_t& other) throw() {
            using boost::swap;
            attribute_t::swap(other);
            swap(curve, other.curve);
        }

        curve_t curve;
    };

    ///////////////////////////////////////////////
    /** @class data_log_t
     * @brief Represents an attribute with a bounded curve.
     * Curve consists of array of a row_t objects.
     * @var curve - curve data is an array of row_t.
     */
    struct data_log_t  : public attribute_t {        
        /**
         *  index - reference to the DepthAttribute::curve
         *  value - is a curve's value.
         */
        struct row_t { 
            row_t(int a, double b) : index(a), value(b) {}
            row_t() : index(0), value(0.0) {}
            bool operator == (row_t const& other) const { 
                return index == other.index && value == other.value; 
            }
            int index;
            double value; 
        };
        typedef std::vector<row_t> curve_t;
        /** Constructor */
        data_log_t(std::string const& n, std::string const& u,  std::string const& v, std::string const& d) 
            : attribute_t(n, u, v, d)
        {}
        // Do not reserve space for the curve because it 
        // my easily lead to space waste due to COW nature. 
        // of the Qt containers. 
        data_log_t() {}

        /** Clear all data */
        void clear() {
            attribute_t::clear();
            curve.clear();
        }
        /** Check if empty */
        bool is_empty() const {
            return attribute_t::is_empty() && curve.empty();
        }
        void swap(data_log_t& other) throw() {
            using boost::swap;
            attribute_t::swap(other);
            swap(curve, other.curve);
        }

        curve_t curve;
    };    
    ///////////////////////////////////////////////
    /** @class attr_section_t
     *  @brief Represents Las file's section
     *  consists of the name and list of attributes.
     */
    class attr_section_t : public section_t {
        enum { DEF_ATTR_NUM = 16 };

    public:
        typedef std::vector<attribute_t> attributes_t;

        attr_section_t() { attributes_.reserve(DEF_ATTR_NUM); } 

        attr_section_t(std::string const& n) : section_t(n) 
        { attributes_.reserve(DEF_ATTR_NUM); } 

        attr_section_t(attr_section_t const& other) 
            : section_t(other)
            , attributes_(other.attributes_) 
        {}

        attr_section_t& operator = (attr_section_t rhs) {
            swap(rhs);
            return *this;
        }

        attribute_t* add_attribute(std::string const& name, std::string const& unit, std::string const& val, std::string const& desc) {
            if (attribute_t* attr = find_attribute(name)) {
                attr->unit = unit;
                attr->data = val;
                attr->desc = desc;
                return attr;
            } else {
                attributes_.push_back(attribute_t(name, unit, val, desc));            
                return &attributes_.back();
            }
        }
        
        attr_list_t attr_vector() const {
            return attributes_;
        }

        attribute_t* find_attribute(std::string const& name) {
            if ( !name.empty() ) {
                BOOST_FOREACH(attribute_t& attr, attributes_) {
                    if (attr.name == name)
                        return &attr;
                }
            }
            return 0;
        }
        
        attribute_t const* find_attribute(std::string const& name) const {
            if ( !name.empty() ) {
                BOOST_FOREACH(attribute_t const& attr, attributes_) {
                    if (attr.name == name)
                        return &attr;
                }
            }
            return 0;
        }
                
        attributes_t& attributes() 
        { return attributes_; }
        attributes_t const& attributes() const 
        { return attributes_; }

        int size() const { return attributes_.size(); }
        bool is_empty() const { return attributes_.empty(); }

        void clear() { 
            attributes_.clear(); 
            name.clear();
        }

        // No throw swap
        void swap(attr_section_t& rhs) throw() {
            using boost::swap;
            named_t::swap(rhs);
            swap(attributes_, rhs.attributes_);
        }
    protected:
        attributes_t attributes_;
    };

    ///////////////////////////////////////////////
    /** @class logs_section_t
     *  @brief Represents Las file's curve section
     *  consists of the name, list of data logs and  
     *  depth log
     */

    class logs_section_t : public section_t {
        enum { DEF_ATTR_NUM = 16 };
    public:
        typedef std::vector<data_log_t> logs_list_t;

    public:
        logs_section_t() { 
            logs_.reserve(DEF_ATTR_NUM); 
        } 
        logs_section_t(std::string const& name) : section_t(name) { 
            logs_.reserve(DEF_ATTR_NUM); 
        }

        logs_section_t(logs_section_t const& other) 
            : section_t(other)
            , depth_(other.depth_) 
        {}

        logs_section_t& operator = (logs_section_t rhs) {
            swap(rhs);
            return *this;
        }

        attribute_t* add_attribute(std::string const& name, std::string const& unit, std::string const& val, std::string const& desc) {
            if ( depth_.name.empty() || depth_.name == name ) {
                depth_.name = name;
                depth_.unit = unit;
                depth_.data  = val;
                depth_.desc = desc;
                return &depth_;
            } else if ( data_log_t* log = find_log(name) ) {
                log->unit = unit;
                log->data = val;
                log->desc = desc;
                return log;
            } else {
                logs_.push_back(data_log_t(name, unit, val, desc));
                return &logs_.back();
            }
        }

        data_log_t* find_log(std::string const& name) {
            if ( !name.empty() ) {
                BOOST_FOREACH(data_log_t& attr, logs_) {
                    if (attr.name.compare(name) == 0)
                        return &attr;
                }
            }
            return 0;
        }

        data_log_t const* find_log(std::string const& name) const {
            if ( !name.empty() ) {
                BOOST_FOREACH(data_log_t const& attr, logs_) {
                    if (attr.name ==  name)
                        return &attr;
                }
            }
            return 0;
        }

        attr_list_t attr_vector() const {
            attr_list_t list;
            list.reserve(logs_.size() + 1);
            list.push_back(depth_);
            BOOST_FOREACH(data_log_t const& attr, logs_) {
                list.push_back(attr);
            }
            return list;
        }

        attribute_t* find_attribute(std::string const& name) {
            if ( !name.empty() ) {
                if (name == depth_.name)
                    return &depth_;
                else
                    return find_log(name);
            }
            return 0;
        }

        attribute_t const* find_attribute(std::string const& name) const {
            if ( !name.empty() ) {
                if (name == depth_.name)
                    return &depth_;
                else
                    return find_log(name);
            }
            return 0;
        }

        depth_log_t& depth_log() { return depth_; }
        depth_log_t const& depth_log() const { return depth_; }
        logs_list_t& logs_list() { return logs_; }
        logs_list_t const & logs_list() const { return logs_; }

        void clear() { 
            named_t::clear();
            logs_.clear();
            depth_.clear();
        }

        bool is_empty() const { 
            return named_t::is_empty() && depth_.is_empty() && logs_.empty(); 
        }
        
        int size() const { return logs_.size() + (depth_.is_empty() ? 0 : 1); }

        // No throw swap
        void swap(logs_section_t& rhs) throw() {
            using boost::swap;
            named_t::swap(rhs);
            swap(depth_, rhs.depth_);
            swap(logs_, rhs.logs_);
        }

    private:
        depth_log_t depth_;
        logs_list_t logs_;
    };
    ///////////////////////////////////////////////////////    
    /** @class las_t represents the las file.
     *  @brief consists of the set of sections 
     */
    class las_t {
        enum { DEF_SEC_NUM = 16 };
    public:
        typedef boost::shared_ptr<las_t> s_ptr;
        typedef std::vector<attr_section_t> attr_sec_list_t;

    public:
        las_t() {
            attrs_.reserve(DEF_SEC_NUM);
        }
        las_t(las_t const& rhs) 
            : attrs_(rhs.attrs_)
            , logs_(rhs.logs_) 
        {}

        /** Assignment */
        las_t& operator = (las_t rhs) {
            swap(rhs);
            return *this;
        }
        /** Get section by key
         * @brief find the section by the key.
         * @param key the section's key
         * @return pointer to section if found or null.
         */
        section_t* find_section( char key ) {
            switch ( key ) {
                case 'C':
                    return &logs_;
                default:
                    BOOST_FOREACH(section_t& sec, attrs_) {
                        if ( sec.key() == key )
                            return &sec;
                    }
            }
            return 0;
        }

        section_t const* find_section( char key ) const {
            switch ( key ) {
            case 'C':
                return &logs_;
            default:
                BOOST_FOREACH(section_t const& sec, attrs_) {
                    if ( sec.key() == key )
                        return &sec;
                }
            }
            return 0;
        }

        /** Get section by name
        * @brief find the section with the name.
        * @param name the section name
        * @return pointer to section if found or null.
        */
        section_t* find_section( std::string const& name ) {
            if ( !name.empty() ) 
                return find_section(section_t::key(name));
            else
                return 0;
        }
        /** Returns logs section */
        logs_section_t& logs_section() {
            return logs_;
        }

        logs_section_t const& logs_section() const {
            return logs_;
        }

        /** Return list of attribute sections */
        attr_sec_list_t& attr_sections_list() {
            return attrs_;
        }
        attr_sec_list_t const& attr_sections_list() const {
            return attrs_;
        }

        /** Add a section to the Las
         * @param name the section's name
         * @return pointer to created section
         */
        section_t* add_section(std::string const& name) {
            switch( section_t::key(name) ) {
                case 'C':
                    logs_.name = name;
                    return &logs_;
                default:
                    attrs_.push_back(attr_section_t(name));
                    return &attrs_.back();
            }
            return 0;
        }   
        /** Check whether Las is empty or not */
        bool is_empty() {
            return attrs_.empty() && logs_.is_empty();
        }
        /** Returns number of sections in Las */
        int size() const { return attrs_.size() + (logs_.is_empty() ? 0 : 1); }

        /** Clear content of Las */
        void clear() { 
            attrs_.clear(); 
            logs_.clear(); 
        }
        // No throw swap
        void swap( las_t& other ) throw() {
            using boost::swap;
            swap(attrs_, other.attrs_);
            swap(logs_, other.logs_);
        }
    private:
        attr_sec_list_t attrs_;
        logs_section_t logs_;
    };

}}
