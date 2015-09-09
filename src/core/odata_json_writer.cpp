//---------------------------------------------------------------------
// <copyright file="odata_json_writer.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata/core/odata_json_writer.h"

using namespace ::odata::utility;
using namespace ::odata::edm;

namespace odata { namespace core
{
    odata::utility::json::value odata_json_writer::serialize(const std::shared_ptr<odata_service_document> service_document)
    {
        odata::utility::json::value result = odata::utility::json::value::object();

        // context uri
        result[ANNOTATION_ODATA_CONTEXT] = odata::utility::json::value::string(m_metadata_document_uri.to_string());

        odata::utility::json::value value = odata::utility::json::value::array(service_document->size());

        size_t index = 0;
        for (auto iter = service_document->cbegin(); iter != service_document->cend(); iter++)
        {
            value[index++] = serialize(*iter);
        }

        result[JSON_VALUE] = value;

        return result;
    }

    odata::utility::json::value odata_json_writer::serialize(std::shared_ptr<odata_value> value_object)
    {
        if (value_object)
        {
            return serialize_odata_value(value_object->get_value_type(), value_object);
        }

        return odata::utility::json::value::null();
    }

    ::odata::utility::json::value odata_json_writer::serialize(std::vector<std::shared_ptr<odata_parameter>> parameters)
    {
        if (parameters.empty())
        {
            return odata::utility::json::value::null();
        }

        odata::utility::json::value result = odata::utility::json::value::object();
        for(auto iter = parameters.cbegin(); iter != parameters.cend(); iter++)
        {
            result[(*iter)->get_name()] = serialize((*iter)->get_value());
        }

        return result;
    }

    ::odata::utility::json::value odata_json_writer::serialize_odata_value(const std::shared_ptr<edm_named_type>& property_type, const std::shared_ptr<odata_value>& property_value)
    {
        odata::utility::json::value result = odata::utility::json::value::object();

        if (!property_type || !property_value)
        {
            return result;
        }

        switch(property_type->get_type_kind())
        {
        case edm_type_kind_t::Primitive:
            {
                auto p_value = std::dynamic_pointer_cast<odata_primitive_value>(property_value);
                auto p_primitive_type = std::dynamic_pointer_cast<edm_primitive_type>(property_type);
                return serialize_primitive_value(p_primitive_type, p_value);
            }
            break;
        case edm_type_kind_t::Enum:
            {
                auto p_value = std::dynamic_pointer_cast<odata_enum_value>(property_value);
                return serialize_enum_value(p_value);
            }
            break;
        case edm_type_kind_t::PayloadAnnotation:
            {
                auto p_value = std::dynamic_pointer_cast<odata_primitive_value>(property_value);
                return odata::utility::json::value::string(p_value->to_string());
            }
            break;
        case edm_type_kind_t::Collection:
            {
                auto p_value = std::dynamic_pointer_cast<odata_collection_value>(property_value);
                return serialize_collection_value(p_value);
            }
            break;
        case edm_type_kind_t::Complex:
        case edm_type_kind_t::Entity:
            {
                auto p_value = std::dynamic_pointer_cast<odata_structured_value>(property_value);
                return seriliaze_structured_value(p_value);
            }
            break;
        default:
            {
                throw std::runtime_error("write unsupported property type!");  
            }
            break;
        }
    }

    ::odata::utility::json::value odata_json_writer::serialize_primitive_value(const std::shared_ptr<edm_primitive_type>& p_primitive_type, const std::shared_ptr<odata_primitive_value>& p_value)
    {
        odata::utility::json::value result = odata::utility::json::value::object();
        if (!p_primitive_type || !p_value)
        {
            return result;
        }

        switch(p_primitive_type->get_primitive_kind())
        {
        case edm_primitive_type_kind_t::Boolean:
            return ::odata::utility::json::value::boolean(p_value->as<bool>());
        case edm_primitive_type_kind_t::Double:
            return ::odata::utility::json::value::number(p_value->as<double>());
        case edm_primitive_type_kind_t::Int32:
            return ::odata::utility::json::value::number(p_value->as<int32_t>());
        case edm_primitive_type_kind_t::Int16:
            return ::odata::utility::json::value::number(p_value->as<int16_t>());
        case edm_primitive_type_kind_t::Int64:
            return ::odata::utility::json::value(p_value->as<int64_t>());
        case edm_primitive_type_kind_t::Single:
            return ::odata::utility::json::value(p_value->as<float>());
        case edm_primitive_type_kind_t::SByte:
        case edm_primitive_type_kind_t::Guid:
        case edm_primitive_type_kind_t::Binary:
        case edm_primitive_type_kind_t::DateTimeOffset:
        case edm_primitive_type_kind_t::Duration:
        case edm_primitive_type_kind_t::String:
            {
                return ::odata::utility::json::value::string(p_value->to_string());
            }
            break;
        case edm_primitive_type_kind_t::Stream:
            {
                throw std::runtime_error("stream primitive value not implemented!");  
            }
            break;
        default:
            {
                throw std::runtime_error("unknown value!");  
            }
            break;
        }
    }

