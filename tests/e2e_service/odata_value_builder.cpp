//---------------------------------------------------------------------
// <copyright file="odata_value_builder.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata_value_builder.h"
#include "odata_service_exception.h"

using namespace ::odata::edm;
using namespace ::odata::core;

namespace odata { namespace service
{
	std::shared_ptr<odata_value> odata_value_builder::build_odata_value(std::shared_ptr<edm_named_type> edm_type, const std::vector<::mongo::BSONObj> &data)
	{
		if (edm_type->get_type_kind() == edm_type_kind_t::Collection)
		{
			auto collection_type = std::dynamic_pointer_cast<edm_collection_type>(edm_type);			
			auto element_type = collection_type->get_element_type();

			if (element_type->get_type_kind() == edm_type_kind_t::Entity)
			{
				auto collection_value = std::make_shared<odata_collection_value>(collection_type);
				collection_value->set_is_top_level(true);

				for (auto iter = data.cbegin(); iter != data.cend(); iter++)
				{
					collection_value->add_collection_value(build_odata_value(element_type, *iter));
				}

				return collection_value;
			}
			
		}
		
		if (data.size() > 1)
		{
			//throw exception: internal server error
		}

		std::shared_ptr<odata_value> ret_value;
		if (data.size() == 1)
		{
			ret_value = build_odata_value(edm_type, data[0]);
		}
		else
		{
			//nothing found. should return 204, or possibly 404. 
			ret_value = std::make_shared<odata_value>(edm_type);
		}
		ret_value->set_is_top_level(true);

		return ret_value;
	}


	std::shared_ptr<odata_value> odata_value_builder::build_odata_value(std::shared_ptr<edm_named_type> edm_type, ::mongo::BSONObj data)
	{
		std::shared_ptr<odata_value> ret_value;

		switch (edm_type->get_type_kind())
		{
		case edm_type_kind_t::Primitive:
			{
				auto primitive_type = std::dynamic_pointer_cast<edm_primitive_type>(edm_type);
				return build_odata_primitive_value(primitive_type, data.firstElement());
			}
		case edm_type_kind_t::Enum:
			{
				auto enum_type = std::dynamic_pointer_cast<edm_enum_type>(edm_type);
				return build_odata_enum_value(enum_type, data.firstElement());
			}
		case edm_type_kind_t::Collection:
			{
				auto collection_type = std::dynamic_pointer_cast<edm_collection_type>(edm_type);
				return build_odata_collection_value(collection_type, data.firstElement());
			}
		case edm_type_kind_t::Entity:
			{
				auto entity_type = std::dynamic_pointer_cast<edm_entity_type>(edm_type);
				return build_odata_structured_value(entity_type, data);
			}

		case edm_type_kind_t::Complex:
			{
				auto complex_type = std::dynamic_pointer_cast<edm_complex_type>(edm_type);
				return build_odata_structured_value(complex_type, data.firstElement().Obj());
			}
		default:
			{
				throw new odata_service_exception(U("Service does not support yet."));
			}
			break;
		}

	}

