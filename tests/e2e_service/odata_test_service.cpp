//---------------------------------------------------------------------
// <copyright file="odata_test_service.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include <cstdlib>
#include <WinSock2.h>

#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>

#include "data_source_task.h"
#include "query_handler.h"
#include "create_handler.h"
#include "odata_test_service.h"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

using namespace ::odata::core;
using namespace ::odata::edm;
using namespace concurrency::streams;


namespace odata { namespace service
{

// TODO replace ::odata::utility::string_t with web::url
odata_test_service::odata_test_service(::odata::utility::string_t url, std::shared_ptr<::odata::edm::edm_model> model, std::shared_ptr<odata_service_document> service_document)
	: m_listener(url), m_model(model), m_service_document(service_document)
{
	m_uri_parser = std::make_shared<::odata::core::odata_uri_parser>(model);
	m_service_root = web::uri(url);
    m_listener.support(methods::GET, std::bind(&odata_test_service::handle_get, this, std::placeholders::_1));
	m_listener.support(methods::POST, std::bind(&odata_test_service::handle_create, this, std::placeholders::_1));
}

void odata_test_service::initialize_datasource()
{
	shared_ptr<mongo_task> ds_task = make_shared<mongo_initialize_task>();
	ds_task->run();
}

void odata_test_service::handle_error(pplx::task<void>& t)
{  
    try
    {
        t.get();
    }
    catch(...)
    {
        // Ignore the error, Log it if a logger is available 
    }
}

pplx::task<void> odata_test_service::open()
{
    return m_listener.open().then(std::bind(&handle_error, std::placeholders::_1));
}

pplx::task<void> odata_test_service::close()
{
    return m_listener.close().then(std::bind(&handle_error, std::placeholders::_1));
}

// Handler to process HTTP::GET requests.
// Replies to the request with data.
void odata_test_service::handle_get(http_request message)
{
	try
	{
		auto parsed_uri = m_uri_parser->parse_uri(message.relative_uri());

		odata_message_writer writer(m_model, m_service_root);
		::odata::utility::string_t content;
		if (parsed_uri->is_service_document())
		{
			// Write service document
			content = writer.write_service_document(m_service_document);
		}
		else if (parsed_uri->is_metadata_document())
		{
			// Write metadata document
			content = writer.write_metadata_document();
		}
		else
		{
			shared_ptr<query_handler> handler = make_shared<query_handler>(make_shared<http_request>(message), m_model);
			return handler->handle();
		}
	
		message.reply(status_codes::OK, content).then(std::bind(&handle_error, std::placeholders::_1));
	}
	catch (::odata::core::odata_exception &e)
	{
		message.reply(status_codes::OK, U("Exception: ") + e.what()).then(std::bind(&handle_error, std::placeholders::_1));
	}
}

// Handler to process HTTP::POST requests.
void odata_test_service::handle_create(http_request message)
{
	try
	{
		shared_ptr<create_handler> handler = make_shared<create_handler>(make_shared<http_request>(message), m_model);
		return handler->handle();
	}
	catch (::odata::core::odata_exception &e)
	{
		message.reply(status_codes::OK, U("Exception: ") + e.what()).then(std::bind(&handle_error, std::placeholders::_1));
	}
}

}}

using namespace ::odata::service;

