#pragma once 

/** 
 * @file LasReader.hpp
  * @author garipova.nailya@tgtoil.com
  * @author Copyright(C) 2012 TGT Oil & Gas Services. All right reserved.
 */


#include <stdio.h>
#include <utility>
#include <vector>
#include <istream>
#include "LasParser.h"
#include "Las.h"


namespace readers {

  class LasReader {
  public:
    typedef std::pair<int, std::string> err_info;
    typedef std::vector<err_info> err_infos;
    static int const DEF_ERRS = 128;
  public:
    LasReader(data::las::las_t& las) 
      : parser_(parsers::las::create(las))
    { las.clear(); err_.reserve(DEF_ERRS); }

	char* read(std::string is, /*std::vector<std::string> *vbuf,*/ char const* STOP_MARKER = 0 ) {
      std::string buffer;
      init_buffer(buffer);

	  int ii=0;
	  int iii=0;
	  int ind = 0;
	  //int bNewLine = 1;
	  //int sprflag;


      parser_->begin();
	  
	  /*if (is.length()>0)
		  int eobuf = 0;
	  else 
		  int eobuf = 1; */

	  
	  

	  for(unsigned int i = 0; i < is.length() - 1; ++i)
	  {
			++ii;
			  if(is[i] == '\n' || is[i] == '\r')
				  continue;
			  ++iii;

			  int endind =0;

			  while(is[i+endind] != '\n' && is[i+endind] != '\r'){
				  ++endind;
				  //++i;
			  }
			  
			  std::string str = is.substr(i, endind);
 			  //vbuf->push_back(str);

			  i += endind;
			  char const* line = str.c_str();//(char*)is.substr(i, endind).c_str();
 
			  
			  //DON'T return inside of the loop!
			  //if (ind ==3) return (char*)str.c_str();
			  ++ind;
			  //if ( STOP_MARKER && strstr(line, STOP_MARKER) ) break;
  			  int err = parser_->parseLine(line);
			  switch (err) {
				  case parsers::LineParser::e_ok:
					  break;
				  default:
					  std::string isret1;
	  /*char* cisret = (char*)isret1.c_str();

	  int sprflag1= sprintf(cisret, "err =  %d on ind = %d", err, ind);
      if (sprflag1>0) return cisret;*/
					  err_.push_back( std::make_pair(err, buffer));
			  }
	  }
		  
      /*while ( std::getline(is, buffer) ) {
		ii++;
        if ( buffer.empty() ) continue;
        char const* line = buffer.c_str();
		if (ii == 2)
		return (char*)line;
        if ( STOP_MARKER && strstr(line, STOP_MARKER) ) break;

        int err = parser_->parseLine(line);
        switch (err) {
        case parsers::LineParser::e_ok:
          break;
        default:
          err_.push_back( std::make_pair(err, buffer));
        }
		iii++;
      }*/
      parser_->end();
	  std::string retstr = "end of read";
	  std::string retstr1;
	  char* cretstr1 = (char*)retstr1.c_str();
	  int sprflag= sprintf(cretstr1, "overall number of lines: %d, number of not empty lines: %d", ii, iii);
      if (sprflag>0) return cretstr1;
	  else return (char*)retstr.c_str();
    }
    /** @brief returns list of errors as a vector of lines
     * like that <error_code, line_containing_error>
     * @return constant reference to a list of pair
     */
    err_infos const& error_infos() const {
      return err_;
    }

    void init_buffer(std::string& buffer ) const {
      size_t const BUFFER_SIZE = 1024;
      buffer.reserve(BUFFER_SIZE);
    }

  private:
    parsers::MultilineParser::s_ptr parser_;
    err_infos err_;
  };
}
