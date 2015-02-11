//---------------------------------------------------------------------
// <copyright file="json.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata/common/platform.h"
#include "odata/common/basic_types.h"
#include "odata/common/asyncrt_utils.h"
#include "odata/common/json.h"

using namespace odata::utility;

bool json::details::g_keep_json_object_unsorted = false;
void json::keep_object_element_order(bool keep_order)
{
    json::details::g_keep_json_object_unsorted = keep_order;
}

odata::utility::ostream_t& odata::utility::json::operator << (utility::ostream_t &os, const odata::utility::json::value &val)
{
    val.serialize(os);
    return os;
}

odata::utility::istream_t& odata::utility::json::operator >> (utility::istream_t &is, json::value &val)
{
    val = json::value::parse(is);
    return is;
}

#pragma region json::value Constructors

odata::utility::json::value::value() : 
    m_value(utility::details::make_unique<odata::utility::json::details::_Null>())
#ifdef ENABLE_JSON_VALUE_VISUALIZER
    ,m_kind(value::Null)
#endif
    { }

odata::utility::json::value::value(int32_t value) : 
    m_value(utility::details::make_unique<odata::utility::json::details::_Number>(value))
#ifdef ENABLE_JSON_VALUE_VISUALIZER
    ,m_kind(value::Number)
#endif
    { }

odata::utility::json::value::value(uint32_t value) : 
    m_value(utility::details::make_unique<odata::utility::json::details::_Number>(value))
#ifdef ENABLE_JSON_VALUE_VISUALIZER
    ,m_kind(value::Number)
#endif
    { }

odata::utility::json::value::value(int64_t value) : 
    m_value(utility::details::make_unique<odata::utility::json::details::_Number>(value))
#ifdef ENABLE_JSON_VALUE_VISUALIZER
    ,m_kind(value::Number)
#endif
    { }

odata::utility::json::value::value(uint64_t value) : 
    m_value(utility::details::make_unique<odata::utility::json::details::_Number>(value))
#ifdef ENABLE_JSON_VALUE_VISUALIZER
    ,m_kind(value::Number)
#endif
    { }

odata::utility::json::value::value(double value) : 
    m_value(utility::details::make_unique<odata::utility::json::details::_Number>(value))
#ifdef ENABLE_JSON_VALUE_VISUALIZER
    ,m_kind(value::Number)
#endif
    { }

odata::utility::json::value::value(bool value) : 
    m_value(utility::details::make_unique<odata::utility::json::details::_Boolean>(value))
#ifdef ENABLE_JSON_VALUE_VISUALIZER
    ,m_kind(value::Boolean)
#endif
    { }

odata::utility::json::value::value(utility::string_t value) : 
    m_value(utility::details::make_unique<odata::utility::json::details::_String>(std::move(value)))
#ifdef ENABLE_JSON_VALUE_VISUALIZER
    ,m_kind(value::String)
#endif
    { }

odata::utility::json::value::value(const utility::char_t* value) : 
    m_value(utility::details::make_unique<odata::utility::json::details::_String>(utility::string_t(value)))
#ifdef ENABLE_JSON_VALUE_VISUALIZER
    ,m_kind(value::String)
#endif
    { }

odata::utility::json::value::value(const value &other) : 
    m_value(other.m_value->_copy_value())
#ifdef ENABLE_JSON_VALUE_VISUALIZER
    ,m_kind(other.m_kind)
#endif
    { }

odata::utility::json::value &odata::utility::json::value::operator=(const value &other)
{
    if(this != &other)
    {
        m_value = std::unique_ptr<details::_Value>(other.m_value->_copy_value());
#ifdef ENABLE_JSON_VALUE_VISUALIZER
        m_kind = other.m_kind;
#endif
    }
    return *this;
}
odata::utility::json::value::value(value &&other) : 
    m_value(std::move(other.m_value))
#ifdef ENABLE_JSON_VALUE_VISUALIZER
    ,m_kind(other.m_kind)
#endif
{}