	std::shared_ptr<odata_structured_value> odata_value_builder::build_odata_structured_value(std::shared_ptr<edm_structured_type> edm_type, ::mongo::BSONObj data)
	{
		std::shared_ptr<odata_structured_value> ret_value;

		if (edm_type->get_type_kind() == edm_type_kind_t::Entity)
		{
			auto entity_type = std::dynamic_pointer_cast<edm_entity_type>(edm_type);
			ret_value = std::make_shared<odata_entity_value>(entity_type);
		}
		else
		{
			auto complex_type = std::dynamic_pointer_cast<edm_complex_type>(edm_type);
			ret_value = std::make_shared<odata_complex_value>(complex_type);
		}

		for (auto iter = edm_type->begin(); iter != edm_type->end(); iter++)
		{
			::odata::utility::string_t property_name = (*iter).first;
			auto property_type = (*iter).second;

			auto property_value_type = property_type->get_property_type();
			std::string property_name_s(property_name.cbegin(), property_name.cend());
			switch (property_value_type->get_type_kind())
			{
			case edm_type_kind_t::Collection:
				{
					auto collection_type = std::dynamic_pointer_cast<edm_collection_type>(property_value_type);
					auto collection_value = build_odata_collection_value(collection_type, data.getField(property_name_s));

					ret_value->set_value(property_name, collection_value);
				}
				break;
			case edm_type_kind_t::Primitive:
				{
					auto primitive_type = std::dynamic_pointer_cast<edm_primitive_type>(property_value_type);
					auto primitive_value = build_odata_primitive_value(primitive_type, data.getField(property_name_s));

					ret_value->set_value(property_name, primitive_value);
				}
				break;

			case edm_type_kind_t::Enum:
				{
					auto enum_type = std::dynamic_pointer_cast<edm_enum_type>(property_value_type);
					auto enum_value = build_odata_enum_value(enum_type, data.getField(property_name_s));

					ret_value->set_value(property_name, enum_value);
				}
				break;
			case edm_type_kind_t::Complex:
				{
					auto complex_type = std::dynamic_pointer_cast<edm_complex_type>(property_value_type);
							
					mongo::BSONElement field = data.getField(property_name_s);
					if (field.eoo())
					{
						//The field is not found. Either the data is broken or the field is null
					}
					else
					{
						auto structured_value = build_odata_structured_value(complex_type, field.Obj()); //catch exceptions
						ret_value->set_value(property_name, std::dynamic_pointer_cast<odata_complex_value>(structured_value)); 
					}
				}
				break;
			default:
				{
						
				}
				break;
			}
					
		}

		return ret_value;
	}

	std::shared_ptr<odata_collection_value> odata_value_builder::build_odata_collection_value(std::shared_ptr<edm_collection_type> edm_type, ::mongo::BSONElement data)
	{
		auto collection_type = std::dynamic_pointer_cast<edm_collection_type>(edm_type);
		auto collection_value = std::make_shared<odata_collection_value>(collection_type);

		if (data.eoo())
		{
			//The field is not found. Either the data is broken or the field is null
			return nullptr;
		}
		else
		{
			auto element_type = collection_type->get_element_type();
			auto elements = data.Array(); //catch exceptions
			for (auto iter = elements.cbegin(); iter != elements.cend(); iter++)
			{
				switch (element_type->get_type_kind())
				{
				case edm_type_kind_t::Primitive:
					{
						auto primitive_type = std::dynamic_pointer_cast<edm_primitive_type>(element_type);
						collection_value->add_collection_value(build_odata_primitive_value(primitive_type, *iter));
					}
					break;
				case edm_type_kind_t::Enum:
					{
						auto enum_type = std::dynamic_pointer_cast<edm_enum_type>(element_type);
						collection_value->add_collection_value(build_odata_enum_value(enum_type, *iter));
					}
					break;
				case edm_type_kind_t::Collection:
					{
						auto collection_type = std::dynamic_pointer_cast<edm_collection_type>(element_type);
						collection_value->add_collection_value(build_odata_collection_value(collection_type, *iter));
					}
					break;
				case edm_type_kind_t::Complex:
				case edm_type_kind_t::Entity:
					{
						auto structured_type = std::dynamic_pointer_cast<edm_structured_type>(element_type);
						collection_value->add_collection_value(build_odata_structured_value(structured_type, iter->Obj()));
					}
					break;
				default:
					{
						throw new odata_service_exception(U("Invalid edm type.")); 
					}
					break;
				}

			}

			return collection_value;
		}
		
	}


