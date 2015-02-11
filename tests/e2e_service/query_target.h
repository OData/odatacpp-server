//---------------------------------------------------------------------
// <copyright file="query_target.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/edm/edm_model.h"
#include "odata/core/odata_path.h"
#include "odata/core/odata_path_segment_visitor.h"

namespace odata { namespace service
{

class query_target
{
private:
	std::shared_ptr<::odata::edm::edm_navigation_source> m_navigation_source;
	std::shared_ptr<::odata::edm::edm_named_type> m_edm_type;

public:
	query_target() {};
	~query_target() {};

	std::shared_ptr<::odata::edm::edm_named_type> target_edm_type() const;

	std::shared_ptr<::odata::edm::edm_navigation_source> target_navigation_source() const;

	static std::shared_ptr<query_target> resolve_path(std::shared_ptr<::odata::core::odata_path> path);

	void handle(std::shared_ptr<::odata::core::odata_path_segment> segment);

	void handle(std::shared_ptr<::odata::core::odata_entity_set_segment> segment);

	void handle(std::shared_ptr<::odata::core::odata_key_segment> segment);

	void handle(std::shared_ptr<::odata::core::odata_structural_property_segment> segment);

	void handle(std::shared_ptr<::odata::core::odata_navigation_property_segment> segment);

};

}}