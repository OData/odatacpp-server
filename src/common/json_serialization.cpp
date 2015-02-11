//---------------------------------------------------------------------
// <copyright file="json_serialization.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata/common/platform.h"
#include "odata/common/basic_types.h"
#include "odata/common/asyncrt_utils.h"
#include "odata/common/json.h"

#pragma warning(disable : 4127) // allow expressions like while(true) pass 
using namespace odata::utility;
using namespace odata::utility::json;
using namespace odata::utility::conversions;


//
// JSON Serialization
//

#ifdef _MS_WINDOWS
void odata::utility::json::value::serialize(std::ostream& stream) const
{ 
    // This has better performance than writing directly to stream.
    std::string str;
    m_value->serialize_impl(str);
    stream << str; 
}
void odata::utility::json::value::format(std::basic_string<wchar_t> &string) const 
{
    m_value->format(string); 
}
#endif

void odata::utility::json::value::serialize(utility::ostream_t &stream) const 
{
    // This has better performance than writing directly to stream.
    utility::string_t str;
    m_value->serialize_impl(str);
    stream << str;  
}

void odata::utility::json::value::format(std::basic_string<char>& string) const
{
    m_value->format(string);
}

template<typename CharType>
void odata::utility::json::details::append_escape_string(std::basic_string<CharType>& str, const std::basic_string<CharType>& escaped)
{
    for (auto iter = escaped.begin(); iter != escaped.end(); ++iter)
    {
        switch (*iter)
        {
            case '\"': 
                str += '\\';
                str += '\"';
                break;
            case '\\': 
                str += '\\';
                str += '\\';
                break;
            case '\b':
                str += '\\';
                str += 'b';
                break;
            case '\f':
                str += '\\';
                str += 'f';
                break;
            case '\r':
                str += '\\';
                str += 'r';
                break;
            case '\n':
                str += '\\';
                str += 'n';
                break;
            case '\t':
                str += '\\';
                str += 't';
                break;
            default:
                str += *iter;
        }
    }
}

void odata::utility::json::details::format_string(const utility::string_t& key, utility::string_t& str)
{
    str.push_back('"');
    append_escape_string(str, key);
    str.push_back('"');
}

#ifdef _MS_WINDOWS
void odata::utility::json::details::format_string(const utility::string_t& key, std::string& str)
{
    str.push_back('"');
    append_escape_string(str, utility::conversions::to_utf8string(key));
    str.push_back('"');
}
#endif

void odata::utility::json::details::_String::format(std::basic_string<char>& str) const
{
    str.push_back('"');

    if(m_has_escape_char)
    {
        append_escape_string(str, utility::conversions::to_utf8string(m_string));
    }
    else
    {
        str.append(utility::conversions::to_utf8string(m_string));
    }

    str.push_back('"');
}

void odata::utility::json::details::_Number::format(std::basic_string<char>& stream) const 
{
    if(m_number.m_type != number::type::double_type)
    {
#ifdef _MS_WINDOWS
        // #digits + 1 to avoid loss + 1 for the sign + 1 for null terminator.
        const size_t tempSize = std::numeric_limits<uint64_t>::digits10 + 3;
        char tempBuffer[tempSize];

        // This can be improved performance-wise if we implement our own routine
        if (m_number.m_type == number::type::signed_type)
            _i64toa_s(m_number.m_intval, tempBuffer, tempSize, 10);
        else
            _ui64toa_s(m_number.m_uintval, tempBuffer, tempSize, 10);

        const auto numChars = strnlen_s(tempBuffer, tempSize);
        stream.append(tempBuffer, numChars);
#else
        std::stringstream ss;
        if (m_number.m_type == number::type::signed_type)
            ss << m_number.m_intval;
        else
            ss << m_number.m_uintval;

        stream.append(ss.str());
#endif
    }
    else
    {
        // #digits + 2 to avoid loss + 1 for the sign + 1 for decimal point + 5 for exponent (e+xxx) + 1 for null terminator
        const size_t tempSize = std::numeric_limits<double>::digits10 + 10;
        char tempBuffer[tempSize];
#ifdef _MS_WINDOWS
        const auto numChars = sprintf_s(tempBuffer, tempSize, "%.*g", std::numeric_limits<double>::digits10 + 2, m_number.m_value);
#else
        const auto numChars = std::snprintf(tempBuffer, tempSize, "%.*g", std::numeric_limits<double>::digits10 + 2, m_number.m_value);
#endif
        stream.append(tempBuffer, numChars);
    }
}

#ifdef _MS_WINDOWS

void odata::utility::json::details::_String::format(std::basic_string<wchar_t>& str) const
{
    str.push_back(L'"');

    if(m_has_escape_char)
    {
        append_escape_string(str, m_string);
    }
    else
    {
        str.append(m_string);
    }

    str.push_back(L'"');
}

void odata::utility::json::details::_Number::format(std::basic_string<wchar_t>& stream) const
{
    if(m_number.m_type != number::type::double_type)
    {
        // #digits + 1 to avoid loss + 1 for the sign + 1 for null terminator.
        const size_t tempSize = std::numeric_limits<uint64_t>::digits10 + 3;
        wchar_t tempBuffer[tempSize];
        
        if (m_number.m_type == number::type::signed_type)
            _i64tow_s(m_number.m_intval, tempBuffer, tempSize, 10);
        else
            _ui64tow_s(m_number.m_uintval, tempBuffer, tempSize, 10);

        stream.append(tempBuffer, wcsnlen_s(tempBuffer, tempSize));
    }
    else
    {
        // #digits + 2 to avoid loss + 1 for the sign + 1 for decimal point + 5 for exponent (e+xxx) + 1 for null terminator
        const size_t tempSize = std::numeric_limits<double>::digits10 + 10;
        wchar_t tempBuffer[tempSize];
        const int numChars = swprintf_s(tempBuffer, tempSize, L"%.*g", std::numeric_limits<double>::digits10 + 2, m_number.m_value);
        stream.append(tempBuffer, numChars);
    }
}

#endif

odata::utility::string_t odata::utility::json::details::_String::as_string() const
{
    return m_string;
}

odata::utility::string_t odata::utility::json::value::as_string() const
{
    return m_value->as_string();
}

odata::utility::string_t json::value::serialize() const { return m_value->to_string(); }