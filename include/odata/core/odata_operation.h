//---------------------------------------------------------------------
// <copyright file="odata_operation.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/common/uri.h"

namespace odata { namespace core
{

class odata_operation
{
public:
    odata_operation(::odata::utility::string_t name) : m_name(name), m_title(), m_target()
    {

    }

    virtual bool is_function() = 0;

    void set_title(::odata::utility::string_t title)
    {
        m_title = title;
    }

    ::odata::utility::string_t get_title()
    {
        return m_title;
    }

    void set_target(::odata::utility::uri target)
    {
        m_target = target;
    }

    ::odata::utility::uri get_target()
    {
        return m_target;
    }

    ::odata::utility::string_t get_name()
    {
        return m_name;
    }

private:
    ::odata::utility::string_t m_name;
    ::odata::utility::string_t m_title;
    ::odata::utility::uri m_target;
};

class odata_function : public odata_operation
{
public:
    odata_function(::odata::utility::string_t name) : odata_operation(name)
    {
    }

    bool is_function()
    {
        return true;
    }
};

class odata_action : public odata_operation
{
public:
    odata_action(::odata::utility::string_t name) : odata_operation(name)
    {
    }
    
    bool is_function()
    {
        return false;
    }
};
}}