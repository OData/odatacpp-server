//---------------------------------------------------------------------
// <copyright file="asyncrt_utils.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/xxpublic.h"
#include "odata/common/basic_types.h"
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <system_error>

#if !defined(_MS_WINDOWS) || (_MSC_VER >= 1700)
#include <chrono>
#endif

#ifndef _MS_WINDOWS
#include <boost/algorithm/string.hpp>
#endif

namespace odata { namespace utility
{

#if  !defined(_MS_WINDOWS) || (_MSC_VER >= 1700) // Post VS10 and Linux
typedef std::chrono::seconds seconds;
#else // VS10
/// <summary>
/// A type used to represent timeouts for Azure storage APIs
/// </summary>
// The chrono header is not present on Visual Studio with versions < 1700 so we define a 'seconds' type for timeouts
class seconds
{
public:
    explicit seconds(__int64 time = 0): m_count(time) {}
    __int64 count() const { return m_count; }

private:
    __int64 m_count;
};
#endif

namespace timespan
{
    /// <summary>
    /// Converts a timespan/interval in seconds to xml duration string as specified by
    /// http://www.w3.org/TR/xmlschema-2/#duration
    /// </summary>
    ODATACPP_API utility::string_t __cdecl seconds_to_xml_duration(utility::seconds numSecs);

    /// <summary>
    /// Converts an xml duration to timespan/interval in seconds
    /// http://www.w3.org/TR/xmlschema-2/#duration
    /// </summary>
    ODATACPP_API utility::seconds __cdecl xml_duration_to_seconds(utility::string_t timespanString);
}

namespace conversions
{
    /// <summary>
    /// Converts a UTF-16 string to a UTF-8 string
    /// </summary>
    std::string __cdecl utf16_to_utf8(const utf16string &w);

    /// <summary>
    /// Converts a UTF-8 string to a UTF-16
    /// </summary>
    ODATACPP_API utf16string __cdecl utf8_to_utf16(const std::string &s);

    /// <summary>
    /// Converts a ASCII (us-ascii) string to a UTF-16 string.
    /// </summary>
    ODATACPP_API utf16string __cdecl usascii_to_utf16(const std::string &s);

    /// <summary>
    /// Converts a Latin1 (iso-8859-1) string to a UTF-16 string.
    /// </summary>
    ODATACPP_API utf16string __cdecl latin1_to_utf16(const std::string &s);

    /// <summary>
    /// Converts a string with the OS's default code page to a UTF-16 string.
    /// </summary>
    ODATACPP_API utf16string __cdecl default_code_page_to_utf16(const std::string &s);

    /// <summary>
    /// Decode to string_t from either a utf-16 or utf-8 string
    /// </summary>
    ODATACPP_API utility::string_t __cdecl to_string_t(std::string &&s);
    ODATACPP_API utility::string_t __cdecl to_string_t(utf16string &&s);
    ODATACPP_API utility::string_t __cdecl to_string_t(const std::string &s);
    ODATACPP_API utility::string_t __cdecl to_string_t(const utf16string &s);

    /// <summary>
    /// Decode to utf16 from either a narrow or wide string
    /// </summary>
    ODATACPP_API utf16string __cdecl to_utf16string(const std::string &value);
    ODATACPP_API utf16string __cdecl to_utf16string(utf16string value);

    /// <summary>
    /// Decode to UTF-8 from either a narrow or wide string.
    /// </summary>
    ODATACPP_API std::string __cdecl to_utf8string(std::string value);
    ODATACPP_API std::string __cdecl to_utf8string(const utf16string &value);

    /// <summary>
    /// Encode the given byte array into a base64 string
    /// </summary>
    ODATACPP_API utility::string_t __cdecl to_base64(const std::vector<unsigned char>& data);

    /// <summary>
    /// Encode the given 8-byte integer into a base64 string
    /// </summary>
    ODATACPP_API utility::string_t __cdecl to_base64(uint64_t data);

    /// <summary>
    /// Decode the given base64 string to a byte array
    /// </summary>
    ODATACPP_API std::vector<unsigned char> __cdecl from_base64(const utility::string_t& str);

