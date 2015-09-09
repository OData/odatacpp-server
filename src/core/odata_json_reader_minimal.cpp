//---------------------------------------------------------------------
// <copyright file="odata_json_reader_minimal.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata/core/odata_context_url_parser.h"
#include "odata/core/odata_entity_factory.h"
#include "odata/core/odata_json_reader_minimal.h"
#include "odata/core/odata_uri_parser.h"

using namespace ::odata::edm;
using namespace ::odata::utility;

namespace odata { namespace core
{

enum RET_NAVI_ENTITY_TYPE
{
	NAVI_ENTITY_NONE = 0,
	NAVI_ENTITY_SINGLE,
	NAVI_ENTITY_COLLECTION,
	ENTITY_SINGLETON,
	ENTITY_ENTITY_SET,
};

::odata::utility::string_t odata_json_reader_minimal::get_navigation_source_from_context_url(const ::odata::utility::string_t& context_url)
{
    ::odata::utility::string_t ret = context_url;

	int index = (int)ret.find(U("#"));
	ret = ret.substr(index + 1, ret.length() - index - 1);

	index = (int)ret.find(U("/$entity"));
	if (index != -1)
	{
		ret = ret.substr(0, index);
	}

	int index_last_slash = (int)ret.rfind(U("/"));

	int index_last_bracket = (int)ret.find_first_of(U("("), ++index_last_slash);
	if (index_last_slash < index_last_bracket)
	{
		ret = ret.substr(0, index_last_bracket);
	}

	return ret;
}

void odata_json_reader_minimal::set_edit_link_for_entity_collection_value(const std::shared_ptr<odata_collection_value>& entity_collection_value, const ::odata::utility::string_t& expect_type_name, 
									const ::odata::utility::string_t& navigation_source)
{
	if (!entity_collection_value)
	{
		return ;
	}

	auto collection_type = std::dynamic_pointer_cast<edm_collection_type>(entity_collection_value->get_value_type());
	if (!collection_type || collection_type->get_element_type()->get_type_kind() != edm_type_kind_t::Entity)
	{
		return;
	}

	for (auto iter = entity_collection_value->get_collection_values().cbegin(); iter != entity_collection_value->get_collection_values().cend(); iter++)
	{
		auto entity_value = std::dynamic_pointer_cast<odata_entity_value>(*iter);
		if (!entity_value || !entity_value->get_edit_link().is_empty())
		{
			continue;
		}

		auto key_string = entity_value->get_entity_key_string();
		entity_value->set_edit_link(navigation_source + key_string);

		// check to see if it is an derived type
		if (expect_type_name != entity_value->get_value_type()->get_name())
		{
			::odata::utility::string_t derived_edit_link = navigation_source + key_string;
			derived_edit_link += U("/");
			if (entity_value->has_property(PAYLOAD_ANNOTATION_TYPE))
			{
				::odata::utility::string_t derived_type;
				entity_value->try_get(PAYLOAD_ANNOTATION_TYPE, derived_type);
				if (!derived_type.empty() && derived_type[0] == U('#'))
					derived_type = derived_type.substr(1, derived_type.length() - 1);
				derived_edit_link += derived_type;
				entity_value->set_edit_link(derived_edit_link);
			}
		}
	}
}


void odata_json_reader_minimal::set_edit_link_for_entity_value(const std::shared_ptr<odata_entity_value>& entity_value, const ::odata::utility::string_t& expect_type_name, 
									const ::odata::utility::string_t& navigation_source)
{
	if (!entity_value || !entity_value->get_edit_link().is_empty())
	{
		return ;
	}

	auto path_parser = std::make_shared<odata_uri_parser>(m_model);
	::odata::utility::string_t relative_path = navigation_source;
	if (relative_path.find(m_service_root_url) == 0)
	{
		relative_path = relative_path.substr(m_service_root_url.size());
	}
	if (relative_path.find(U("/")) != 0)
	{
		relative_path = U("/") + relative_path;
	}
	auto odata_path = path_parser->parse_path(relative_path);

	auto& segments = odata_path->segments();
	if (segments.size() == 0)
	{
		throw std::runtime_error("Invalid context url");
	}
	std::shared_ptr<odata_path_segment> last_segment;
	for (int i = (int)segments.size() - 1; i >=0; i--)
	{
		if (segments[i]->segment_type() != odata_path_segment_type::Type)
		{
			last_segment = segments[i];
			break;
		}
	}

	bool is_collection = true;
	if (last_segment->segment_type() == odata_path_segment_type::Singleton)
	{
		is_collection = false;
	}
	else if (last_segment->segment_type() == odata_path_segment_type::NavigationProperty)
	{
		auto navigation_type = last_segment->as<odata_navigation_property_segment>()->navigation_type()->get_navigation_type();
		if (navigation_type->get_type_kind() == edm_type_kind_t::Entity)
		{
			is_collection = false;
		}
	}

	auto key_string = is_collection ? entity_value->get_entity_key_string() : U("");
	entity_value->set_edit_link(navigation_source + key_string);

	// check to see if it is an derived type
	if (expect_type_name != entity_value->get_value_type()->get_name())
	{
		::odata::utility::string_t derived_edit_link = navigation_source + key_string;
		derived_edit_link += U("/");
		if (entity_value->has_property(PAYLOAD_ANNOTATION_TYPE))
		{
			::odata::utility::string_t derived_type;
			entity_value->try_get(PAYLOAD_ANNOTATION_TYPE, derived_type);
			if (!derived_type.empty() && derived_type[0] == U('#'))
				derived_type = derived_type.substr(1, derived_type.length() - 1);
			derived_edit_link += derived_type;
			entity_value->set_edit_link(derived_edit_link);
		}
	}
}

std::shared_ptr<edm_named_type> odata_json_reader_minimal::prepare_for_reading(const odata::utility::json::value& content, std::shared_ptr<edm_named_type> expected_type = nullptr)
{
	::odata::utility::string_t context_url;
	if (content.has_field(U("@odata.context")))
	{
		odata::utility::json::value annotation_value = content.at(U("@odata.context"));
		context_url = annotation_value.is_null() ? U("") : annotation_value.as_string();
	}

	std::shared_ptr<edm_named_type> context_url_parsed_type;
	if (!context_url.empty())
	{
		auto context_url_parser = entity_factory<odata_contex_url_parser>::create_context_url_parser(m_model, m_service_root_url);
		context_url_parsed_type = context_url_parser->get_payload_content_type(context_url);
	}

	std::shared_ptr<edm_named_type> payload_type;
	if (m_is_reading_response) //if it's response message
	{
		//Either the entity type is specified for reading, or the context url is present in payload.

		if (context_url.empty() || !context_url_parsed_type)
		{
			throw std::runtime_error("Failed to parse context url in payload");
		}
		
		if (expected_type)
		{
			//TODO: verify the type parsed from context url

			payload_type = expected_type;
		}
		else
		{
			payload_type = context_url_parsed_type;
		}
	}
	else
	{
		//Must specify entity type for reading
		if (!expected_type)
		{
			throw std::runtime_error("Entity type is not specified for reading message");
		}

		payload_type = expected_type;
	}

	return payload_type;
}

std::shared_ptr<odata_collection_value> odata_json_reader_minimal::deserilize_entity_collection(const odata::utility::json::value& content, std::shared_ptr<edm_entity_set> entity_set)
{
	std::shared_ptr<edm_collection_type> collection_type;
	if (entity_set)
	{
		collection_type = std::make_shared<edm_collection_type>(entity_set->get_entity_type());
	}
	std::shared_ptr<edm_named_type> resolved_type = prepare_for_reading(content, collection_type);

	auto resolved_collection_type = std::dynamic_pointer_cast<edm_collection_type>(resolved_type);
	if (!resolved_collection_type || resolved_collection_type->get_type_kind() != edm_type_kind_t::Collection)
	{
		throw std::runtime_error("the resolved type must be a collection type");
	}
	
	auto element_type = resolved_collection_type->get_element_type();
	if (!element_type || element_type->get_type_kind() != edm_type_kind_t::Entity)
	{
		throw std::runtime_error("the element type must be an entity type");
	}

	if (!content.has_field(U("value")))
	{
		throw std::runtime_error("invalid payload format");
	}

	auto collection_value = handle_extract_collection_property(resolved_collection_type, content.at(U("value")));

	if (m_is_reading_response)
	{
		::odata::utility::string_t context_url = content.at(U("@odata.context")).as_string();

		::odata::utility::string_t navigation_source = get_navigation_source_from_context_url(context_url);
		navigation_source = m_service_root_url + U("/") + navigation_source;

		set_edit_link_for_entity_collection_value(collection_value, element_type->get_name(), navigation_source);

		if (content.has_field(U("@odata.nextLink")))
		{
			auto annotation_value = content.at(U("@odata.nextLink"));
			collection_value->set_next_link(annotation_value.is_null() ? U("") : annotation_value.as_string());		
		}
	}

	collection_value->set_is_top_level(true);
	return collection_value;
}

std::shared_ptr<odata_entity_value> odata_json_reader_minimal::deserilize_entity_value(const odata::utility::json::value& content, std::shared_ptr<edm_entity_type> entity_type)
{
	std::shared_ptr<edm_named_type> resolved_type = prepare_for_reading(content, entity_type);

	auto resolved_entity_type = std::dynamic_pointer_cast<edm_entity_type>(resolved_type);
	if (!resolved_entity_type || resolved_entity_type->get_type_kind() != edm_type_kind_t::Entity)
	{
		throw std::runtime_error("the resolved type must be an entity type");
	}

	auto entity_value = handle_extract_entity_property(content, resolved_entity_type);

	if (m_is_reading_response)
	{
		::odata::utility::string_t context_url = content.at(U("@odata.context")).as_string();

		::odata::utility::string_t navigation_source = get_navigation_source_from_context_url(context_url);
		navigation_source = m_service_root_url + U("/") + navigation_source;

		set_edit_link_for_entity_value(entity_value, resolved_entity_type->get_name(), navigation_source);
	}

	entity_value->set_is_top_level(true);
	return entity_value;
}

std::shared_ptr<odata_value> odata_json_reader_minimal::deserilize_property(const odata::utility::json::value& content, std::shared_ptr<edm_named_type> edm_type)
{
	std::shared_ptr<edm_named_type> resolved_type = prepare_for_reading(content, edm_type);
	if (!resolved_type)
	{
		throw std::runtime_error("cannot resolve type for the payload");
	}

	switch (resolved_type->get_type_kind())
	{
	case edm_type_kind_t::Primitive:
		{
			auto primitive_type = std::dynamic_pointer_cast<edm_primitive_type>(resolved_type);
			if (!content.has_field(U("value")))
			{
				throw std::runtime_error("invalid payload format");
			}
			auto primitive_value = std::make_shared<odata_primitive_value>(primitive_type, strip_string(content.at(U("value")).serialize()));

			primitive_value->set_is_top_level(true);
			return primitive_value;
		}
		break;
	case edm_type_kind_t::Complex:
		{
			auto complex_type = std::dynamic_pointer_cast<edm_complex_type>(resolved_type);

			auto complex_value = handle_extract_complex_property(content, complex_type);

			complex_value->set_is_top_level(true);
			return complex_value;
		}
		break;
	case edm_type_kind_t::Enum:
		{
			auto enum_type = std::dynamic_pointer_cast<edm_enum_type>(resolved_type);
			if (!content.has_field(U("value")))
			{
				throw std::runtime_error("invalid payload format");
			}
			auto enum_value = std::make_shared<odata_enum_value>(enum_type, strip_string(content.at(U("value")).serialize()));

			enum_value->set_is_top_level(true);
			return enum_value;
		}
		break;
	case edm_type_kind_t::Collection:
		{
			auto collection_type = std::dynamic_pointer_cast<edm_collection_type>(resolved_type);
			auto element_type = collection_type->get_element_type();
			auto element_type_kind = element_type->get_type_kind();
			if (element_type_kind != edm_type_kind_t::Primitive && element_type_kind != edm_type_kind_t::Enum && element_type_kind != edm_type_kind_t::Complex)
			{
				throw std::runtime_error("the resolved type is not a property type");
			}
			if (!content.has_field(U("value")))
			{
				throw std::runtime_error("invalid payload format");
			}
			auto collection_value = handle_extract_collection_property(collection_type, content.at(U("value")));

			collection_value->set_is_top_level(true);
			return collection_value;
		}
		break;
	default:
		{
			throw std::runtime_error("cannot resolve the expected type");
		}
	}
}

std::shared_ptr<odata_value> odata_json_reader_minimal::deserilize(const odata::utility::json::value& content)
{
	auto return_type = prepare_for_reading(content);
	::odata::utility::string_t context_url = content.at(U("@odata.context")).as_string();

	std::shared_ptr<odata_value> return_value;

	if (!return_type)
	{
		return return_value;
	}

	::odata::utility::string_t edit_link = get_navigation_source_from_context_url(context_url);
	edit_link = m_service_root_url + U("/") + edit_link;
	
	auto return_type_kind = return_type->get_type_kind();

	if (return_type_kind == edm_type_kind_t::Collection)
	{
		if (!content.has_field(U("value")))
		{
			throw std::runtime_error("invalid payload format");
		}

		auto collection_value = handle_extract_collection_property(return_type, content.at(U("value")));
		set_edit_link_for_entity_collection_value(collection_value, std::dynamic_pointer_cast<edm_collection_type>(return_type)->get_element_type()->get_name(), edit_link);

		if (content.has_field(U("@odata.nextLink")))
		{
			odata::utility::json::value annotation_value = content.at(U("@odata.nextLink"));
			collection_value->set_next_link(annotation_value.is_null() ? U("") : annotation_value.as_string());
		}

		return_value = collection_value;
	}
	else if (return_type_kind == edm_type_kind_t::Entity)
	{
		auto entity_return_type = std::dynamic_pointer_cast<edm_entity_type>(return_type);
			
		auto entity_value = handle_extract_entity_property(content, entity_return_type);

		// calculate edit link if necessary
		if (entity_value)
		{
			set_edit_link_for_entity_value(entity_value, return_type->get_name(), edit_link);
		}

		return_value = entity_value;
	}
	else if (return_type_kind == edm_type_kind_t::Primitive)
	{
		if (!content.has_field(U("value")))
		{
			throw std::runtime_error("invalid payload format");
		}
		auto primitive_value = std::make_shared<odata_primitive_value>(return_type, strip_string(content.at(U("value")).serialize()));

		return_value = primitive_value;
	}
	else if (return_type_kind == edm_type_kind_t::Complex)
	{
		auto complex_return_type = std::dynamic_pointer_cast<edm_complex_type>(return_type);

		return_value = handle_extract_complex_property(content, complex_return_type);
	}
	else if (return_type_kind == edm_type_kind_t::Enum)
	{
		if (!content.has_field(U("value")))
		{
			throw std::runtime_error("invalid payload format");
		}
		auto enum_value = std::make_shared<odata_enum_value>(return_type, strip_string(content.at(U("value")).serialize()));

		return_value = enum_value;
	}

	return_value->set_is_top_level(true);
	return return_value;
}

std::shared_ptr<odata_value> odata_json_reader_minimal::handle_extract_navigation_property(const odata::utility::json::value& value, std::shared_ptr<edm_navigation_type> navigation_type)
{
	if (!navigation_type)
	{
		return nullptr;
	}

	switch(value.type())
	{
	case json::value::Array:
		{
			auto collection = std::dynamic_pointer_cast<::odata::edm::edm_collection_type>(navigation_type->get_navigation_type());

			if (collection)
			{
				return handle_extract_collection_property(collection, value);
			}
		}
		break;
    case json::value::Object:
		{
			auto navi_entity_type = std::dynamic_pointer_cast<::odata::edm::edm_entity_type>(navigation_type->get_navigation_type());

			return handle_extract_entity_property(value, navi_entity_type);
		}
		break;
	default:
		break;
	}

	return nullptr;
}

std::shared_ptr<odata_entity_value> odata_json_reader_minimal::handle_extract_entity_property(const odata::utility::json::value& value, std::shared_ptr<edm_entity_type>& entity_type)
{
	if (!entity_type)
	{
		return nullptr;
	}

	auto ret_value = std::make_shared<odata_entity_value>(entity_type);

	// find odata.type and check odata.type to see if it is a derived type
	if (value.has_field(PAYLOAD_ANNOTATION_TYPE))
	{
		auto annotation_type = 	value.at(PAYLOAD_ANNOTATION_TYPE);
		auto annotation_value = annotation_type.as_string();
		ret_value->set_value(PAYLOAD_ANNOTATION_TYPE, std::make_shared<odata_primitive_value>(std::make_shared<edm_payload_annotation_type>(PAYLOAD_ANNOTATION_TYPE), annotation_value));
		annotation_value = annotation_value.substr(1, annotation_value.length() - 1);
		auto ret_entity_type = m_model->find_entity_type(annotation_value);
		if (ret_entity_type)
		{
			entity_type = ret_entity_type;
			ret_value->set_value_type(entity_type);
		}
	}

	if (value.has_field(PAYLOAD_ANNOTATION_EDITLINK))
	{
		auto annotation_type = 	value.at(PAYLOAD_ANNOTATION_EDITLINK);
		auto annotation_value = annotation_type.as_string();
		if (::odata::utility::is_relative_path(m_service_root_url, annotation_value))
		{
			annotation_value = m_service_root_url + U("/") + annotation_value;
		}
		ret_value->set_edit_link(annotation_value);
	}

	odata::utility::json::value original_value = value;
	
    for (auto iter = value.as_object().begin(); iter != value.as_object().end(); ++iter)
    {
        auto name = iter->first;
        auto& value = iter->second;

        auto index = name.find(U("odata."));
        if (index == ::odata::utility::string_t::npos)
        {
			auto prop = entity_type->find_property(name);
			if (prop && prop->get_property_type() && prop->get_property_type()->get_type_kind() == edm_type_kind_t::Navigation)
			{
				// process navigation field
				auto navigation_type = std::dynamic_pointer_cast<edm_navigation_type>(prop->get_property_type());
				if (navigation_type)
				{
			        ::odata::utility::string_t context_url_property = name + U("@odata.context");
					::odata::utility::string_t edit_link;
					if (original_value.has_field(context_url_property))
					{
						edit_link = original_value[context_url_property].as_string();
						edit_link = get_navigation_source_from_context_url(edit_link);
					}
					else if (!navigation_type->is_contained())
					{
						// not a contained navigation property
						auto binded_source = navigation_type->get_binded_navigation_source();
						if (binded_source)
						{
							edit_link = binded_source->get_name();
						}
					}

					auto navigation_value = handle_extract_navigation_property(value, navigation_type);
					ret_value->set_value(name, navigation_value);

					if (edit_link.empty() || !m_is_reading_response)
					{
						continue;
					}
					edit_link = m_service_root_url + U("/") + edit_link;


                    // set edit_link
					if (navigation_value->get_value_type()->get_type_kind() == edm_type_kind_t::Collection)
					{
						auto collection_value = std::dynamic_pointer_cast<odata_collection_value>(navigation_value);
						
						if (collection_value)
						{
							set_edit_link_for_entity_collection_value(collection_value, collection_value->get_value_type()->get_name(), edit_link);
						}
					}
					else if (navigation_value->get_value_type()->get_type_kind() == edm_type_kind_t::Entity)
					{
						auto element_value = std::dynamic_pointer_cast<odata_entity_value>(navigation_value);
						if (element_value)
						{
							set_edit_link_for_entity_value(element_value, element_value->get_value_type()->get_name(), edit_link);
						}
					}
				}
			}
			else
			{
				// Process fields that are not annotations
				switch(value.type())
				{
				case json::value::Array:
					{
						// A collection
						auto collection_prop = entity_type->find_property(name);
						if (!collection_prop)
						{
							continue;
						}

						ret_value->set_value(name, handle_extract_collection_property(collection_prop->get_property_type(), value));
					}
					break;
				case json::value::Object:
					{
						// A complex
						auto complex_prop = entity_type->find_property(name);
						if (!complex_prop)
						{
							// to do maybe the element is an entity
							continue;
						}

						auto a = complex_prop->get_property_type();
						auto b = a->get_name();

						auto ct_type = m_model->find_complex_type(complex_prop->get_property_type()->get_name());
						if (ct_type)
						{
							ret_value->set_value(name, handle_extract_complex_property(value, ct_type));
						}
					}
					break;
				case json::value::Null:
					{
						auto value_prop = entity_type->find_property(name); 
						if (!value_prop)
						{
							continue;
						}

						std::shared_ptr<odata_value> null_odata_value;;
						ret_value->set_value(name, null_odata_value);
					}
					break;
				default:
					{
						auto primitive_prop = entity_type->find_property(name);
						if (!primitive_prop)
						{
							continue;
						}

						if (primitive_prop->get_property_type()->get_type_kind() == Enum)
						{
							ret_value->set_value(name, std::make_shared<odata_enum_value>(primitive_prop->get_property_type(), strip_string(value.serialize())));
						}
						else
						{
							ret_value->set_value(name, std::make_shared<odata_primitive_value>(primitive_prop->get_property_type(), strip_string(value.serialize())));
						}
					}
					break;
				}
			}
        }
        else
        {
            auto annotation = name.substr(index - 1);
            if (annotation == PAYLOAD_ANNOTATION_NAVIGATIONLINK)
            {
                auto pname = name.substr(0, index - 1);
				auto navigation_prop = entity_type->find_property(pname);
				if (!navigation_prop)
				{
					continue;
				}

				ret_value->set_value(name, std::make_shared<odata_primitive_value>(navigation_prop->get_property_type(), strip_string(value.serialize())));
            }
			else
			{
				if (name.find(U("@")) == 0)
				{
					auto annotation = name.substr(index - 1);
					auto annotation_value = strip_string(value.serialize());

					if (annotation == PAYLOAD_ANNOTATION_READLINK)
					{
						ret_value->set_value(PAYLOAD_ANNOTATION_READLINK, std::make_shared<odata_primitive_value>(std::make_shared<edm_payload_annotation_type>(PAYLOAD_ANNOTATION_READLINK), annotation_value));
					}
					else if (annotation == PAYLOAD_ANNOTATION_ID)
					{
						ret_value->set_value(PAYLOAD_ANNOTATION_ID, std::make_shared<odata_primitive_value>(std::make_shared<edm_payload_annotation_type>(PAYLOAD_ANNOTATION_ID), annotation_value));
					}
				}
			}
        }
    }

    return ret_value;
}

std::shared_ptr<odata_complex_value> odata_json_reader_minimal::handle_extract_complex_property(const ::odata::utility::json::value& value, std::shared_ptr<::odata::edm::edm_complex_type>& edm_complex_type)
{
	if (!edm_complex_type)
	{
		return nullptr;
	}

	auto ret_value = std::make_shared<odata_complex_value>(edm_complex_type);

	// find odata.type and check odata.type to see if it is a derived type
	if (value.has_field(PAYLOAD_ANNOTATION_TYPE))
	{
		auto annotation_type = 	value.at(PAYLOAD_ANNOTATION_TYPE);
		auto annotation_value = annotation_type.as_string();
		ret_value->set_value(PAYLOAD_ANNOTATION_TYPE, std::make_shared<odata_primitive_value>(std::make_shared<edm_payload_annotation_type>(PAYLOAD_ANNOTATION_TYPE), annotation_value));
		annotation_value = annotation_value.substr(1, annotation_value.length() - 1);
		auto ret_complex_type = m_model->find_complex_type(annotation_value);
		if (ret_complex_type)
		{
			edm_complex_type = ret_complex_type;
			ret_value->set_value_type(edm_complex_type);
		}
	}

	for (auto iter = value.as_object().begin(); iter != value.as_object().end(); ++iter)
    {
        auto name = iter->first;
        auto& value = iter->second;

        auto index = name.find(U("odata."));
        if (index == ::odata::utility::string_t::npos)
        {
            // Process fields that are not annotations
            switch(value.type())
            {
            case json::value::Array:
				{
                    // A collection
				    auto collection_prop = edm_complex_type->find_property(name);
					if (!collection_prop)
					{
						continue;
					}

					ret_value->set_value(name, handle_extract_collection_property(collection_prop->get_property_type(), value));
				}
                break;
            case json::value::Object:
                {
                    // A complex
					auto complext_prop = edm_complex_type->find_property(name);
					if (!complext_prop)
					{
						continue;
					}

                    auto ct_type = m_model->find_complex_type(complext_prop->get_property_type()->get_name());
                    if (ct_type)
					{
						ret_value->set_value(name, handle_extract_complex_property(value, ct_type));
					}

                }
				break;
			case json::value::Null:
				{
					auto value_prop = edm_complex_type->find_property(name);
					if (!value_prop)
					{
						// to do maybe the element is an entity
						continue;
					}

					ret_value->set_value(name, (std::nullptr_t)nullptr);
				}
				break;
            default:
                {
					auto primitive_prop = edm_complex_type->find_property(name);
					if (!primitive_prop)
					{
						continue;
					}

						if (primitive_prop->get_property_type()->get_type_kind() == Enum)
						{
							ret_value->set_value(name, std::make_shared<odata_enum_value>(primitive_prop->get_property_type(), strip_string(value.serialize())));
						}
						else
						{
							ret_value->set_value(name, std::make_shared<odata_primitive_value>(primitive_prop->get_property_type(), strip_string(value.serialize())));
						}
                }
                break;
            }
        }
        else
        {
			if (name.find(U("@")) == 0)
			{
				auto annotation = name.substr(index - 1);
				auto annotation_value = strip_string(value.serialize());

				if (annotation == PAYLOAD_ANNOTATION_READLINK)
				{
					ret_value->set_value(PAYLOAD_ANNOTATION_READLINK, std::make_shared<odata_primitive_value>(std::make_shared<edm_payload_annotation_type>(PAYLOAD_ANNOTATION_READLINK), annotation_value));
				}
				else if (annotation == PAYLOAD_ANNOTATION_ID)
				{
					ret_value->set_value(PAYLOAD_ANNOTATION_ID, std::make_shared<odata_primitive_value>(std::make_shared<edm_payload_annotation_type>(PAYLOAD_ANNOTATION_ID), annotation_value));
				}
			}
			
        }
    }

    return ret_value;
}

std::shared_ptr<odata_collection_value> odata_json_reader_minimal::handle_extract_collection_property(std::shared_ptr<edm_named_type> type, const odata::utility::json::value& value)
{
    if (!type)
	{
		return nullptr;
	}
                    
	// get elements of collection
	auto p_edm_collection_type = std::dynamic_pointer_cast<edm_collection_type>(type);
	if (!p_edm_collection_type)
	{
		return nullptr;
	}

	auto element_type = p_edm_collection_type->get_element_type();
	if (!element_type)
	{
		return nullptr;
	}
	
	auto p_collection_property = std::make_shared<odata_collection_value>(type);

	for (auto iter = value.as_array().begin(); iter != value.as_array().end(); iter++)
	{
		odata::utility::json::value element_value = *iter;

		if (element_type->get_type_kind() == edm_type_kind_t::Primitive)
		{
			p_collection_property->add_collection_value(std::make_shared<odata_primitive_value>(element_type, strip_string(element_value.serialize())));
		}
		else if (element_type->get_type_kind() == edm_type_kind_t::Complex)
		{
			auto ct_type = m_model->find_complex_type(element_type->get_name());
			if (ct_type)
			{
                p_collection_property->add_collection_value(handle_extract_complex_property(element_value, ct_type));
			}
		}
		else if (element_type->get_type_kind() == edm_type_kind_t::Entity)
		{
			auto entity_element_type = std::dynamic_pointer_cast<edm_entity_type>(element_type);
			
			p_collection_property->add_collection_value(handle_extract_entity_property(element_value, entity_element_type));
		}
		else if (element_type->get_type_kind() == edm_type_kind_t::Enum)
		{
			p_collection_property->add_collection_value(std::make_shared<odata_enum_value>(element_type, strip_string(element_value.serialize())));
		}
		else
		{
			throw std::runtime_error("Can't parse odata_payload contains collection of collection!");
		}
	}

	return p_collection_property;
}

void odata_json_reader_minimal::handle_extract_entity_annotation(const ::odata::utility::string_t& annotation, const ::odata::utility::string_t& value, std::shared_ptr<odata_structured_value>& entity_value)
{
	if (annotation == PAYLOAD_ANNOTATION_READLINK)
	{
		entity_value->set_value(PAYLOAD_ANNOTATION_READLINK, std::make_shared<odata_primitive_value>(std::make_shared<edm_payload_annotation_type>(PAYLOAD_ANNOTATION_READLINK), value));
	}
	else if (annotation == PAYLOAD_ANNOTATION_ID)
	{
		entity_value->set_value(PAYLOAD_ANNOTATION_ID, std::make_shared<odata_primitive_value>(std::make_shared<edm_payload_annotation_type>(PAYLOAD_ANNOTATION_ID), value));
	}
	else if (annotation == PAYLOAD_ANNOTATION_TYPE)
	{
		entity_value->set_value(PAYLOAD_ANNOTATION_TYPE, std::make_shared<odata_primitive_value>(std::make_shared<edm_payload_annotation_type>(PAYLOAD_ANNOTATION_TYPE), value));
	}
}

}}