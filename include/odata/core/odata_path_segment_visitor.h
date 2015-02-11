//---------------------------------------------------------------------
// <copyright file="odata_path_segment_visitor.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/core/odata_path_segment.h"

namespace odata { namespace core
{

class odata_path_segment;
class odata_metadata_segment;

template <typename T>
class odata_path_segment_visitor
{
public:
	odata_path_segment_visitor() {}
	virtual ~odata_path_segment_visitor() {}

	virtual T visit(std::shared_ptr<::odata::core::odata_metadata_segment> segment) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_batch_segment> segment) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_entity_set_segment> segment) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_singleton_segment> segment) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_key_segment> segment) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_structural_property_segment> segment) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_navigation_property_segment> segment) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_dynamic_property_segment> segment) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_value_segment> segment) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_count_segment> segment) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_ref_segment> segment) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_type_segment> segment) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_operation_segment> segment) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_operation_import_segment> segment) = 0;
	virtual T visit_any(std::shared_ptr<::odata::core::odata_path_segment> segment) = 0;
};

}}