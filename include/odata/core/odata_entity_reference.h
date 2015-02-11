//---------------------------------------------------------------------
// <copyright file="odata_entity_reference.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/common/uri.h"

namespace odata { namespace core
{

class odata_entity_reference
{
public:
    odata_entity_reference(::odata::utility::uri id) : m_id(id)
    {
    }

    ::odata::utility::uri get_id()
    {
        return m_id;
    }

    void set_id(::odata::utility::uri id)
    {
        m_id = id;
    }

    void set_context_url(::odata::utility::uri value)
    {
        m_context_url = value;
    }

    ::odata::utility::uri get_context_url()
    {
        return m_context_url;
    }

private:
    ::odata::utility::uri m_context_url;
    ::odata::utility::uri m_id;
};

}}