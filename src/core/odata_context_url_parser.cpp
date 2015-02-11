//---------------------------------------------------------------------
// <copyright file="odata_context_url_parser.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata/core/odata_context_url_parser.h"
#include "odata/edm/edm_model_utility.h"
#include "odata/core/odata_uri_parser.h"

using namespace ::odata::edm;

namespace odata { namespace core
{

bool odata_contex_url_parser::end_with(const ::odata::utility::string_t& s1, const ::odata::utility::string_t& s2)
{
	auto found = s1.rfind(s2);
	if (found != ::odata::utility::string_t::npos)
	{
		return found == s1.size() - s2.size();
	}

	return false;
}

std::shared_ptr<edm_named_type> odata_contex_url_parser::resolve_literal_type(const ::odata::utility::string_t& type_name)
{
	if (type_name.substr(0, 4) == U("Edm."))
	{
		return edm_model_utility::get_edm_primitive_type_from_name(type_name);
	}
	else if (type_name.find(U("Collection")) == 0)
	{
		auto inside_name = type_name.substr(11, type_name.length() - 12);
		return std::make_shared<edm_collection_type>(resolve_literal_type(inside_name));
	}
	else
	{
		std::shared_ptr<edm_named_type> ret_type;

		ret_type = m_model->find_complex_type(type_name);
		if (ret_type)
		{
			return ret_type;
		}

		ret_type = m_model->find_enum_type(type_name);
		if (ret_type)
		{
			return ret_type;
		}

		ret_type = m_model->find_entity_type(type_name);
		if (ret_type)
		{
			return ret_type;
		}

		throw std::runtime_error("Failed to parse context url");
	}
}

std::shared_ptr<edm_named_type> odata_contex_url_parser::get_payload_content_type(const ::odata::utility::string_t& context_url)
{
	if (!m_model || m_service_root_url.empty())
	{
		return nullptr;
	}

	::odata::utility::string_t path = context_url;
	::odata::utility::string_t root = m_service_root_url + U("/$metadata#");

	path = context_url.substr(context_url.find(U("$metadata#")) + 10);
	bool select_single_entity = end_with(path, U("/$entity"));
	if (select_single_entity)
	{
		path = path.substr(0, path.size()-8);
	}

	//Handle select/expand items. Right now just remove them. 
	int index_last_slash = path.rfind(U("/"));
	int index_last_bracket = path.find_first_of(U("("), ++index_last_slash);
	if (index_last_slash < index_last_bracket)
	{
		auto first_part = path.substr(0, index_last_bracket);
		if (first_part != U("Collection"))
		{
			path = first_part;
		}
	}

	if (path.find(U("/")) == ::odata::utility::string_t::npos && !select_single_entity)
	{
		//1.service document; 2.entity set or singleton; 3.primitive/complex type; 4. collection of primitive/complex type
		std::shared_ptr<edm_entity_container> container = m_model->find_container();
		if (container)
		{
			auto entity_set = container->find_entity_set(path);
			if (entity_set)
			{
				return std::make_shared<edm_collection_type>(entity_set->get_entity_type());
			}
			auto singleton = container->find_singleton(path);
			if (singleton)
			{
				return singleton->get_entity_type();
			}
		}
		
		return resolve_literal_type(path);
	}
	else
	{
		auto path_parser = std::make_shared<odata_uri_parser>(m_model);
		auto odata_path = path_parser->parse_path(U("/") + path);

		auto& segments = odata_path->segments();
		if (segments.size() == 0)
		{
			throw std::runtime_error("Invalid context url");
		}

		std::shared_ptr<odata_path_segment> last_segment;
		int index;
		for (index = segments.size() - 1; index >=0; index--)
		{
			if (segments[index]->segment_type() != odata_path_segment_type::Type)
			{
				last_segment = segments[index];
				break;
			}
		}
		
		std::shared_ptr<edm_named_type> ret_type;
		if (last_segment->segment_type() == odata_path_segment_type::EntitySet)
		{
			if (select_single_entity)
			{
				ret_type = last_segment->as<odata_entity_set_segment>()->entity_type();
			}
			else
			{
				auto entity_type = last_segment->as<odata_entity_set_segment>()->entity_type();
				ret_type = std::make_shared<edm_collection_type>(entity_type);
			}
		}
		else if (last_segment->segment_type() == odata_path_segment_type::Singleton)
		{
			ret_type = last_segment->as<odata_singleton_segment>()->entity_type();
		}
		else if (last_segment->segment_type() == odata_path_segment_type::NavigationProperty)
		{
			auto edm_navigation_type = last_segment->as<odata_navigation_property_segment>()->navigation_type();
			if (!edm_navigation_type->is_contained())
			{
				throw std::runtime_error("Invalid context url");
			}
			auto navigation_type = edm_navigation_type->get_navigation_type();
			if (navigation_type->get_type_kind() == edm_type_kind_t::Collection && select_single_entity)
			{
				auto collection_entity_type = std::dynamic_pointer_cast<edm_collection_type>(navigation_type);
				ret_type = collection_entity_type->get_element_type();
			}
			else
			{
				ret_type = navigation_type;
			}
		}
		else if (last_segment->segment_type() == odata_path_segment_type::StructuralProperty)
		{
			auto edm_property_type = last_segment->as<odata_structural_property_segment>()->property();
			auto property_type = edm_property_type->get_property_type();
			
			ret_type = property_type;
		}
		else 
		{
			throw std::runtime_error("Invalid context url");
		}

		if (index != segments.size() - 1)
		{
			last_segment = segments[segments.size() - 1];
			if (last_segment->segment_type() == odata_path_segment_type::Type)
			{
				auto target_type = last_segment->as<odata_type_segment>()->type();
				if (ret_type->get_type_kind() == edm_type_kind_t::Collection)
				{
					std::dynamic_pointer_cast<edm_collection_type>(ret_type)->set_element_type(target_type);
				}
				else
				{
					ret_type = target_type;
				}
			}
		}

		return ret_type;
	}
}

std::shared_ptr<edm_named_type> odata_contex_url_parser::parse_complex_or_primitive(const ::odata::utility::string_t& current_path)
{
	std::shared_ptr<edm_named_type> current_type = edm_model_utility::get_edm_primitive_type_from_name(current_path);

	if (!current_type)
	{
		current_type = m_model->find_complex_type(current_path);
	}

	return current_type;
}

}}