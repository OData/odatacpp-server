//---------------------------------------------------------------------
// <copyright file="odata_exception.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/common/asyncrt_utils.h"

namespace odata { namespace core
{

/// <summary>
/// Represents an OData exception
/// </summary>
class odata_exception : public std::exception
{
public:

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="error">A string value containing the service error.</param>
    explicit odata_exception(::odata::utility::string_t error) : m_error(error)
    {
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~odata_exception() _noexcept {}

	const ::odata::utility::string_t& what()
	{
		return m_error;
	}

private:
    ::odata::utility::string_t m_error;
};

}}