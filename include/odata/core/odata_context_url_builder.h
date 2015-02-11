//---------------------------------------------------------------------
// <copyright file="odata_context_url_builder.h" company="Microsoft">
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
class odata_context_url_builder
{
public:
    odata_context_url_builder(std::shared_ptr<::odata::edm::edm_model> model, ::odata::utility::uri service_root) 
		: m_model(model)
	{
        m_builder = ::odata::utility::uri_builder(service_root);
        m_metadata_url = m_builder.append_path(U("$metadata")).to_uri();
	}

    ::odata::utility::uri get_context_uri_for_service_document(std::shared_ptr<::odata::core::odata_service_document> service_document)
    {
        return m_metadata_url;
    }

    ::odata::utility::uri get_context_uri_for_collection_of_entities(std::shared_ptr<::odata::edm::edm_entity_set> entity_set)
    {
        ::odata::utility::uri_builder builder(m_metadata_url);
        return builder.append_path(entity_set->get_name()).to_uri();
    }

    ::odata::utility::uri get_context_uri_for_entity(std::shared_ptr<::odata::edm::edm_entity_set> entity_set)
    {
        ::odata::utility::uri_builder builder(m_metadata_url);
        return builder.append_path(entity_set->get_name()).append_path(U("$entity")).to_uri();
    }

    ::odata::utility::uri get_context_uri_for_singleton(std::shared_ptr<::odata::edm::edm_singleton> singleton)
    {
        ::odata::utility::uri_builder builder(m_metadata_url);
        return builder.append_path(singleton->get_name()).to_uri();
    }

    ::odata::utility::uri get_context_uri_for_collection_of_dervied_entities(std::shared_ptr<::odata::edm::edm_entity_set> entity_set, std::shared_ptr<::odata::edm::edm_entity_type> entity_type)
    {
        ::odata::utility::uri_builder builder(m_metadata_url);
        return builder.append_path(entity_set->get_name()).append_path(entity_type->get_full_name()).to_uri();
    }

    ::odata::utility::uri get_context_uri_for_derived_entity(std::shared_ptr<::odata::edm::edm_entity_set> entity_set, std::shared_ptr<::odata::edm::edm_entity_type> entity_type)
    {
        ::odata::utility::uri_builder builder(m_metadata_url);
        return builder.append_path(entity_set->get_name()).append_path(entity_type->get_full_name()).append_path(U("$entity")).to_uri();
    }

private:
	std::shared_ptr<::odata::edm::edm_model> m_model;
	::odata::utility::uri m_metadata_url;
    ::odata::utility::uri_builder m_builder;
};

}}