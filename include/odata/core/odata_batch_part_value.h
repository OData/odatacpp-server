//---------------------------------------------------------------------
// <copyright file="odata_batch_part_value.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/core/odata_value.h"

namespace odata { namespace core
{
class odata_batch_part_value
{
public:
    odata_batch_part_value():m_status_code(200), m_status_message(U("OK"))
    {
    }

    std::shared_ptr<::odata::core::odata_value> get_odata_value()
    {
        return m_value;
    }

    void set_odata_value(std::shared_ptr<::odata::core::odata_value> odata_value)
    {
        m_value = odata_value;
    }

    int16_t get_status_code()
    {
        return m_status_code;
    }

    void set_status_code(int16_t status_code)
    {
        m_status_code = status_code;
    }

    ::odata::utility::string_t  get_status_message()
    {
        return m_status_message;
    }

    void set_status_message(::odata::utility::string_t status_message)
    {
        m_status_message = status_message;
    }

    void set_header(::odata::utility::string_t name, ::odata::utility::string_t value)
    {
        m_headers[name] = value;
    }

    ::odata::utility::string_t get_header(::odata::utility::string_t name)
    {
        return m_headers[name];
    }

    std::unordered_map<::odata::utility::string_t, ::odata::utility::string_t> get_headers()
    {
        return m_headers;
    }

private:
    std::shared_ptr<::odata::core::odata_value> m_value;
    int16_t m_status_code;
    ::odata::utility::string_t m_status_message;
    std::unordered_map<::odata::utility::string_t, ::odata::utility::string_t> m_headers;
};
}}