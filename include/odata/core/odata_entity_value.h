//---------------------------------------------------------------------
// <copyright file="odata_entity_value.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/edm/odata_edm.h"
#include "odata/core/odata_value.h"
#include "odata/core/odata_structured_value.h"

namespace odata { namespace core
{

class odata_entity_value : public odata_structured_value
{
public:
	odata_entity_value(std::shared_ptr<::odata::edm::edm_entity_type> type) : odata_structured_value(type)
	{}

    odata_entity_value(odata_property_map properties, std::shared_ptr<::odata::edm::edm_entity_type> type) : odata_structured_value(type, properties)
	{}

    ::odata::utility::string_t get_etag()
    {
        return m_etag;
    }

    void set_etag(::odata::utility::string_t etag)
    {
        m_etag = etag;
    }

    ::odata::utility::uri get_id()
    {
        return m_id;
    }

    void set_id(::odata::utility::uri id)
    {
        m_id = id;
    }

    ::odata::utility::uri get_edit_link()
    {
        return m_edit_link;
    }

    void set_edit_link(::odata::utility::uri edit_link)
    {
        m_edit_link = edit_link;
    }

    ::odata::utility::uri get_read_link()
    {
        return m_read_link;
    }

    void set_read_link(::odata::utility::uri read_link)
    {
        m_read_link = read_link;
    }

	ODATACPP_API ::odata::utility::string_t get_entity_key_string();

private:
    ::odata::utility::string_t m_etag;

    ::odata::utility::uri m_id;
    ::odata::utility::uri m_edit_link;
    ::odata::utility::uri m_read_link;
};

}}