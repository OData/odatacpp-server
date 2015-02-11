//---------------------------------------------------------------------
// <copyright file="odata_error.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/common/uri.h"

namespace odata { namespace core
{

class odata_error
{
public:
    odata_error(::odata::utility::string_t code, ::odata::utility::string_t message, ::odata::utility::string_t target) : m_code(code), m_message(message), m_target(target)
    {

    }

    ::odata::utility::string_t get_code()
    {
        return m_message;
    }

    ::odata::utility::string_t get_message()
    {
        return m_message;
    }

    ::odata::utility::string_t get_target()
    {
        return m_target;
    }


private:
    ::odata::utility::string_t m_code;
    ::odata::utility::string_t m_message;
    ::odata::utility::string_t m_target;
};

}}