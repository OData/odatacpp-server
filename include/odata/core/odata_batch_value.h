//---------------------------------------------------------------------
// <copyright file="odata_batch_value.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/core/odata_batch_part_value.h"

namespace odata { namespace core
{
class odata_batch_value
{
    public:
    ::odata::utility::string_t get_boundary()
    {
        return m_boundary;
    }

    void set_boundary(::odata::utility::string_t boundary)
    {
        m_boundary = boundary;
    }

    std::vector<std::shared_ptr<odata_batch_part_value>>::const_iterator cbegin() const
    {
        return m_parts.cbegin();
    }

    std::vector<std::shared_ptr<odata_batch_part_value>>::const_iterator cend() const
    {
        return m_parts.cend();
    }

    void add_part(std::shared_ptr<odata_batch_part_value> batch_part_value)
	{
        m_parts.push_back(batch_part_value);
	}

    private:
    ::odata::utility::string_t m_boundary;
    std::vector<std::shared_ptr<odata_batch_part_value>> m_parts;
};
}}