odata::utility::json::value &odata::utility::json::value::operator=(odata::utility::json::value &&other)
{
    if(this != &other)
    {
        m_value.swap(other.m_value);
#ifdef ENABLE_JSON_VALUE_VISUALIZER
        m_kind = other.m_kind;
#endif
    }
    return *this;
}

#pragma endregion


#pragma region Static Factories

odata::utility::json::value odata::utility::json::value::null()
{
    return odata::utility::json::value();
}

odata::utility::json::value odata::utility::json::value::number(double value)
{
    return odata::utility::json::value(value);
}

odata::utility::json::value odata::utility::json::value::number(int32_t value)
{
    return odata::utility::json::value(value);
}

odata::utility::json::value odata::utility::json::value::boolean(bool value)
{
    return odata::utility::json::value(value);
}

odata::utility::json::value odata::utility::json::value::string(utility::string_t value)
{
    std::unique_ptr<details::_Value> ptr = utility::details::make_unique<details::_String>(std::move(value));
    return odata::utility::json::value(std::move(ptr)
#ifdef ENABLE_JSON_VALUE_VISUALIZER
            ,value::String
#endif
            );
}

#ifdef _MS_WINDOWS
odata::utility::json::value odata::utility::json::value::string(const std::string &value)
{
    std::unique_ptr<details::_Value> ptr = utility::details::make_unique<details::_String>(utility::conversions::to_utf16string(value));
    return odata::utility::json::value(std::move(ptr)
#ifdef ENABLE_JSON_VALUE_VISUALIZER
            ,value::String
#endif
            );
}
#endif

odata::utility::json::value odata::utility::json::value::object(bool keep_order)
{
    std::unique_ptr<details::_Value> ptr = utility::details::make_unique<details::_Object>(keep_order);
    return odata::utility::json::value(std::move(ptr)
#ifdef ENABLE_JSON_VALUE_VISUALIZER
            ,value::Object
#endif
            );
}

odata::utility::json::value odata::utility::json::value::object(std::vector<std::pair<::odata::utility::string_t, value>> fields, bool keep_order)
{
    std::unique_ptr<details::_Value> ptr = utility::details::make_unique<details::_Object>(std::move(fields), keep_order);
    return odata::utility::json::value(std::move(ptr)
#ifdef ENABLE_JSON_VALUE_VISUALIZER
            ,value::Object
#endif
            );
}

odata::utility::json::value odata::utility::json::value::array()
{
    std::unique_ptr<details::_Value> ptr = utility::details::make_unique<details::_Array>();
    return odata::utility::json::value(std::move(ptr)
#ifdef ENABLE_JSON_VALUE_VISUALIZER
            ,value::Array
#endif
            );
}

odata::utility::json::value odata::utility::json::value::array(size_t size)
{
    std::unique_ptr<details::_Value> ptr = utility::details::make_unique<details::_Array>(size);
    return odata::utility::json::value(std::move(ptr)
#ifdef ENABLE_JSON_VALUE_VISUALIZER
            ,value::Array
#endif
            );
}

odata::utility::json::value odata::utility::json::value::array(std::vector<value> elements)
{
    std::unique_ptr<details::_Value> ptr = utility::details::make_unique<details::_Array>(std::move(elements));
    return odata::utility::json::value(std::move(ptr)
#ifdef ENABLE_JSON_VALUE_VISUALIZER
            ,value::Array
#endif
            );
}

#pragma endregion

odata::utility::json::number odata::utility::json::value::as_number() const
{
    return m_value->as_number();
}

double odata::utility::json::value::as_double() const
{
    return m_value->as_double();
}
 
int odata::utility::json::value::as_integer() const
{
    return m_value->as_integer();
}

bool odata::utility::json::value::as_bool() const
{
    return m_value->as_bool();
}

json::array& odata::utility::json::value::as_array()
{
    return m_value->as_array();
}

const json::array& odata::utility::json::value::as_array() const
{
    return m_value->as_array();
}

json::object& odata::utility::json::value::as_object()
{
    return m_value->as_object();
}

