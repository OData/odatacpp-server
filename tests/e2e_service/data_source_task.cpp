//---------------------------------------------------------------------
// <copyright file="data_source_task.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include <vector>

#include "data_source_task.h"
#include "query_node_visitor.h"
#include "odata_value_builder.h"
#include "odata_service_exception.h"

using namespace ::std;
using namespace ::utility;
using namespace ::odata::edm;
using namespace ::odata::core;

namespace odata { namespace service
{
	mongo_task::mongo_task(data_source_operation_type operation_type, shared_ptr<odata_request_context> request_context) : m_operation_type(operation_type), m_request_context(request_context)
	{
	
	}

	const std::vector<::mongo::BSONObj> &mongo_task::run()
	{
		resolve_request();

		if (m_actions.size() == 0)
		{
			throw new odata_service_exception(U("Invalid request. No actions can be done."));
		}
		else
		{
			return m_actions[m_actions.size() - 1]->execute();
		}
	}

	mongo_query_task::mongo_query_task(shared_ptr<odata_request_context> request_context) : mongo_task(ds_query, request_context)
	{
	
	}

	void mongo_query_task::resolve_request()
	{
		auto path_segments = m_request_context->get_odata_path()->segments();
		string db_name("trippin");

		shared_ptr<odata_path_segment> last_segment;
		for (auto iter = path_segments.cbegin(); iter != path_segments.cend(); iter++)
		{
			shared_ptr<odata_path_segment> seg = *iter;
			switch (seg->segment_type())
			{
				case odata_path_segment_type::EntitySet:
					{
						if (last_segment != nullptr)
						{
							//throw exception
						}

						shared_ptr<odata_entity_set_segment> entityset_seg = seg->as<odata_entity_set_segment>();
						string_t entityset_name = entityset_seg->entity_set()->get_name();
						string entityset_name_s(entityset_name.cbegin(), entityset_name.cend());
				
						shared_ptr<mongo_action> start_action = make_shared<mongo_action>(ds_query, db_name, entityset_name_s);
						start_action->projection_builder() << "_id" << 0;
						m_actions.push_back(start_action);
						break;
					}
				case odata_path_segment_type::Key:
					{
						int last_segment_type = last_segment->segment_type();
						if (last_segment_type != odata_path_segment_type::EntitySet)
						{
							//throw exception
						}

						shared_ptr<odata_key_segment> key_seg = seg->as<odata_key_segment>();
						auto keys = key_seg->keys();
					
						for (auto key = keys.cbegin(); key != keys.cend(); key++)
						{
							string key_name_s, key_value_s;
							key_name_s.assign(key->first.cbegin(), key->first.cend());
							string_t second = key->second->as<::odata::utility::string_t>();
							key_value_s.assign(second.cbegin(), second.cend());
							m_actions[m_actions.size()-1]->query_builder() <<  key_name_s << key_value_s;
						}
						break;
					}
				case odata_path_segment_type::StructuralProperty:
					{
						int last_segment_type = last_segment->segment_type();
						if (last_segment_type != odata_path_segment_type::Key)
						{
							//throw exception
						}

						shared_ptr<odata_structural_property_segment> property_seg = seg->as<odata_structural_property_segment>();
						string_t property_name = property_seg->property()->get_name();
					
						string property_name_s(property_name.cbegin(), property_name.cend());
						m_actions[m_actions.size()-1]->projection_builder() << property_name_s << 1;

						break;
					}

				case odata_path_segment_type::NavigationProperty:
					{
						int last_segment_type = last_segment->segment_type();
						if (last_segment_type != odata_path_segment_type::Key)
						{
							//throw exception
						}

						shared_ptr<odata_navigation_property_segment> property_seg = seg->as<odata_navigation_property_segment>();
						string_t property_name = property_seg->property()->get_name();
						shared_ptr<edm_navigation_source> target_source = property_seg->navigation_type()->get_binded_navigation_source();
						string_t target_source_name = target_source->get_name();

						string property_name_s(property_name.cbegin(), property_name.cend());
						string target_source_name_s(target_source_name.cbegin(), target_source_name.cend());

						m_actions[m_actions.size()-1]->projection_builder() << property_name_s << 1;
						shared_ptr<mongo_action> another_action = make_shared<mongo_action>(ds_query, db_name, target_source_name_s, m_actions[m_actions.size()-1], property_name_s);
						another_action->projection_builder() << "_id" << 0;
						m_actions.push_back(another_action);
						break;
					}
				default:
					{
						break;
					}
			}

			last_segment = seg;
			
		}

		auto filter_clause = m_request_context->get_filter_clause();
		if (filter_clause)
		{
			auto query_node_visitor = make_shared<node_to_bson_visitor>();
			::mongo::BSONObj filter_query_obj = query_node_visitor->translate_node(filter_clause->expression());

			::mongo::BSONObjIterator fieldIterator(filter_query_obj["query"].Obj());
			while (fieldIterator.more())
			{
				m_actions[m_actions.size()-1]->query_builder() << fieldIterator.next();
			}
		}
		
		auto orderby_clause = m_request_context->get_orderby_clause();
		if (orderby_clause)
		{
			auto query_node_visitor = make_shared<node_to_bson_visitor>();
			for (auto iter = orderby_clause->items().cbegin(); iter!= orderby_clause->items().cend(); iter++)
			{
				::mongo::BSONObj orderby_obj = query_node_visitor->translate_node(iter->first);
				m_actions[m_actions.size()-1]->orderby_builder() << orderby_obj["query"].String() << (iter->second ? 1 : -1);
				
			}
			
		}
	}

	

