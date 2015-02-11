//---------------------------------------------------------------------
// <copyright file="odata_path_segment.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/edm/odata_edm.h"
#include "odata/core/odata_primitive_value.h"

namespace odata { namespace core
{

namespace odata_path_segment_type {
enum {
	None,
	Metadata,
	Batch,
	EntitySet,
	Singleton,
	Key,
    StructuralProperty,
	NavigationProperty,
	DynamicProperty,
    Value,
	Count,
	Ref,
    Type,
	Operation,
	OperationImport,
};
}

template <typename T>
class odata_path_segment_visitor;

class odata_metadata_segment;
class odata_batch_segment;
class odata_entity_set_segment;
class odata_singleton_segment;
class odata_key_segment;
class odata_structural_property_segment;
class odata_navigation_property_segment;
class odata_dynamic_property_segment;
class odata_value_segment;
class odata_count_segment;
class odata_ref_segment;
class odata_type_segment;
class odata_operation_segment;
class odata_operation_import_segment;

class odata_path_segment : public std::enable_shared_from_this<odata_path_segment>
{
public:
	odata_path_segment()
		: m_segment_type(odata_path_segment_type::None) {}
	virtual ~odata_path_segment() {}

	int segment_type() const { return m_segment_type; }

	template <typename T> std::shared_ptr<T> as()
	{
		return std::static_pointer_cast<T>(shared_from_this());
	}

	ODATACPP_API static std::shared_ptr<odata_path_segment> create_metadata_segment();
	ODATACPP_API static std::shared_ptr<odata_path_segment> create_batch_segment();
	ODATACPP_API static std::shared_ptr<odata_path_segment> create_entity_set_segment(std::shared_ptr<::odata::edm::edm_entity_set> entity_set);
	ODATACPP_API static std::shared_ptr<odata_path_segment> create_singleton_segment(std::shared_ptr<::odata::edm::edm_singleton> singleton);
	ODATACPP_API static std::shared_ptr<odata_path_segment> create_key_segment(
        std::shared_ptr<::odata::edm::edm_navigation_source> navigation_source,
		std::shared_ptr<::odata::edm::edm_entity_type> target_entity_type,
        std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> keys);
    ODATACPP_API static std::shared_ptr<odata_path_segment> create_structural_property_segment(
        std::shared_ptr<::odata::edm::edm_structured_type> owning_type,
        std::shared_ptr<::odata::edm::edm_property_type> property);
	ODATACPP_API static std::shared_ptr<odata_path_segment> create_navigation_property_segment(
        std::shared_ptr<::odata::edm::edm_structured_type> owning_type,
		std::shared_ptr<::odata::edm::edm_property_type> property,
        std::shared_ptr<::odata::edm::edm_navigation_type> navigation_type);
	ODATACPP_API static std::shared_ptr<odata_path_segment> create_dynamic_property_segment(const ::odata::utility::string_t &property_name);
    ODATACPP_API static std::shared_ptr<odata_path_segment> create_value_segment();
	ODATACPP_API static std::shared_ptr<odata_path_segment> create_count_segment();
	ODATACPP_API static std::shared_ptr<odata_path_segment> create_ref_segment();
    ODATACPP_API static std::shared_ptr<odata_path_segment> create_type_segment(std::shared_ptr<::odata::edm::edm_named_type> type);
	ODATACPP_API static std::shared_ptr<odata_path_segment> create_operation_import_segment(
		std::shared_ptr<::odata::edm::edm_operation_import> operation_import,
		std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> parameters);
	ODATACPP_API static std::shared_ptr<odata_path_segment> create_operation_segment(
		std::shared_ptr<::odata::edm::edm_operation_type> operation,
		std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> parameters);

