//---------------------------------------------------------------------
// <copyright file="odata_collection_value.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/edm/odata_edm.h"
#include "odata/core/odata_value.h"
#include "odata/common/nullable.h"

namespace odata { namespace core
{

class odata_collection_value : public odata_value
{
public:
    odata_collection_value(std::shared_ptr<::odata::edm::edm_named_type> type) : odata_value(type)
    {
    }

    void add_collection_value(std::shared_ptr<odata_value> value)
    {
        m_values.push_back(value);
    }

	void add_collection_value(const std::vector<std::shared_ptr<odata_value>>& values)
    {
		for (auto iter = values.cbegin(); iter != values.cend(); iter++)
		{
			m_values.push_back(*iter);
		}
    }

	const std::vector<std::shared_ptr<odata_value>>& get_collection_values() const 
	{
		return m_values;
	}

    void set_next_link(::odata::utility::uri value)
    {
        m_next_link = value;
    }

    const ::odata::utility::uri get_next_link() const
    {
        return m_next_link;
    }

    void set_delta_link(::odata::utility::uri value)
    {
        m_delta_link = value;
    }

    const ::odata::utility::uri get_delta_link() const
    {
        return m_delta_link;
    }

    void set_count(int64_t count)
    {
        m_count = count;
    }

    const ::odata::common::nullable<int64_t> get_count() const
    {
        return m_count;
    }

private:
    std::vector<std::shared_ptr<odata_value>>  m_values;

    ::odata::utility::uri m_next_link;
    ::odata::utility::uri m_delta_link;

    ::odata::common::nullable<int64_t> m_count;
    
};

}}