	std::shared_ptr<odata_primitive_value> odata_value_builder::build_odata_primitive_value(std::shared_ptr<edm_primitive_type> edm_type, ::mongo::BSONElement data)
	{

		if (data.eoo())
		{
			//The field is not found. Either the data is broken or the field is null
			return nullptr;
		}
		else
		{
			switch (edm_type->get_primitive_kind())
			{
			case edm_primitive_type_kind_t::Guid:
			case edm_primitive_type_kind_t::String:
				{
					std::string string_value = data.String(); //catch exceptions
					return std::make_shared<odata_primitive_value>(edm_primitive_type::STRING(), ::utility::conversions::to_string_t(string_value));
				}
				break;
			case edm_primitive_type_kind_t::Boolean:
				{
					bool bool_value = data.Bool(); //catch exceptions
					return std::make_shared<odata_primitive_value>(edm_primitive_type::BOOLEAN(), ::utility::conversions::print_string(bool_value));
				}
				break;
			case edm_primitive_type_kind_t::Int16:
				{
					int int16_value = data.Int(); //stored as int32; catch exceptions
					return std::make_shared<odata_primitive_value>(edm_primitive_type::INT16(), ::utility::conversions::print_string(int16_value));
				}
				break;
			case edm_primitive_type_kind_t::Int32:
				{
					int int32_value = data.Int(); //catch exceptions
					return std::make_shared<odata_primitive_value>(edm_primitive_type::INT32(), ::utility::conversions::print_string(int32_value));
				}
				break;
			case edm_primitive_type_kind_t::Int64:
				{
					long long int64_value = data.Long(); //catch exceptions
					return std::make_shared<odata_primitive_value>(edm_primitive_type::INT64(), ::utility::conversions::print_string(int64_value));
				}
				break;
			default:
				{
					throw new odata_service_exception(U("service does not support the primitive type."));
				}
				break;
			}
		}
	}

	std::shared_ptr<odata_enum_value> odata_value_builder::build_odata_enum_value(std::shared_ptr<edm_enum_type> edm_type, ::mongo::BSONElement data)
	{	
		if (data.eoo())
		{
			//The field is not found. Either the data is broken or the field is null
			return nullptr;
		}
		else
		{
			std::string enum_value = data.String(); //catch exceptions
			return std::make_shared<odata_enum_value>(edm_type, ::utility::conversions::to_string_t(enum_value));
		}
	}

	::mongo::BSONObj odata_value_builder::build_BSONObj(std::shared_ptr<odata_value> odata_value)
	{
		if (odata_value)
		{
			return build_BSONObj(odata_value->get_value_type(), odata_value);
		}

		return ::mongo::BSONObj();
	}

	::mongo::BSONObj odata_value_builder::build_BSONObj(std::shared_ptr<edm_named_type> edm_type, std::shared_ptr<odata_value> odata_value)
	{
		if (edm_type && odata_value)
		{
			switch (edm_type->get_type_kind())
			{
			case edm_type_kind_t::Entity:
				{
					auto structured_value = std::dynamic_pointer_cast<odata_structured_value>(odata_value);
					return build_BSONObj_from_structured_value(structured_value);
				}
				break;
			default:
				break;
			}
		}

		return ::mongo::BSONObj();
	}

	::mongo::BSONObj odata_value_builder::build_BSONObj_from_structured_value(std::shared_ptr<odata_structured_value> odata_value)
	{
		::mongo::BSONObjBuilder builder;

		for (auto iter = odata_value->properties().cbegin(); iter != odata_value->properties().cend(); iter++)
		{
			std::string property_name(iter->first.cbegin(), iter->first.cend());

			if (!iter->second)
			{
				builder << property_name << ::mongo::BSONObj();
			}
			else
			{
				auto property_edm_type = iter->second->get_value_type();
				switch (property_edm_type->get_type_kind())
				{
				case edm_type_kind_t::Primitive:
					{
						auto primitive_type = std::dynamic_pointer_cast<edm_primitive_type>(property_edm_type);
						auto primitive_value = std::dynamic_pointer_cast<odata_primitive_value>(iter->second);
						switch (primitive_type->get_primitive_kind())
						{
						case edm_primitive_type_kind_t::String:
							{
								auto str_value = primitive_value->as<::odata::utility::string_t>();
								std::string std_str(str_value.cbegin(), str_value.cend());
								builder << property_name << std_str;
							}
							break;
						case edm_primitive_type_kind_t::Int32:
							{
								builder << property_name << primitive_value->as<int>();
							}
							break;
						default:
							break;
						}
					}
					break;
				case edm_type_kind_t::Complex:
					{
						auto structured_value = std::dynamic_pointer_cast<odata_structured_value>(iter->second);
						builder << property_name << build_BSONObj_from_structured_value(structured_value);
					}
					break;
				default:
					break;
				}
			}
		}

		return builder.obj();
	}

}}