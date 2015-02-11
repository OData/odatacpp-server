//---------------------------------------------------------------------
// <copyright file="odata_format.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

namespace odata { namespace core
{

enum odata_format_type
{
    json_minimal_metadata,
    json_full_metadata,
    json_none_metadata,
};

class odata_format
{
public:
    odata_format(odata_format_type format_type) : m_format_type(format_type)
	{
	}

    odata_format_type get_format_type()
    {
        return m_format_type;
    }

private:
    odata_format_type m_format_type;
};

}}