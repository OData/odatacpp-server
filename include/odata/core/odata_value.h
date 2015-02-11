//---------------------------------------------------------------------
// <copyright file="odata_value.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/edm/odata_edm.h"
#include "odata/core/odata_property_map.h"
#include "odata/core/odata_operation.h"

namespace odata { namespace core
{

class odata_entity_value;
// A combination of the property type (see enum above) and a string representation of the property value.
class odata_value
{
public:
    /// <summary>Default constructor</summary>
    odata_value() : m_property_type(std::make_shared<::odata::edm::edm_named_type>()), m_is_null_value(), m_is_top_level(){}

    odata_value(std::shared_ptr<::odata::edm::edm_named_type>type, bool is_null_value = false) 
		: m_property_type(type), m_is_null_value(is_null_value), m_is_top_level()
	{
	}

    virtual ~odata_value(){};

    std::shared_ptr<::odata::edm::edm_named_type> get_value_type() const { return m_property_type; }

	void set_value_type(std::shared_ptr<::odata::edm::edm_named_type> property_type)
	{
		m_property_type = property_type;
	}

    void set_is_top_level(bool is_top_level)
    {
        m_is_top_level = is_top_level;
    }

    void set_instance_annotation(::odata::utility::string_t name, std::shared_ptr<odata_value> value)
    {
        m_instance_annotations[name] = value;
    }

    bool try_get_instance_annotation_value(::odata::utility::string_t name, std::shared_ptr<odata_value>& value)
    {
        const auto& annotation = m_instance_annotations.find(name);
        bool found = (annotation != m_instance_annotations.end());

        if (found)
        {
            value = annotation->second;
        }

        return found;
    }

    std::unordered_map<::odata::utility::string_t, std::shared_ptr<odata_value>>& get_instance_annotations()
    {
        return m_instance_annotations;
    }

    void set_context_url(::odata::utility::uri value)
    {
        m_context_url = value;
    }

    ::odata::utility::uri get_context_url()
    {
        return m_context_url;
    }

    bool is_top_level()
    {
        return m_is_top_level;
    }

    std::vector<std::shared_ptr<odata_action>>& get_actions()
    {
        return m_actions;
    }

    std::vector<std::shared_ptr<odata_function>>& get_functions()
    {
        return m_functions;
    }

    void add_action(std::shared_ptr<odata_action> action)
    {
        m_actions.push_back(action);
    }

    void add_function(std::shared_ptr<odata_function> function)
    {
        m_functions.push_back(function);
    }

    void set_type_name(::odata::utility::string_t type_name)
    {
        m_type_name = type_name;
    }

    ::odata::utility::string_t get_type_name()
    {
        return m_type_name;
    }

private:
    friend class entity;
    friend class odata_property_map;

    std::shared_ptr<::odata::edm::edm_named_type> m_property_type;
	bool m_is_null_value;
    bool m_is_top_level;

    std::unordered_map<::odata::utility::string_t, std::shared_ptr<odata_value>> m_instance_annotations;

    ::odata::utility::uri m_context_url;
    ::odata::utility::string_t m_type_name;

    std::vector<std::shared_ptr<odata_action>> m_actions;
    std::vector<std::shared_ptr<odata_function>> m_functions;
};

}}