    ::odata::utility::json::value odata_json_writer::seriliaze_structured_value(const std::shared_ptr<odata_structured_value>& p_value)
    {
        odata::utility::json::value result = odata::utility::json::value::object();
        if (!p_value)
        {
            return result;
        }

        if(!p_value->get_context_url().is_empty())
        {
            result[ANNOTATION_ODATA_CONTEXT] = odata::utility::json::value::string(p_value->get_context_url().to_string());
        }

        if(!p_value->get_type_name().empty())
        {
            result[ANNOTATION_ODATA_TYPE] = odata::utility::json::value::string(p_value->get_type_name());
        }

        auto p_entity_value = std::dynamic_pointer_cast<odata_entity_value, odata_structured_value>(p_value);
        if (p_entity_value && !p_entity_value->get_etag().empty())
        {
            result[ANNOTATION_ODATA_ETAG] = odata::utility::json::value::string(p_entity_value->get_etag());
        }

        if (p_entity_value && !p_entity_value->get_id().is_empty())
        {
            result[ANNOTATION_ODATA_ID] = odata::utility::json::value::string(p_entity_value->get_id().to_string());
        }

        if (p_entity_value && !p_entity_value->get_edit_link().is_empty())
        {
            result[ANNOTATION_ODATA_EDIT_LINK] = odata::utility::json::value::string(p_entity_value->get_edit_link().to_string());
        }

        if (p_entity_value && !p_entity_value->get_read_link().is_empty())
        {
            result[ANNOTATION_ODATA_READ_LINK] = odata::utility::json::value::string(p_entity_value->get_read_link().to_string());
        }

        for (auto iter = p_value->properties().cbegin(); iter != p_value->properties().cend(); iter++)
        {	
            if (!iter->second)
            {
                result[iter->first] = odata::utility::json::value::null();
            }
            else
            {
                auto property_type = iter->second->get_value_type();

                if (!is_type_serializable(property_type))
                {
                    continue;
                }
                result[iter->first] = serialize_odata_value(property_type, iter->second);
            }

        }

        for (auto iter = p_value->get_actions().cbegin(); iter != p_value->get_actions().cend(); iter++)
        {
            odata::utility::json::value action = odata::utility::json::value::object();
            action[U("title")] = odata::utility::json::value::string((*iter)->get_title());
            action[U("target")] = odata::utility::json::value::string((*iter)->get_target().to_string());
            result[U("#") + (*iter)->get_name()] = action;
        }

        for (auto iter = p_value->get_functions().cbegin(); iter != p_value->get_functions().cend(); iter++)
        {
            odata::utility::json::value function = odata::utility::json::value::object();
            function[U("title")] = odata::utility::json::value::string((*iter)->get_title());
            function[U("target")] = odata::utility::json::value::string((*iter)->get_target().to_string());
            result[U("#") + (*iter)->get_name()] = function;
        }

        for (auto iter = p_value->get_instance_annotations().cbegin(); iter != p_value->get_instance_annotations().cend(); iter++)
        {
            odata::utility::json::value annotation = odata::utility::json::value::object();
            annotation[U("#") + iter->first] = serialize(iter->second);
        }

        return result;
    }

    odata::utility::json::value odata_json_writer::serialize_enum_value(const std::shared_ptr<odata_enum_value>& p_value)
    {
        if (!p_value)
        {
            return odata::utility::json::value::null();
        }

        return odata::utility::json::value::string(p_value->to_string());
    }

    odata::utility::json::value odata_json_writer::serialize_collection_value(const std::shared_ptr<odata_collection_value>& p_value)
    {
        if (!p_value)
        {
            return odata::utility::json::value::null();
        }

        odata::utility::json::value result = odata::utility::json::value::object();
        
        if(!p_value->get_context_url().is_empty())
        {
            result[ANNOTATION_ODATA_CONTEXT] = odata::utility::json::value::string(p_value->get_context_url().to_string());
        }

        if(p_value->get_count().has_value())
        {
            result[ANNOTATION_ODATA_COUNT] = odata::utility::json::value::number((double)p_value->get_count().value());
        }

        std::shared_ptr<edm_collection_type> collection_type = std::dynamic_pointer_cast<edm_collection_type, edm_named_type>(p_value->get_value_type());
        auto element_type = collection_type->get_element_type();

        std::vector<odata::utility::json::value> values = std::vector<odata::utility::json::value>();

        for (auto iter = p_value->get_collection_values().cbegin(); iter != p_value->get_collection_values().cend(); iter++)
        {
            if (!is_type_serializable(element_type))
            {
                continue;
            }

            values.push_back(serialize_odata_value(element_type, *iter));
        }

        odata::utility::json::value collection_json = odata::utility::json::value::array(values);
        result[U("value")] = collection_json;

        if(!p_value->get_next_link().is_empty())
        {
            result[ANNOTATION_ODATA_NEXT_LINK] = odata::utility::json::value::string(p_value->get_next_link().to_string());
        }

        if(!p_value->get_delta_link().is_empty())
        {
            result[ANNOTATION_ODATA_DELTA_LINK] = odata::utility::json::value::string(p_value->get_delta_link().to_string());
        }

        if (p_value->is_top_level())
        {
            return result;
        }
        else
        {
            return collection_json;
        }
    }