    template <typename Source>
    utility::string_t print_string(const Source &val)
    {
        utility::ostringstream_t oss;
        oss << val;
        if (oss.bad())
            throw std::bad_cast();
        return oss.str();
    }
    template <typename Target>
    Target scan_string(const utility::string_t &str)
    {
        Target t;
        utility::istringstream_t iss(str);
        iss >> t;
        if (iss.bad())
            throw std::bad_cast();
        return t;
    }
}

namespace details
{
    /// <summary>
    /// Our own implementation of alpha numeric instead of std::isalnum to avoid
    /// taking global lock for performance reasons.
    /// </summary>
    inline bool __cdecl is_alnum(char ch)
    {
        return (ch >= '0' && ch <= '9')
            || (ch >= 'A' && ch <= 'Z')
            || (ch >= 'a' && ch <= 'z');
    }

    /// <summary>
    /// Simplistic implementation of make_unique. A better implementation would be based on variadic templates
    /// and therefore not be compatible with Dev10.
    /// </summary>
    template <typename _Type>
    std::unique_ptr<_Type> make_unique() {
        return std::unique_ptr<_Type>(new _Type());
    }

    template <typename _Type, typename _Arg1>
    std::unique_ptr<_Type> make_unique(_Arg1&& arg1) {
        return std::unique_ptr<_Type>(new _Type(std::forward<_Arg1>(arg1)));
    }

    template <typename _Type, typename _Arg1, typename _Arg2>
    std::unique_ptr<_Type> make_unique(_Arg1&& arg1, _Arg2&& arg2) {
        return std::unique_ptr<_Type>(new _Type(std::forward<_Arg1>(arg1), std::forward<_Arg2>(arg2)));
    }

    template <typename _Type, typename _Arg1, typename _Arg2, typename _Arg3>
    std::unique_ptr<_Type> make_unique(_Arg1&& arg1, _Arg2&& arg2, _Arg3&& arg3) {
        return std::unique_ptr<_Type>(new _Type(std::forward<_Arg1>(arg1), std::forward<_Arg2>(arg2), std::forward<_Arg3>(arg3)));
    }

    /// <summary>
    /// Cross platform utility function for performing case insensitive string comparision.
    /// </summary>
    /// <param name="left">First string to compare.</param>
    /// <param name="right">Second strong to compare.</param>
    /// <returns>true if the strings are equivalent, false otherwise</returns>
    inline bool str_icmp(const utility::string_t &left, const utility::string_t &right)
    {
#ifdef _MS_WINDOWS
        return _wcsicmp(left.c_str(), right.c_str()) == 0;
#else
        return boost::iequals(left, right);
#endif
    }

    // Turn const_iterator into an iterator
    template <typename Container, typename ConstIterator>
    typename Container::iterator remove_iterator_constness(Container& c, ConstIterator it)
    {
        return c.erase(it, it);
    }

#ifdef _MS_WINDOWS

/// <summary>
/// Category error type for Windows OS errors.
/// </summary>
class windows_category_impl : public std::error_category
{
public:
    virtual const char *name() const { return "windows"; }

    ODATACPP_API virtual std::string message(int errorCode) const;

    ODATACPP_API virtual std::error_condition default_error_condition(int errorCode) const;
};

/// <summary>
/// Gets the one global instance of the windows error category.
/// </summary>
/// </returns>An error category instance.</returns>
ODATACPP_API const std::error_category & __cdecl windows_category();

#else

/// <summary>
/// Gets the one global instance of the linux error category.
/// </summary>
/// </returns>An error category instance.</returns>
ODATACPP_API const std::error_category & __cdecl linux_category();

#endif

/// <summary>
/// Gets the one global instance of the current platform's error category.
/// <summary>
ODATACPP_API const std::error_category & __cdecl platform_category();

/// <summary>
/// Creates an instance of std::system_error from a OS error code.
/// </summary>
inline std::system_error __cdecl create_system_error(unsigned long errorCode)
{
    std::error_code code((int)errorCode, platform_category());
    return std::system_error(code, code.message());
}

/// <summary>
/// Creates a std::error_code from a OS error code.
/// </summary>
inline std::error_code __cdecl create_error_code(unsigned long errorCode)
{
    return std::error_code((int)errorCode, platform_category());
}

/// <summary>
/// Creates the corresponding error message from a OS error code.
/// </summary>
inline utility::string_t __cdecl create_error_message(unsigned long errorCode)
{
    return utility::conversions::to_string_t(create_error_code(errorCode).message());
}

}

class datetime
{
public:
    typedef uint64_t interval_type;