	template <typename T>
	T accept(std::shared_ptr<::odata::core::odata_path_segment_visitor<T>> visitor)
	{
		switch (m_segment_type)
		{
		case odata_path_segment_type::Metadata:
			return visitor->visit(this->as<::odata::core::odata_metadata_segment>());
		case odata_path_segment_type::Batch:
			return visitor->visit(this->as<::odata::core::odata_batch_segment>());
		case odata_path_segment_type::EntitySet:
			return visitor->visit(this->as<::odata::core::odata_entity_set_segment>());
		case odata_path_segment_type::Singleton:
			return visitor->visit(this->as<::odata::core::odata_singleton_segment>());
		case odata_path_segment_type::Key:
			return visitor->visit(this->as<::odata::core::odata_key_segment>());
		case odata_path_segment_type::StructuralProperty:
			return visitor->visit(this->as<::odata::core::odata_structural_property_segment>());
		case odata_path_segment_type::NavigationProperty:
			return visitor->visit(this->as<::odata::core::odata_navigation_property_segment>());
		case odata_path_segment_type::DynamicProperty:
			return visitor->visit(this->as<::odata::core::odata_dynamic_property_segment>());
		case odata_path_segment_type::Value:
			return visitor->visit(this->as<::odata::core::odata_value_segment>());
		case odata_path_segment_type::Count:
			return visitor->visit(this->as<::odata::core::odata_count_segment>());
		case odata_path_segment_type::Ref:
			return visitor->visit(this->as<::odata::core::odata_ref_segment>());
		case odata_path_segment_type::Type:
			return visitor->visit(this->as<::odata::core::odata_type_segment>());
		case odata_path_segment_type::Operation:
			return visitor->visit(this->as<::odata::core::odata_operation_segment>());
		case odata_path_segment_type::OperationImport:
			return visitor->visit(this->as<::odata::core::odata_operation_import_segment>());
		}

		return visitor->visit_any(shared_from_this());
	}

protected:
	odata_path_segment(int segment_type)
		: m_segment_type(segment_type) {}
	
private:
	// Make odata_path_segment and all its subclasses non-copyable.
	odata_path_segment(const odata_path_segment &);
	odata_path_segment &operator=(const odata_path_segment &);

	int m_segment_type;
};

class odata_metadata_segment : public odata_path_segment
{
public:
	odata_metadata_segment()
		: odata_path_segment(odata_path_segment_type::Metadata) {}
	~odata_metadata_segment() {}
};

class odata_batch_segment : public odata_path_segment
{
public:
	odata_batch_segment()
		: odata_path_segment(odata_path_segment_type::Batch) {}
	~odata_batch_segment() {}
};

class odata_entity_set_segment : public odata_path_segment
{
public:
	odata_entity_set_segment(std::shared_ptr<::odata::edm::edm_entity_set> entity_set)
		: odata_path_segment(odata_path_segment_type::EntitySet),
		m_entity_set(entity_set),
		m_entity_type(entity_set->get_entity_type()) {}
	~odata_entity_set_segment() {}

	std::shared_ptr<::odata::edm::edm_entity_set> entity_set() const { return m_entity_set.lock(); }
	std::shared_ptr<::odata::edm::edm_entity_type> entity_type() const { return m_entity_type.lock(); }

private:
	std::weak_ptr<::odata::edm::edm_entity_set> m_entity_set;
	std::weak_ptr<::odata::edm::edm_entity_type> m_entity_type;
};

class odata_singleton_segment : public odata_path_segment
{
public:
	odata_singleton_segment(std::shared_ptr<::odata::edm::edm_singleton> singleton)
		: odata_path_segment(odata_path_segment_type::Singleton),
		m_singleton(singleton),
		m_entity_type(singleton->get_entity_type()) {}
	~odata_singleton_segment() {}

	std::shared_ptr<::odata::edm::edm_singleton> singleton() const { return m_singleton.lock(); }
	std::shared_ptr<::odata::edm::edm_entity_type> entity_type() const { return m_entity_type.lock(); }

private:
	std::weak_ptr<::odata::edm::edm_singleton> m_singleton;
	std::weak_ptr<::odata::edm::edm_entity_type> m_entity_type;
};

class odata_key_segment : public odata_path_segment
{
public:
	odata_key_segment(
        std::shared_ptr<::odata::edm::edm_navigation_source> navigation_source,
		std::shared_ptr<::odata::edm::edm_entity_type> target_entity_type,
        std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> &&keys)
		: odata_path_segment(odata_path_segment_type::Key),
        m_navigation_source(navigation_source),
		m_target_entity_type(target_entity_type),
		m_keys(keys) {}
	~odata_key_segment() {}
    
    std::shared_ptr<::odata::edm::edm_navigation_source> navigation_source() const { return m_navigation_source.lock(); }
	std::shared_ptr<::odata::edm::edm_entity_type> target_entity_type() const { return m_target_entity_type.lock(); }

	const std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>> &key_at(::size_t i) const { return m_keys[i]; }
	const std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> &keys() const { return m_keys; }

private:
    std::weak_ptr<::odata::edm::edm_navigation_source> m_navigation_source;
	std::weak_ptr<::odata::edm::edm_entity_type> m_target_entity_type;
	std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> m_keys;
};

class odata_structural_property_segment : public odata_path_segment
{
public:
    odata_structural_property_segment(
        std::shared_ptr<::odata::edm::edm_structured_type> owning_type,
        std::shared_ptr<::odata::edm::edm_property_type> property)
		: odata_path_segment(odata_path_segment_type::StructuralProperty),
        m_owning_type(owning_type),
        m_property(property) {}
    ~odata_structural_property_segment() {}
    
