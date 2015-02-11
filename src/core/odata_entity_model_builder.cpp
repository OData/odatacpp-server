//---------------------------------------------------------------------
// <copyright file="odata_entity_model_builder.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata/core/odata_entity_model_builder.h"
#include "odata/common/utility.h"

using namespace ::odata::edm;

namespace odata { namespace core
{

::odata::utility::string_t odata_entity_model_builder::compute_edit_link(
		const ::odata::utility::string_t& root_url, 
		std::shared_ptr<odata_entity_value> entity_value, 
		const ::odata::utility::string_t& parent_edit_link, 
		bool is_collection_navigation)
{
	::odata::utility::string_t edit_link;

	if (entity_value)
	{
		if (!entity_value->get_edit_link().is_empty())
	    {
			edit_link = entity_value->get_edit_link().to_string();

			if (::odata::utility::is_relative_path(root_url, edit_link))
			{
				edit_link = root_url + U("/") + edit_link;
			}
		}
		else
		{
			if (is_collection_navigation)
			{
				::odata::utility::string_t key_string = get_entity_key_value_string(entity_value);
				edit_link = parent_edit_link + U("(") + key_string + U(")");
			}
			else
			{
				edit_link = parent_edit_link;
			}
		}
	}

	return edit_link;
}

::odata::utility::string_t odata_entity_model_builder::get_entity_key_value_string(const std::shared_ptr<odata_entity_value>& entity_value)
{
	::odata::utility::string_t key_string;

	if (!entity_value)
	{
		return key_string;
	}

	std::vector<::odata::utility::string_t> keys;

	auto parent_type = std::dynamic_pointer_cast<edm_entity_type>(entity_value->get_value_type());
	while (parent_type)
	{
		keys.insert(keys.end(), parent_type->key().cbegin(), parent_type->key().cend());
		parent_type = std::dynamic_pointer_cast<edm_entity_type>(parent_type->get_base_type());
	}

	for (size_t i = 0; i < keys.size(); i++)
	{
		std::shared_ptr<odata_value> property_value;
		entity_value->get_property_value(keys[i], property_value);
		if (property_value)
		{
			std::shared_ptr<edm_named_type> property_type = property_value->get_value_type();
			if (property_type && property_type->get_type_kind() == edm_type_kind_t::Primitive)
			{
				auto primitive_property_value = std::dynamic_pointer_cast<odata_primitive_value>(property_value);
				if (primitive_property_value)
				{
					if (i != 0)
					{
						key_string += U(",");
					}

					key_string += primitive_property_value->to_string();
				}
			}
		}
	}

	return key_string;
}

}}