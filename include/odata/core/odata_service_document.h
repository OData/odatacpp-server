//---------------------------------------------------------------------
// <copyright file="odata_service_document.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/core/odata_service_document_element.h"

namespace odata { namespace core
{

class odata_service_document
{
public:
    odata_service_document()
	{
	}

	void add_service_document_element(std::shared_ptr<odata_service_document_element> element)
	{
		m_elements.push_back(element);
	}

	 /// <summary>
    /// Gets the beginning const_iterator of the properties
    /// </summary>
    std::vector<std::shared_ptr<odata_service_document_element>>::const_iterator cbegin() const
    {
        return m_elements.cbegin();
    }

    /// <summary>
    /// Gets the end const_iterator of the properties
    /// </summary>
    std::vector<std::shared_ptr<odata_service_document_element>>::const_iterator cend() const
    {
        return m_elements.cend();
    }

	int size() const 
	{
		return (int)m_elements.size();
	}

private:
	std::vector<std::shared_ptr<odata_service_document_element>>  m_elements;
};

}}