    std::shared_ptr<::odata::edm::edm_structured_type> owning_type() const { return m_owning_type.lock(); }
    std::shared_ptr<::odata::edm::edm_property_type> property() const { return m_property.lock(); }
    
private:
    std::weak_ptr<::odata::edm::edm_structured_type> m_owning_type;
    std::weak_ptr<::odata::edm::edm_property_type> m_property;
};

class odata_navigation_property_segment : public odata_path_segment
{
public:
    odata_navigation_property_segment(
        std::shared_ptr<::odata::edm::edm_structured_type> owning_type,
        std::shared_ptr<::odata::edm::edm_property_type> property,
        std::shared_ptr<::odata::edm::edm_navigation_type> navigation_type)
		: odata_path_segment(odata_path_segment_type::NavigationProperty),
		m_owning_type(owning_type),
        m_property(property),
        m_navigation_type(navigation_type) {}
    ~odata_navigation_property_segment() {}
    
    std::shared_ptr<::odata::edm::edm_structured_type> owning_type() const { return m_owning_type.lock(); }
    std::shared_ptr<::odata::edm::edm_property_type> property() const { return m_property.lock(); }
    std::shared_ptr<::odata::edm::edm_navigation_type> navigation_type() const { return m_navigation_type.lock(); }
    
private:
    std::weak_ptr<::odata::edm::edm_structured_type> m_owning_type;
    std::weak_ptr<::odata::edm::edm_property_type> m_property;
    std::weak_ptr<::odata::edm::edm_navigation_type> m_navigation_type;
};

class odata_dynamic_property_segment : public odata_path_segment
{
public:
    odata_dynamic_property_segment(
        const ::odata::utility::string_t &property_name)
		: odata_path_segment(odata_path_segment_type::DynamicProperty),
		m_property_name(property_name) {}
    ~odata_dynamic_property_segment() {}
    
	const ::odata::utility::string_t &property_name() const { return m_property_name; }
    
private:
	::odata::utility::string_t m_property_name;
};

class odata_value_segment : public odata_path_segment
{
public:
    odata_value_segment()
		: odata_path_segment(odata_path_segment_type::Value) {}
    ~odata_value_segment() {}
};

class odata_count_segment : public odata_path_segment
{
public:
    odata_count_segment()
		: odata_path_segment(odata_path_segment_type::Count) {}
    ~odata_count_segment() {}
};

class odata_ref_segment : public odata_path_segment
{
public:
    odata_ref_segment()
        : odata_path_segment(odata_path_segment_type::Ref) {}
    ~odata_ref_segment() {}
};

class odata_type_segment : public odata_path_segment
{
public:
    odata_type_segment(std::shared_ptr<::odata::edm::edm_named_type> type)
		: odata_path_segment(odata_path_segment_type::Type),
		m_type(type) {}
    ~odata_type_segment() {}
    
    std::shared_ptr<::odata::edm::edm_named_type> type() const { return m_type.lock(); }
    
private:
    std::weak_ptr<::odata::edm::edm_named_type> m_type;
};

class odata_operation_segment : public odata_path_segment
{
public:
	odata_operation_segment(
		std::shared_ptr<::odata::edm::edm_operation_type> operation,
		std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> &&parameters)
		: odata_path_segment(odata_path_segment_type::Operation),
		m_operation(operation),
		m_parameters(parameters) {}
	~odata_operation_segment() {}

	std::shared_ptr<::odata::edm::edm_operation_type> operation() const { return m_operation.lock(); }
	const std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>> &parameter_at(::size_t i) const { return m_parameters[i]; }
	const std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> &parameters() const { return m_parameters; }

private:
	std::weak_ptr<::odata::edm::edm_operation_type> m_operation;
	std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> m_parameters;
};

class odata_operation_import_segment : public odata_path_segment
{
public:
	odata_operation_import_segment(
		std::shared_ptr<::odata::edm::edm_operation_import> operation_import,
		std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> &&parameters)
		: odata_path_segment(odata_path_segment_type::OperationImport),
		m_operation_import(operation_import),
		m_parameters(parameters) {}
	~odata_operation_import_segment() {}

	std::shared_ptr<::odata::edm::edm_operation_import> operation_import() const { return m_operation_import.lock(); }
	const std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>> &parameter_at(::size_t i) const { return m_parameters[i]; }
	const std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> &parameters() const { return m_parameters; }

private:
	std::weak_ptr<::odata::edm::edm_operation_import> m_operation_import;
	std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> m_parameters;
};

}}