    bool odata_json_writer::is_type_serializable(const std::shared_ptr<edm_named_type>& property_type)
    {
        if (property_type)
        {
            if (property_type->get_type_kind() == edm_type_kind_t::Collection
                || property_type->get_type_kind() == edm_type_kind_t::Entity
                || property_type->get_type_kind() == edm_type_kind_t::Complex
                || property_type->get_type_kind() == edm_type_kind_t::Primitive
                || property_type->get_type_kind() == edm_type_kind_t::Enum
                || (property_type->get_type_kind() == edm_type_kind_t::PayloadAnnotation) 
                && (property_type->get_name() == PAYLOAD_ANNOTATION_TYPE || property_type->get_name() == PAYLOAD_ANNOTATION_ID))
            {
                return true;
            }
        }

        return false;
    }

    odata::utility::json::value odata_json_writer::serialize(std::shared_ptr<odata_service_document_element> service_document_element)
    {
        odata::utility::json::value result = odata::utility::json::value::object();
        result[JSON_NAME] = odata::utility::json::value::string(service_document_element->get_name());

        ::odata::utility::string_t kind;
        switch(service_document_element->get_kind())
        {
        case ENTITY_SET:
            kind = JSON_ENTITYSET
                break;
        case SINGLETON:
            kind = JSON_SINGLETON
                break;
        case FUNCTION_IMPORT:
            kind = JSON_FUNCTIONIMPORT
                break;
        }
        result[JSON_KIND] = odata::utility::json::value::string(kind);
        result[JSON_URI] = odata::utility::json::value::string(service_document_element->get_url());

        return result;
    }

    odata::utility::json::value odata_json_writer::serialize(std::shared_ptr<odata_error> error)
    {
        odata::utility::json::value result = odata::utility::json::value::object();
        odata::utility::json::value error_json = odata::utility::json::value::object();
        error_json[U("code")] = odata::utility::json::value::string(error->get_code());
        error_json[U("message")] = odata::utility::json::value::string(error->get_message());
        error_json[U("target")] = odata::utility::json::value::string(error->get_target());
        result[U("error")] = error_json;
        return result;
    }

    odata::utility::json::value odata_json_writer::serialize(std::shared_ptr<odata_entity_reference> entity_reference)
    {
        odata::utility::json::value result = odata::utility::json::value::object();
        
        if(!entity_reference->get_context_url().is_empty())
        {
            result[ANNOTATION_ODATA_CONTEXT] = odata::utility::json::value::string(entity_reference->get_context_url().to_string());
        }

        if (!entity_reference->get_id().is_empty())
        {
            result[ANNOTATION_ODATA_ID] = odata::utility::json::value::string(entity_reference->get_id().to_string());
        }

        return result;
    }

    odata::utility::json::value odata_json_writer::serialize(std::shared_ptr<odata_entity_reference_collection> entity_reference_collection)
    {
        odata::utility::json::value result = odata::utility::json::value::object();
        
        if(!entity_reference_collection->get_context_url().is_empty())
        {
            result[ANNOTATION_ODATA_CONTEXT] = odata::utility::json::value::string(entity_reference_collection->get_context_url().to_string());
        }

        if(!entity_reference_collection->get_next_link().is_empty())
        {
            result[ANNOTATION_ODATA_NEXT_LINK] = odata::utility::json::value::string(entity_reference_collection->get_next_link().to_string());
        }

        if(!entity_reference_collection->get_delta_link().is_empty())
        {
            result[ANNOTATION_ODATA_DELTA_LINK] = odata::utility::json::value::string(entity_reference_collection->get_delta_link().to_string());
        }

        if(entity_reference_collection->get_count().has_value())
        {
            result[ANNOTATION_ODATA_COUNT] = odata::utility::json::value::number((double)entity_reference_collection->get_count().value());
        }

        odata::utility::json::value value_json = odata::utility::json::value::array(entity_reference_collection->size());
        size_t index = 0;
        for (auto iter = entity_reference_collection->cbegin(); iter != entity_reference_collection->cend(); iter++)
        {
            value_json[index++] = serialize(*iter);
        }

        result[JSON_VALUE] = value_json;

        return result;

    }
}}