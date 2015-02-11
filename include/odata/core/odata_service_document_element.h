//---------------------------------------------------------------------
// <copyright file="odata_service_document_element.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"

namespace odata { namespace core
{

enum odata_service_document_element_kind
{
	ENTITY_SET,
	SINGLETON,
    FUNCTION_IMPORT,
};

class odata_service_document_element
{
public:
    odata_service_document_element(::odata::utility::string_t url, ::odata::utility::string_t name, odata_service_document_element_kind kind) :
		m_url(url), m_name(name), m_kind(kind)
	{
	}

	::odata::utility::string_t get_url()
	{
		return m_url;
	}

	::odata::utility::string_t get_name()
	{
		return m_name;
	}

	odata_service_document_element_kind get_kind()
	{
		return m_kind;
	}

private:
	::odata::utility::string_t m_url;
	::odata::utility::string_t m_name;
	odata_service_document_element_kind m_kind;
};

}}