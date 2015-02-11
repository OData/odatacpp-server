//---------------------------------------------------------------------
// <copyright file="query_target.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "query_target.h"
#include "odata_service_exception.h"

using namespace ::odata::core;
using namespace ::odata::edm;

namespace odata { namespace service
{
	std::shared_ptr<edm_named_type> query_target::target_edm_type() const
	{
		return m_edm_type;
	}

	std::shared_ptr<edm_navigation_source> query_target::target_navigation_source() const 
	{
		return m_navigation_source;
	}

	std::shared_ptr<query_target> query_target::resolve_path(std::shared_ptr<odata_path> path)
	{
		auto target = std::make_shared<query_target>();
		if (path)
		{
			for (auto iter = path->segments().cbegin(); iter != path->segments().cend(); iter++)
			{
				target->handle(*iter);
			}
		}

		return target;
	}

	void query_target::handle(std::shared_ptr<odata_path_segment> segment)
	{
		switch (segment->segment_type())
		{
		case odata_path_segment_type::EntitySet:
			handle(std::dynamic_pointer_cast<odata_entity_set_segment>(segment));
			break;
		case odata_path_segment_type::Key:
			handle(std::dynamic_pointer_cast<odata_key_segment>(segment));
			break;
		case odata_path_segment_type::StructuralProperty:
			handle(std::dynamic_pointer_cast<odata_structural_property_segment>(segment));
			break;
		case odata_path_segment_type::NavigationProperty:
			handle(std::dynamic_pointer_cast<odata_navigation_property_segment>(segment));
			break;
		default:
			throw new odata_service_exception(U("segment not supported."));
		}
	}

	void query_target::handle(std::shared_ptr<odata_entity_set_segment> segment)
	{
		m_navigation_source = segment->entity_set();
		m_edm_type = std::make_shared<edm_collection_type>(segment->entity_type());
	}

	void query_target::handle(std::shared_ptr<odata_key_segment> segment)
	{
		auto collection_type = std::dynamic_pointer_cast<edm_collection_type>(m_edm_type);
		if (collection_type)
		{
			m_edm_type = collection_type->get_element_type();
		}
	}

	void query_target::handle(std::shared_ptr<odata_structural_property_segment> segment)
	{
		m_edm_type = segment->property()->get_property_type();
	}

	void query_target::handle(std::shared_ptr<odata_navigation_property_segment> segment)
	{
		m_navigation_source = segment->navigation_type()->get_binded_navigation_source();
		m_edm_type = segment->navigation_type()->get_navigation_type();
	}

}}