    /// <summary>
    /// Defines the supported date and time string formats.
    /// </summary>
    enum date_format { RFC_1123, ISO_8601 };

    /// <summary>
    /// Returns the current UTC time. 
    /// </summary>
    static ODATACPP_API datetime __cdecl utc_now();

    datetime() : m_interval(0)
    {
    }

    /// <summary>
    /// Creates <c>datetime</c> from a string representing time in UTC in RFC 1123 format.
    /// </summary>
    static ODATACPP_API datetime __cdecl from_string(const utility::string_t& timestring, date_format format = RFC_1123);

    /// <summary>
    /// Returns a string representation of the <c>datetime</c>.
    /// </summary>
    ODATACPP_API utility::string_t to_string(date_format format = RFC_1123) const;

    /// <summary>
    /// Returns the integral time value.
    /// </summary>
    interval_type to_interval() const
    {
        return m_interval;
    }

    datetime operator- (interval_type value) const
    {
        return datetime(m_interval - value);
    }

    datetime operator+ (interval_type value) const
    {
        return datetime(m_interval + value);
    }

    bool operator== (datetime dt) const
    {
        return m_interval == dt.m_interval;
    }

    bool operator!= (const datetime& dt) const
    {
        return !(*this == dt);
    }

    static interval_type from_milliseconds(unsigned int milliseconds)
    {
        return milliseconds*_msTicks;
    }

    static interval_type from_seconds(unsigned int seconds)
    {
        return seconds*_secondTicks;
    }

    static interval_type from_minutes(unsigned int minutes)
    {
        return minutes*_minuteTicks;
    }

    static interval_type from_hours(unsigned int hours)
    {
        return hours*_hourTicks;
    }

    static interval_type from_days(unsigned int days)
    {
        return days*_dayTicks;
    }

    bool is_initialized() const 
    {
        return m_interval != 0;
    }

private:

    friend int operator- (datetime t1, datetime t2);

    static const interval_type _msTicks = static_cast<interval_type>(10000);
    static const interval_type _secondTicks = 1000*_msTicks;
    static const interval_type _minuteTicks = 60*_secondTicks;
    static const interval_type _hourTicks   = 60*60*_secondTicks;
    static const interval_type _dayTicks    = 24*60*60*_secondTicks;


#ifdef _MS_WINDOWS
    // void* to avoid pulling in windows.h
    static ODATACPP_API bool __cdecl datetime::system_type_to_datetime(/*SYSTEMTIME*/ void* psysTime, uint64_t seconds, datetime * pdt);
#else
    static datetime timeval_to_datetime(struct timeval time);
#endif

    // Private constructor. Use static methods to create an instance.
    datetime(interval_type interval) : m_interval(interval)
    {
    }

    interval_type m_interval;
};

#ifndef _MS_WINDOWS

// temporary workaround for the fact that
// utf16char is not fully supported in GCC
class cmp
{
public:
    
    static int icmp(std::string left, std::string right)
    {
        size_t i;
        for (i = 0; i < left.size(); ++i)
        {
            if (i == right.size()) return 1;

            auto l = cmp::tolower(left[i]);
            auto r = cmp::tolower(right[i]);
            if (l > r) return 1;
            if (l < r) return -1;
        }
        if (i < right.size()) return -1;
        return 0;
    }

private:
    static char tolower(char c)
    {
        if (c >= 'A' && c <= 'Z')
            return static_cast<char>(c - 'A' + 'a');
        return c;
    }
};

#endif

inline int operator- (datetime t1, datetime t2)
{
    auto diff = (t1.m_interval - t2.m_interval);

    // Round it down to seconds
    diff /= 10 * 1000 * 1000;
    
    return static_cast<int>(diff);
}

} // namespace utility;
} // namespace odata;