const json::object& odata::utility::json::value::as_object() const
{
    return m_value->as_object();
}

bool odata::utility::json::number::is_int32() const
{
    switch (m_type)
    {
    case signed_type : return m_intval >= std::numeric_limits<int32_t>::min() && m_intval <= std::numeric_limits<int32_t>::max();
    case unsigned_type : return m_uintval <= std::numeric_limits<int32_t>::max();
    case double_type :
    default :
        return false;
    }
}

bool odata::utility::json::number::is_uint32() const
{
    switch (m_type)
    {
    case signed_type : return m_intval >= 0 && m_intval <= std::numeric_limits<uint32_t>::max();
    case unsigned_type : return m_uintval <= std::numeric_limits<uint32_t>::max();
    case double_type :
    default : 
        return false;
    }
}

bool odata::utility::json::number::is_int64() const
{
    switch (m_type)
    {
    case signed_type : return true;
    case unsigned_type : return m_uintval <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max());
    case double_type :
    default :
        return false;
    }
}

bool odata::utility::json::details::_String::has_escape_chars(const _String &str)
{
    static const auto escapes = U("\"\\\b\f\r\n\t");
    return str.m_string.find_first_of(escapes) != utility::string_t::npos;
}

odata::utility::json::details::_Object::_Object(const _Object& other) : 
    m_object(other.m_object.m_elements, other.m_object.m_keep_order), odata::utility::json::details::_Value(other) {}

odata::utility::json::value::value_type json::value::type() const { return m_value->type(); }

bool json::value::is_integer() const
{
    if(!is_number()) 
    {
        return false;
    }
    return m_value->is_integer();
}

bool json::value::is_double() const
{
    if(!is_number())
    {
        return false;
    }
    return m_value->is_double();
}

json::value& odata::utility::json::details::_Object::index(const utility::string_t &key)
{
    return m_object[key];
}

bool odata::utility::json::details::_Object::has_field(const utility::string_t &key) const
{
    return m_object.find(key) != m_object.end();
}

odata::utility::string_t json::value::to_string() const { return m_value->to_string(); }

bool json::value::operator==(const json::value &other) const
{
    if (this->m_value.get() == other.m_value.get())
        return true;
    if (this->type() != other.type())
        return false;

    switch(this->type())
    {
    case Null:
        return true;
    case Number:
        return this->as_number() == other.as_number();
    case Boolean:
        return this->as_bool() == other.as_bool();
    case String:
        return this->as_string() == other.as_string();
    case Object:
        return static_cast<const json::details::_Object*>(this->m_value.get())->is_equal(static_cast<const json::details::_Object*>(other.m_value.get()));
    case Array:
        return static_cast<const json::details::_Array*>(this->m_value.get())->is_equal(static_cast<const json::details::_Array*>(other.m_value.get()));
    }
    UNREACHABLE;
}

// at() overloads
odata::utility::json::value& odata::utility::json::value::at(size_t index)
{
    return this->as_array().at(index);
}

const odata::utility::json::value& odata::utility::json::value::at(size_t index) const
{
    return this->as_array().at(index);
}

odata::utility::json::value& odata::utility::json::value::at(const utility::string_t& key)
{
    return this->as_object().at(key);
}

const odata::utility::json::value& odata::utility::json::value::at(const utility::string_t& key) const
{
    return this->as_object().at(key);
}


odata::utility::json::value& odata::utility::json::value::operator [] (const utility::string_t &key)
{
    if ( this->is_null() )
    {
        m_value.reset(new odata::utility::json::details::_Object(details::g_keep_json_object_unsorted));
#ifdef ENABLE_JSON_VALUE_VISUALIZER
        m_kind = value::Object;
#endif
    }
    return m_value->index(key);
}

odata::utility::json::value& odata::utility::json::value::operator[](size_t index)
{
    if ( this->is_null() )
    {
        m_value.reset(new odata::utility::json::details::_Array());
#ifdef ENABLE_JSON_VALUE_VISUALIZER
        m_kind = value::Array;
#endif
    }
    return m_value->index(index);
}