	mongo_insert_task::mongo_insert_task(shared_ptr<odata_request_context> request_context, std::shared_ptr<::odata::core::odata_value> insert_value) 
		: mongo_task(ds_insert, request_context), m_insert_value(insert_value)
	{
	
	}

	void mongo_insert_task::resolve_request()
	{
		auto path_segments = m_request_context->get_odata_path()->segments();
		string db_name("trippin");

		if (path_segments.size() != 1)
		{
			//throw exception to simplify the scenario for now. should be removed later. 
		}

		auto value_builder = std::make_shared<odata_value_builder>();
		auto insert_document = value_builder->build_BSONObj(m_insert_value);

		shared_ptr<odata_path_segment> last_segment;
		for (auto iter = path_segments.cbegin(); iter != path_segments.cend(); iter++)
		{
			shared_ptr<odata_path_segment> seg = *iter;
			switch (seg->segment_type())
			{
				case odata_path_segment_type::EntitySet:
					{
						if (last_segment != nullptr)
						{
							//throw exception
						}

						shared_ptr<odata_entity_set_segment> entityset_seg = seg->as<odata_entity_set_segment>();
						string_t entityset_name = entityset_seg->entity_set()->get_name();
						string entityset_name_s(entityset_name.cbegin(), entityset_name.cend());
				
						shared_ptr<mongo_action> start_action = make_shared<mongo_action>(ds_insert, db_name, entityset_name_s);
						start_action->set_input_documents(insert_document);
						m_actions.push_back(start_action);
						break;
					}
				default:
					{
						break;
					}
			}

			last_segment = seg;
			
		}

		if (last_segment->segment_type() == odata_path_segment_type::EntitySet)
		{
			shared_ptr<odata_entity_set_segment> entityset_seg = last_segment->as<odata_entity_set_segment>();
			string_t entityset_name = entityset_seg->entity_set()->get_name();
			string entityset_name_s(entityset_name.cbegin(), entityset_name.cend());

			shared_ptr<mongo_action> query_after_insert = make_shared<mongo_action>(ds_query, db_name, entityset_name_s, m_actions[m_actions.size()-1], "");
			
			auto entity_key = entityset_seg->entity_type()->key();
			for (auto key = entity_key.cbegin(); key != entity_key.cend(); key++)
			{
				string key_name_s;
				key_name_s.assign(key->cbegin(), key->cend());

				auto target_key_value_pair = insert_document.getField(key_name_s);
				query_after_insert->query_builder() << target_key_value_pair;
			}

			m_actions.push_back(query_after_insert);
		}
		else {
			//throw exceptions here.
		}
	}

	mongo_initialize_task::mongo_initialize_task() : mongo_task(ds_insert, nullptr)
	{
	
	}

