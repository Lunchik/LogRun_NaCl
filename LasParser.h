#pragma once

/** 
 * @file LasParser.hpp
  * @author garipova.nailya@tgtoil.com
  * @author Copyright(C) 2012 TGT Oil & Gas Services. All right reserved.
 */


#include "LineParser.h"
#include "Las.h"

namespace parsers {
    namespace las {
         MultilineParser::s_ptr create(data::las::las_t& las);
    }
}
