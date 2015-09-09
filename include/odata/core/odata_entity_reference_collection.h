//---------------------------------------------------------------------
// <copyright file="odata_entity_reference_collection.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/common/nullable.h"
#include "odata/core/odata_entity_reference.h"
#include "odata/common/uri.h"

namespace odata { namespace core
{

class odata_entity_reference_collection
{
public:
    odata_entity_reference_collection()
    {

    }

    void add_entity_refernece(std::shared_ptr<odata_entity_reference> reference)
    {
        m_references.push_back(reference);
    }

    ::std::vector<std::shared_ptr<odata_entity_reference>>::const_iterator cbegin() const
    {
        return m_references.cbegin();
    }

    ::std::vector<std::shared_ptr<odata_entity_reference>>::const_iterator cend() const
    {
        return m_references.cend();
    }

    void set_context_url(::odata::utility::uri value)
    {
        m_context_url = value;
    }

    ::odata::utility::uri get_context_url()
    {
        return m_context_url;
    }

    void set_next_link(::odata::utility::uri value)
    {
        m_next_link = value;
    }

    const ::odata::utility::uri get_next_link() const
    {
        return m_next_link;
    }

    void set_delta_link(::odata::utility::uri value)
    {
        m_delta_link = value;
    }

    const ::odata::utility::uri get_delta_link() const
    {
        return m_delta_link;
    }

    void set_count(int64_t count)
    {
        m_count = count;
    }

    const ::odata::common::nullable<int64_t> get_count() const
    {
        return m_count;
    }

    int size() const 
	{
		return (int)m_references.size();
	}

private:
    ::std::vector<std::shared_ptr<odata_entity_reference>> m_references;
    ::odata::utility::uri m_context_url;
    
    ::odata::utility::uri m_next_link;
    ::odata::utility::uri m_delta_link;

    ::odata::common::nullable<int64_t> m_count;
};

}}