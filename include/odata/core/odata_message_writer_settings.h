//---------------------------------------------------------------------
// <copyright file="odata_message_writer_settings.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once
#include "odata/core/odata_format.h"

namespace odata { namespace core
{

class odata_message_writer_settings
{
public:
    odata_message_writer_settings(odata_format format) : m_format(format)
	{
	}

    odata_message_writer_settings() : m_format(odata_format(odata_format_type::json_minimal_metadata))
    {
    }

    odata_format get_format()
    {
        return m_format;
    }

private:
    odata_format m_format;   
};

}}