	void mongo_initialize_task::resolve_request()
	{
		string db_name("trippin"), people("People"), airports("Airports");

		vector<mongo::BSONObj> airports_data;
		mongo::OID aid_1 = mongo::OID::gen();
		airports_data.push_back(BSON("_id" << aid_1 << "IcaoCode" << "KSFO" << "Name" << "San Francisco International Airport" << "IataCode" << "SFO"
			<< "Location" << BSON("Address" << "South McDonnell Road, San Francisco, CA 94128"
			<< "City" << BSON("CountryRegion" << "United States" << "Name" << "San Francisco" << "Region" << "California"))
			));
		mongo::OID aid_2 = mongo::OID::gen();
		airports_data.push_back(BSON("_id" << aid_2 << "IcaoCode" << "KLAX" << "Name" << "Los Angeles International Airport" << "IataCode" << "LAX"
			<< "Location" << BSON("Address" << "1 World Way, Los Angeles, CA, 90045"
			<< "City" << BSON("CountryRegion" << "United States" << "Name" << "Los Angeles" << "Region" << "California"))
			));
		mongo::OID aid_3 = mongo::OID::gen();
		airports_data.push_back(BSON("_id" << aid_3 << "IcaoCode" << "ZSSS" << "Name" << "Shanghai Hongqiao International Airport" << "IataCode" << "SHA"
			<< "Location" << BSON("Address" << "Hongqiao Road 2550, Changning District"
			<< "City" << BSON("CountryRegion" << "China" << "Name" << "Shanghai" << "Region" << "Shanghai"))
			));
		mongo::OID aid_4 = mongo::OID::gen();
		airports_data.push_back(BSON("_id" << aid_4 << "IcaoCode" << "ZBAA" << "Name" << "Beijing Capital International Airport" << "IataCode" << "PEK"
			<< "Location" << BSON("Address" << "Airport Road, Chaoyang District, Beijing, 100621"
			<< "City" << BSON("CountryRegion" << "China" << "Name" << "Beijing" << "Region" << "Beijing"))
			));


		vector<mongo::BSONObj> people_data;
		mongo::OID pid_1 = mongo::OID::gen();
		people_data.push_back(BSON("_id" << pid_1 << "UserName" << "russellwhyte" << "FirstName" << "Russell" << "LastName" << "Whyte" << "Gender" << "Male"
			<< "Emails" << BSON_ARRAY("Russell@example.com" << "Russell@contoso.com")
			<< "AddressInfo" << BSON_ARRAY(BSON("Address" << "187 Suffolk Ln." << "City" << BSON("CountryRegion" << "United States" << "Name" << "Boise" << "Region" << "ID")))
			//<< "RecentAirports" << BSON_ARRAY("KSFO" << "KLAX")
			<< "RecentAirports" << BSON_ARRAY(aid_1 << aid_2)
			));
		mongo::OID pid_2 = mongo::OID::gen();
		people_data.push_back(BSON("_id" << pid_2 << "UserName" << "scottketchum" << "FirstName" << "Scott" << "LastName" << "Ketchum" << "Gender" << "Male"
			<< "Emails" << BSON_ARRAY("Scott@example.com")
			<< "AddressInfo" << BSON_ARRAY(BSON("Address" << "2817 Milton Dr." << "City" << BSON("CountryRegion" << "United States" << "Name" << "Albuquerque" << "Region" << "NM")))
			//<< "RecentAirports" << BSON_ARRAY("KLAX")
			<< "RecentAirports" << BSON_ARRAY(aid_2)
			));
		mongo::OID pid_3 = mongo::OID::gen();
		people_data.push_back(BSON("_id" << pid_3 << "UserName" << "ronaldmundy" << "FirstName" << "Ronald" << "LastName" << "Mundy" << "Gender" << "Male"
			<< "Emails" << BSON_ARRAY("Ronald@example.com" << "Ronald@contoso.com")
			<< "AddressInfo" << mongo::BSONArrayBuilder().arr()
			//<< "RecentAirports" << BSON_ARRAY("ZSSS" << "ZBAA")
			<< "RecentAirports" << BSON_ARRAY(aid_1 << aid_3 << aid_4)
			));
		mongo::OID pid_4 = mongo::OID::gen();
		people_data.push_back(BSON("_id" << pid_4 << "UserName" << "javieralfred" << "FirstName" << "Javier" << "LastName" << "Alfred" << "Gender" << "Male"
			<< "Emails" << BSON_ARRAY("Javier@example.com" << "Javier@contoso.com")
			<< "AddressInfo" << BSON_ARRAY(BSON("Address" << "89 Jefferson Way Suite 2" << "City" << BSON("CountryRegion" << "United States" << "Name" << "Portland" << "Region" << "WA")))
			<< "RecentAirports" << mongo::BSONArrayBuilder().arr()
			));
		mongo::OID pid_5 = mongo::OID::gen();
		people_data.push_back(BSON("_id" << pid_5 << "UserName" << "willieashmore" << "FirstName" << "Willie" << "LastName" << "Ashmore" << "Gender" << "Male"
			<< "Emails" << BSON_ARRAY("Willie@example.com" << "Willie@contoso.com")
			<< "AddressInfo" << mongo::BSONArrayBuilder().arr()
			<< "RecentAirports" << mongo::BSONArrayBuilder().arr()
			));

		shared_ptr<mongo_action> db_drop_action = make_shared<mongo_action>(ds_drop, db_name, "");

		shared_ptr<mongo_action> people_initialize_action = make_shared<mongo_action>(ds_insert, db_name, people, db_drop_action, "");
		people_initialize_action->set_input_documents(people_data);

		shared_ptr<mongo_action> airports_initialize_action = make_shared<mongo_action>(ds_insert, db_name, airports, people_initialize_action, "");
		airports_initialize_action->set_input_documents(airports_data);

		m_actions.push_back(db_drop_action);
		m_actions.push_back(people_initialize_action);
		m_actions.push_back(airports_initialize_action);
	}

}}