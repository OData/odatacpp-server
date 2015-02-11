//---------------------------------------------------------------------
// <copyright file="mongo_action.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include <string>
#include <cstdlib>
#include <WinSock2.h>

#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>

namespace odata { namespace service
{

enum data_source_operation_type
{
	ds_insert,
	ds_update,
	ds_query,
	ds_delete,
	ds_drop
};

class mongo_action
{
public:
	mongo_action(data_source_operation_type operation_type, const ::std::string& db_name, const ::std::string& collection_name);

	mongo_action(data_source_operation_type operation_type, const ::std::string& db_name, const ::std::string& collection_name, ::std::shared_ptr<mongo_action> dependency, ::std::string dependency_field);

	const ::std::vector<mongo::BSONObj>& execute();

	::std::string get_full_collection_name() const;

	mongo::BSONObjBuilder &query_builder();

	mongo::BSONObjBuilder &orderby_builder();

	mongo::BSONObjBuilder &projection_builder();

	void set_input_documents(mongo::BSONObj document);

	void set_input_documents(const ::std::vector<mongo::BSONObj> documents);

private:
	bool m_finished;

	data_source_operation_type m_operation_type;

	::std::string m_db_name;
	::std::string m_collection_name;

	::std::string m_dependency_field;
	::std::shared_ptr<mongo_action> m_dependency;

	mongo::BSONObjBuilder m_query_builder;
	mongo::BSONObjBuilder m_orderby_builder;
	mongo::BSONObjBuilder m_projection_builder;

	::std::auto_ptr<mongo::DBClientCursor> m_result_cursor;
	::std::vector<mongo::BSONObj> m_result;

	::std::vector<mongo::BSONObj> m_input_documents;
};

}}