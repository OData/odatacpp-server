//---------------------------------------------------------------------
// <copyright file="odata_metadata_builder.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/edm/odata_edm.h"
#include "odata/edm/edm_model_writer.h"
#include "odata/core/odata_value.h"
#include "odata/core/odata_json_writer.h"
#include "odata/core/odata_service_document.h"
#include "odata/common/json.h"
#include "odata/common/uri.h"

namespace odata { namespace core
{
class odata_metadata_builder
{
public:
    odata_metadata_builder(std::shared_ptr<::odata::edm::edm_model> model, ::odata::utility::uri service_root) 
		: m_model(model), m_service_root(service_root)
	{
	}

    ::odata::utility::uri get_entity_id(std::shared_ptr<::odata::core::odata_entity_value> entity_value, std::shared_ptr<::odata::edm::edm_entity_set> entity_set)
    {
        ::odata::utility::uri_builder builder(m_service_root);
        return builder.append_path(entity_set->get_name() + entity_value->get_entity_key_string()).to_uri();
    }

    ::odata::utility::uri get_read_link(std::shared_ptr<::odata::core::odata_entity_value> entity_value, std::shared_ptr<::odata::edm::edm_entity_set> entity_set)
    {
        return get_edit_link(entity_value, entity_set);
    }

    ::odata::utility::uri get_edit_link(std::shared_ptr<::odata::core::odata_entity_value> entity_value, std::shared_ptr<::odata::edm::edm_entity_set> entity_set)
    {
        ::odata::utility::uri_builder builder(m_service_root);
        builder.append_path(entity_set->get_name() + entity_value->get_entity_key_string());
        if (entity_value->get_value_type()->get_full_name() != entity_set->get_entity_type()->get_full_name())
        {
            builder.append_path(entity_value->get_value_type()->get_full_name());
        }

        return builder.to_uri();
    }


private:
	std::shared_ptr<::odata::edm::edm_model> m_model;
	::odata::utility::uri m_service_root;
};

}}