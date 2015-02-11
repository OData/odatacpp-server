//---------------------------------------------------------------------
// <copyright file="edm_model_writer.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/edm/odata_edm.h"
#include "odata/common/xmlhelpers.h"

namespace odata { namespace edm
{

class edm_model_writer : public ::odata::edm::xml_writer
{
public:
    edm_model_writer(std::ostream& stream)
    {
        initialize(stream);
    }

	ODATACPP_API void write_model(std::shared_ptr<edm_model> model);

protected:
    ODATACPP_API void write_schema(std::shared_ptr<edm_schema> schema);

    ODATACPP_API void write_enum_type(std::shared_ptr<edm_enum_type> enum_type);

    ODATACPP_API void write_complex_type(std::shared_ptr<edm_complex_type> complex_type);

    ODATACPP_API void write_entity_type(std::shared_ptr<edm_entity_type> entity_type);

    ODATACPP_API void write_operation(std::shared_ptr<edm_operation_type> operation);

    ODATACPP_API void write_entity_container(std::shared_ptr<edm_entity_container> entity_container);

    ODATACPP_API void write_entity_set(std::shared_ptr<edm_entity_set> entity_set);

    ODATACPP_API void write_singleton(std::shared_ptr<edm_singleton> singleton);

    ODATACPP_API void write_operation_import(std::shared_ptr<edm_operation_import> operation_import);

private:
};

}}