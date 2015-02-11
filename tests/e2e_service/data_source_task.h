//---------------------------------------------------------------------
// <copyright file="data_source_task.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <WinSock2.h>

#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>

#include "mongo_action.h"
#include "odata_request_context.h"

namespace odata { namespace service
{

class mongo_task
{
public:
	mongo_task(data_source_operation_type operation_type, ::std::shared_ptr<odata_request_context> request_context);
	~mongo_task() {};
	virtual const std::vector<::mongo::BSONObj> &run();

protected:
	virtual void resolve_request() = 0;

	data_source_operation_type m_operation_type;
	::std::shared_ptr<odata_request_context> m_request_context;
	::std::vector<::std::shared_ptr<mongo_action>> m_actions;
};

class mongo_query_task : public mongo_task
{
public:
	mongo_query_task(::std::shared_ptr<odata_request_context> request_context);
	~mongo_query_task() {};

private:
	virtual void resolve_request();
};

class mongo_insert_task : public mongo_task
{
public:
	mongo_insert_task(::std::shared_ptr<odata_request_context> request_context, std::shared_ptr<::odata::core::odata_value> insert_value);
	~mongo_insert_task() {};

private:
	std::shared_ptr<::odata::core::odata_value> m_insert_value;

	virtual void resolve_request();
};

class mongo_initialize_task : public mongo_task
{
public:
	mongo_initialize_task();
	~mongo_initialize_task() {};

private:
	virtual void resolve_request();
};

}}