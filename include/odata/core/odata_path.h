//---------------------------------------------------------------------
// <copyright file="odata_path.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/core/odata_path_segment.h"

namespace odata { namespace core
{

class odata_path
{
public:
    odata_path(std::vector<std::shared_ptr<odata_path_segment>> segments)
		: m_segments(std::move(segments)) {}
	~odata_path() {}

	bool empty() const { return m_segments.empty(); }

	bool single(int segment_type) const
	{ return m_segments.size() == 1 && m_segments[0]->segment_type() == segment_type; }

	::size_t size() const { return m_segments.size(); }

	std::shared_ptr<odata_path_segment> segment_at(::size_t i) const { return m_segments[i]; }

	const std::vector<std::shared_ptr<odata_path_segment>> &segments() const { return m_segments; }

	template <typename T>
	std::vector<T> visit_with(std::shared_ptr<::odata::core::odata_path_segment_visitor<T>> visitor)
	{
		std::vector<T> result;

		for (auto it = m_segments.begin(); it != m_segments.end(); ++it)
		{
			result.push_back((*it)->accept(visitor));
		}

		return std::move(result);
	}

private:
	odata_path(const odata_path &);
	odata_path &operator=(const odata_path &);

	std::vector<std::shared_ptr<odata_path_segment>> m_segments;
};

}}