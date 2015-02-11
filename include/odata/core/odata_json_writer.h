//---------------------------------------------------------------------
// <copyright file="odata_json_writer.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/json.h"
#include "odata/common/utility.h"
#include "odata/core/odata_core.h"
#include "odata/edm/odata_edm.h"
#include "odata/core/odata_service_document.h"
#include "odata/core/odata_error.h"
#include "odata/core/odata_entity_reference.h"
#include "odata/core/odata_entity_reference_collection.h"

namespace odata { namespace core
{

#define ANNOTATION_ODATA_CONTEXT U("@odata.context")
#define ANNOTATION_ODATA_COUNT U("@odata.count")
#define ANNOTATION_ODATA_ETAG U("@odata.etag")
#define ANNOTATION_ODATA_ID U("@odata.id")
#define ANNOTATION_ODATA_EDIT_LINK U("@odata.editLink")
#define ANNOTATION_ODATA_READ_LINK U("@odata.readLink")
#define ANNOTATION_ODATA_NEXT_LINK U("@odata.nextLink")
#define ANNOTATION_ODATA_DELTA_LINK U("@odata.deltaLink")
#define ANNOTATION_ODATA_TYPE U("@odata.type")
#define JSON_VALUE U("value")
#define JSON_NAME U("name")
#define JSON_KIND U("kind")
#define JSON_URI U("uri")

#define JSON_ENTITYSET U("EntitySet");
#define JSON_SINGLETON U("Singleton");
#define JSON_FUNCTIONIMPORT U("FunctionImport");
#define METADATA U("$metadata")

class odata_json_writer
{
public:
	odata_json_writer(std::shared_ptr<::odata::edm::edm_model> model) : m_model(model)
	{
	}

    odata_json_writer(std::shared_ptr<::odata::edm::edm_model> model, ::odata::utility::uri service_root) 
		: m_model(model), m_service_root(service_root)
	{
		odata::utility::uri_builder builder(m_service_root);
		builder.append_path(METADATA);

		m_metadata_document_uri = builder.to_uri();
	}

	ODATACPP_API ::odata::utility::json::value serialize(std::shared_ptr<odata_value> value_object);
	ODATACPP_API ::odata::utility::json::value serialize(std::vector<std::shared_ptr<odata_parameter>> parameters);
    ODATACPP_API ::odata::utility::json::value serialize(std::shared_ptr<odata_service_document> service_document);
    ODATACPP_API ::odata::utility::json::value serialize(std::shared_ptr<odata_error> error);
    ODATACPP_API ::odata::utility::json::value serialize(std::shared_ptr<odata_entity_reference> entity_reference);

    ODATACPP_API ::odata::utility::json::value serialize(std::shared_ptr<odata_entity_reference_collection> entity_reference_collection);

private:
	odata::utility::json::value serialize_odata_value(const std::shared_ptr<::odata::edm::edm_named_type>& property_type, const std::shared_ptr<odata_value>& property_value);
    odata::utility::json::value seriliaze_structured_value(const std::shared_ptr<odata_structured_value>& p_value);
	odata::utility::json::value serialize_primitive_value(const std::shared_ptr<::odata::edm::edm_primitive_type>& p_primitive_type, const std::shared_ptr<odata_primitive_value>& p_value);
	odata::utility::json::value serialize_enum_value(const std::shared_ptr<odata_enum_value>& p_value);
    odata::utility::json::value serialize_collection_value(const std::shared_ptr<odata_collection_value>& p_value);
	bool is_type_serializable(const std::shared_ptr<::odata::edm::edm_named_type>& property_type);
    odata::utility::json::value serialize(std::shared_ptr<odata_service_document_element> service_document_element);
	std::shared_ptr<::odata::edm::edm_model> m_model;
    ::odata::utility::uri m_service_root;
	::odata::utility::uri m_metadata_document_uri;
};

}}