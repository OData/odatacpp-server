//---------------------------------------------------------------------
// <copyright file="utility.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include <cfloat>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <functional>
#include <memory>
#include <vector>
#include <utility>
#include <exception>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <list>
#include <thread> 
#include <mutex>
#include <iomanip>

#include "odata/common/basic_types.h"
#include "odata/common/uri.h"
#include "odata/common/json.h"

namespace odata { namespace utility
{

ODATACPP_API ::odata::utility::string_t strip_string(const ::odata::utility::string_t& escaped);

ODATACPP_API void split_string(::odata::utility::string_t& source, const ::odata::utility::string_t& delim, std::list<::odata::utility::string_t>& ret);

template<typename T>
void to_string(const T& input, ::odata::utility::string_t& result)
{
	odata::utility::stringstream_t ostr;
	ostr << input;
	result = ostr.str();
}

template<typename key_type, typename _t>
bool bind(const key_type &text, _t &ref) // const
{
    utility::istringstream_t iss(text);
    iss >> ref;
    if (iss.fail() || !iss.eof())
    {
        return false;
    }
    return true;
}

template <typename key_type>
bool bind(const key_type &text, utility::string_t &ref) //const 
{ 
    ref = text; 
    return true; 
}

ODATACPP_API bool is_relative_path(const ::odata::utility::string_t& root_url, const ::odata::utility::string_t& path);

ODATACPP_API ::odata::utility::string_t print_double(const double& db, int precision = 20);
ODATACPP_API ::odata::utility::string_t print_float(const float& db, int precision = 16);

ODATACPP_API bool is_digit(::odata::utility::char_t c);
ODATACPP_API bool is_hex_digit(::odata::utility::char_t c);
ODATACPP_API bool is_letter(::odata::utility::char_t c);
ODATACPP_API bool is_letter_or_digit(::odata::utility::char_t c);

}}
