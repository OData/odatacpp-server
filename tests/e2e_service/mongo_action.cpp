//---------------------------------------------------------------------
// <copyright file="mongo_action.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "mongo_action.h"

using namespace ::std;
using namespace mongo;

namespace odata { namespace service
{
	mongo_action::mongo_action(data_source_operation_type operation_type, const string& db_name, const string& collection_name)
		:m_operation_type(operation_type), m_db_name(db_name), m_collection_name(collection_name), m_finished(false)
	{
	
	}

	mongo_action::mongo_action(data_source_operation_type operation_type, const string& db_name, const string& collection_name, ::std::shared_ptr<mongo_action> dependency, ::std::string dependency_field)
		:m_operation_type(operation_type),  m_db_name(db_name), m_collection_name(collection_name), m_finished(false), m_dependency(dependency), m_dependency_field(dependency_field)
	{
	
	}

	BSONObjBuilder &mongo_action::query_builder()
	{
		return m_query_builder;
	}

	BSONObjBuilder &mongo_action::orderby_builder()
	{
		return m_orderby_builder;
	}

	BSONObjBuilder &mongo_action::projection_builder()
	{
		return m_projection_builder;
	}

	void mongo_action::set_input_documents(BSONObj document)
	{
		m_input_documents.clear();
		m_input_documents.push_back(document);
	}

	void mongo_action::set_input_documents(const vector<BSONObj> documents)
	{
		m_input_documents.clear();
		for (auto iter = documents.cbegin(); iter != documents.cend(); iter++)
		{
			m_input_documents.push_back(*iter);
		}
	}

	const vector<BSONObj>& mongo_action::execute()
	{
		if (m_finished)
		{
			return m_result;
		}

		if (m_dependency)
		{
			const vector<BSONObj>& dependency_result = m_dependency->execute();
			
			//If m_dependency_field is empty, that means the current action depends on the result of m_dependency, it just need the m_dependency action to finish first.
			if (!m_dependency_field.empty())
			{
				if (dependency_result.size() != 1)
				{
					//throw exception
				}
				auto field_value = dependency_result[0].getField(m_dependency_field);
				m_query_builder << "_id" << BSON("$in" << field_value);
			}
					
		}

		string full_collection_name = get_full_collection_name();

		DBClientConnection c;
		c.connect("localhost");

		//TODO: handle exceptions here
		cout << m_query_builder.done() << endl;
		cout << m_projection_builder.done() << endl;
		if (m_operation_type == ds_drop)
		{
			if (m_collection_name.empty())
			{
				//drop the whole database
				c.dropDatabase(m_db_name);
			}
			else
			{
				//drop the collection
				c.dropCollection(full_collection_name);
			}
		}
		else if (m_operation_type == ds_insert)
		{
			c.insert(full_collection_name, m_input_documents);
		}
		else if (m_operation_type == ds_query)
		{
			m_result_cursor = c.query(full_collection_name, Query(m_query_builder.done()).sort(m_orderby_builder.done()), 0, 0, &m_projection_builder.done());

			while (m_result_cursor->more())
			{
				m_result.push_back(m_result_cursor->next());
			}
		}
		
		m_finished = true;
		return m_result;
	}

	string mongo_action::get_full_collection_name() const
	{
		return m_db_name + "." + m_collection_name;
	}

}}