int _tmain(int argc, _TCHAR* argv[])
{
	const char* test_edm_model = 
"<edmx:Edmx xmlns:edmx=\"http://docs.oasis-open.org/odata/ns/edmx\" Version=\"4.0\"> \
  <edmx:DataServices> \
    <Schema xmlns=\"http://docs.oasis-open.org/odata/ns/edm\" Namespace=\"Microsoft.OData.SampleService.Models.TripPin\"> \
      <EnumType Name=\"PersonGender\"> \
        <Member Name=\"Male\" Value=\"0\"/> \
        <Member Name=\"Female\" Value=\"1\"/> \
        <Member Name=\"Unknown\" Value=\"2\"/> \
      </EnumType> \
      <ComplexType Name=\"City\"> \
        <Property Name=\"CountryRegion\" Type=\"Edm.String\" Nullable=\"false\"/> \
        <Property Name=\"Name\" Type=\"Edm.String\" Nullable=\"false\"/> \
        <Property Name=\"Region\" Type=\"Edm.String\" Nullable=\"false\"/> \
      </ComplexType> \
      <ComplexType Name=\"Location\" OpenType=\"true\"> \
        <Property Name=\"Address\" Type=\"Edm.String\" Nullable=\"false\"/> \
        <Property Name=\"City\" Type=\"Microsoft.OData.SampleService.Models.TripPin.City\" Nullable=\"false\"/> \
      </ComplexType> \
      <EntityType Name=\"Person\"> \
        <Key> \
          <PropertyRef Name=\"UserName\"/> \
        </Key> \
        <Property Name=\"UserName\" Type=\"Edm.String\" Nullable=\"false\"/> \
        <Property Name=\"FirstName\" Type=\"Edm.String\" Nullable=\"false\"/> \
        <Property Name=\"LastName\" Type=\"Edm.String\" Nullable=\"false\"/> \
        <Property Name=\"Emails\" Type=\"Collection(Edm.String)\" Nullable=\"false\"/> \
        <Property Name=\"AddressInfo\" Type=\"Collection(Microsoft.OData.SampleService.Models.TripPin.Location)\" Nullable=\"false\"/> \
        <Property Name=\"Gender\" Type=\"Microsoft.OData.SampleService.Models.TripPin.PersonGender\"/> \
        <NavigationProperty Name=\"RecentAirports\" Type=\"Collection(Microsoft.OData.SampleService.Models.TripPin.Airport)\"/> \
      </EntityType> \
      <EntityType Name=\"Airport\"> \
        <Key> \
          <PropertyRef Name=\"IcaoCode\"/> \
        </Key> \
        <Property Name=\"IcaoCode\" Type=\"Edm.String\" Nullable=\"false\"/> \
        <Property Name=\"Name\" Type=\"Edm.String\" Nullable=\"false\"/> \
        <Property Name=\"IataCode\" Type=\"Edm.String\" Nullable=\"false\"/> \
        <Property Name=\"Location\" Type=\"Microsoft.OData.SampleService.Models.TripPin.Location\" Nullable=\"false\"/> \
      </EntityType> \
      <EntityContainer Name=\"DefaultContainer\"> \
        <EntitySet Name=\"People\" EntityType=\"Microsoft.OData.SampleService.Models.TripPin.Person\"> \
          <NavigationPropertyBinding Path=\"RecentAirports\" Target=\"Airports\"/> \
        </EntitySet> \
        <EntitySet Name=\"Airports\" EntityType=\"Microsoft.OData.SampleService.Models.TripPin.Airport\"> \
        </EntitySet> \
      </EntityContainer> \
    </Schema> \
  </edmx:DataServices> \
</edmx:Edmx> \
";
	std::istringstream iss(std::move(std::string(test_edm_model)));
	auto model_reader = std::make_shared<::odata::edm::edm_model_reader>(iss);
	model_reader->parse();
	auto model = model_reader->get_model();

	std::shared_ptr<odata_service_document> service_document = std::make_shared<odata_service_document>();
	service_document->add_service_document_element(std::make_shared<odata_service_document_element>(U("People"), U("People"), ENTITY_SET));
	service_document->add_service_document_element(std::make_shared<odata_service_document_element>(U("Airports"), U("Airports"), ENTITY_SET));

	std::wstring address = U("http://localhost:4789");

    odata_test_service listener(address, model, service_document);
	listener.initialize_datasource();
    listener.open().wait();

    std::wcout << ::odata::utility::string_t(U("Listening for requests at: ")) << address << std::endl;

    std::string line;
    std::wcout << U("Hit Enter to close the listener.");
    std::getline(std::cin, line);

    listener.close().wait();

    return 0;
}