//---------------------------------------------------------------------
// <copyright file="edm_model_writer.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata/edm/edm_model_writer.h"

namespace odata { namespace edm
{
void edm_model_writer::write_model(std::shared_ptr<edm_model> model)
{
    //write_start_element_with_prefix(U("edmx"), U("Edmx"), U("edmx"));
    write_start_element(U("Edmx"), U("edmx"));
    write_attribute_string(U("xmlns"), U("edmx"), U(""), U("http://docs.oasis-open.org/odata/ns/edmx"));
    write_attribute_string(U(""), U("Version"), U(""), model->get_version());

    
    write_start_element(U("DataServices"), U("edmx"));
    for(auto schema_iter = model->get_schema().cbegin(); schema_iter != model->get_schema().cend(); schema_iter++)
    {
        write_schema(*schema_iter);
    }


    write_end_element();
    write_end_element();

    finalize();
}

void edm_model_writer::write_schema(std::shared_ptr<edm_schema> schema)
{
    write_start_element(U("Schema"), U("ns"));
    write_attribute_string(U("xmlns"), U("ns"), U(""), U("http://docs.oasis-open.org/odata/ns/edm"));
    write_attribute_string(U(""), U("Namespace"), U(""), schema->get_name());

    for(auto iter = schema->get_enum_types().cbegin(); iter != schema->get_enum_types().cend(); iter++)
    {
        write_enum_type(iter->second);
    }

    for(auto iter = schema->get_complex_types().cbegin(); iter != schema->get_complex_types().cend(); iter++)
    {
        write_complex_type(iter->second);
    }

    for(auto iter = schema->get_entity_types().cbegin(); iter != schema->get_entity_types().cend(); iter++)
    {
        write_entity_type(iter->second);
    }

    for(auto iter = schema->get_operation_types().cbegin(); iter != schema->get_operation_types().cend(); iter++)
    {
        write_operation(iter->second);
    }

    for(auto iter = schema->get_containers().cbegin(); iter != schema->get_containers().cend(); iter++)
    {
        write_entity_container(iter->second);
    }

    write_end_element();
}

void edm_model_writer::write_enum_type(std::shared_ptr<edm_enum_type> enum_type)
{
    write_start_element(U("EnumType"));
    write_attribute_string(U(""), U("Name"), U(""), enum_type->get_name());
    
    for(auto iter = enum_type->get_enum_members().cbegin(); iter != enum_type->get_enum_members().cend(); iter++)
    {
        write_start_element(U("Memeber"));
        write_attribute_string(U(""), U("Name"), U(""), (*iter) -> get_enum_member_name());

        ::odata::utility::string_t str;
        ::odata::utility::stringstream_t ss;
        ss << (*iter) -> get_enum_member_value();
        ss >> str;
        write_attribute_string(U(""), U("Value"), U(""), str);
        write_end_element();
    }

    write_end_element();
}

void edm_model_writer::write_complex_type(std::shared_ptr<edm_complex_type> complex_type)
{
    write_start_element(U("ComplexType"));
    write_attribute_string(U(""), U("Name"), U(""), complex_type->get_name());

    for(auto iter = complex_type->begin(); iter != complex_type->end(); iter++)
    {
        write_start_element(U("Property"));
        write_attribute_string(U(""), U("Name"), U(""), iter->second->get_name());
        write_attribute_string(U(""), U("Type"), U(""), iter->second->get_property_type()->get_name());
        write_end_element();
    }

    write_end_element();
}

void edm_model_writer::write_entity_type(std::shared_ptr<edm_entity_type> entity_type)
{
    write_start_element(U("EntityType"));
    write_attribute_string(U(""), U("Name"), U(""), entity_type->get_name());

    write_start_element(U("Key"));

    for(auto iter = entity_type->key().cbegin(); iter != entity_type->key().cend(); iter++)
    {
        write_start_element(U("PropertyRef"));
        write_attribute_string(U(""), U("Name"), U(""), *iter);
        write_end_element();
    }

    write_end_element();

    for(auto iter = entity_type->begin(); iter != entity_type->end(); iter++)
    {
        write_start_element(U("Property"));
        write_attribute_string(U(""), U("Name"), U(""), iter->second->get_name());
        write_attribute_string(U(""), U("Type"), U(""), iter->second->get_property_type()->get_name());
        write_end_element();
    }

    write_end_element();
}

void edm_model_writer::write_operation(std::shared_ptr<edm_operation_type> operation)
{
    write_start_element(operation->is_function() ? U("Function") : U("Action"));
    write_attribute_string(U(""), U("Name"), U(""), operation->get_name());
    write_attribute_string(U(""), U("IsBound"), U(""), operation->is_bound() ? U("true") : U("false"));
    write_attribute_string(U(""), U("IsComposable"), U(""), operation->is_composable() ? U("true") : U("false"));

    for(auto iter = operation->get_operation_parameters().cbegin(); iter != operation->get_operation_parameters().cend(); iter++)
    {
        write_start_element(U("Parameter"));
        write_attribute_string(U(""), U("Name"), U(""), (*iter)->get_param_name());
        write_attribute_string(U(""), U("Type"), U(""), (*iter)->get_param_type()->get_name());
        write_end_element();
    }

    if (operation->get_operation_return_type())
    {
        write_start_element(U("ReturnType"));
        write_attribute_string(U(""), U("Type"), U(""), operation->get_operation_return_type()->get_name());
        write_end_element();
    }

    write_end_element();
}

void edm_model_writer::write_entity_container(std::shared_ptr<edm_entity_container> entity_container)
{
    write_start_element(U("EntityContainer"));
    write_attribute_string(U(""), U("Name"), U(""), entity_container->get_name());

    for(auto iter = entity_container->begin(); iter != entity_container->end(); iter++)
    {
        write_entity_set(iter->second);
    }

    for(auto iter = entity_container->get_singletons().cbegin(); iter != entity_container->get_singletons().cend(); iter++)
    {
        write_singleton(iter->second);
    }

    for(auto iter = entity_container->get_operation_imports().cbegin(); iter != entity_container->get_operation_imports().cend(); iter++)
    {
        write_operation_import(iter->second);
    }

    write_end_element();
}

void edm_model_writer::write_entity_set(std::shared_ptr<edm_entity_set> entity_set)
{
    write_start_element(U("EntitySet"));
    write_attribute_string(U(""), U("Name"), U(""), entity_set->get_name());
    write_attribute_string(U(""), U("EntityType"), U(""), entity_set->get_entity_type_name());

    for(auto iter = entity_set->get_navigation_sources().cbegin(); iter != entity_set->get_navigation_sources().cend(); iter++)
    {
        write_start_element(U("NavigationPropertyBinding"));
        write_attribute_string(U(""), U("Path"), U(""), iter->first);
        write_attribute_string(U(""), U("Target"), U(""), iter->second);
        write_end_element();
    }

    write_end_element();
}

void edm_model_writer::write_singleton(std::shared_ptr<edm_singleton> singleton)
{
    write_start_element(U("Singleton"));
    write_attribute_string(U(""), U("Name"), U(""), singleton->get_name());
    write_attribute_string(U(""), U("EntityType"), U(""), singleton->get_entity_type_name());

    for(auto iter = singleton->get_navigation_sources().cbegin(); iter != singleton->get_navigation_sources().cend(); iter++)
    {
        write_start_element(U("NavigationPropertyBinding"));
        write_attribute_string(U(""), U("Path"), U(""), iter->first);
        write_attribute_string(U(""), U("Target"), U(""), iter->second);
        write_end_element();
    }

    write_end_element();
}

void edm_model_writer::write_operation_import(std::shared_ptr<edm_operation_import> operation_import)
{
    write_start_element(operation_import->get_operation_import_kind() == OperationImportKind::FunctionImport ? U("FunctionImport") : U("ActionImport"));
    write_attribute_string(U(""), U("Name"), U(""), operation_import->get_name());
    write_attribute_string(U(""), U("EntitySet"), U(""), operation_import->get_entity_set_name());

    write_end_element();
}

}}
