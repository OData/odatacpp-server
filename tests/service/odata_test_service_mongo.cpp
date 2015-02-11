//---------------------------------------------------------------------
// <copyright file="odata_test_service_mongo.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include <cstdlib>
#include <WinSock2.h>

#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>

#include "odata_test_service.h"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

using namespace ::odata::core;

namespace odata { namespace service
{

// TODO replace ::utility::string_t with web::url
odata_test_service::odata_test_service(::utility::string_t url, std::shared_ptr<::odata::edm::edm_model> model, std::shared_ptr<odata_service_document> service_document)
	: m_listener(url), m_model(model), m_service_document(service_document)
{
	m_uri_parser = std::make_shared<::odata::core::odata_uri_parser>(model);
	m_service_root = web::uri(url);
    m_listener.support(methods::GET, std::bind(&odata_test_service::handle_get, this, std::placeholders::_1));
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

void odata_test_service::initialize_datasource()
{
	string db_name("trippin");

	mongo::DBClientConnection c;
	c.connect("localhost");

	c.dropDatabase(db_name);

	string people_collection = db_name + "." + "People";
	vector<mongo::BSONObj> people_data;
	people_data.push_back(BSON("UserName" << "russellwhyte" << "FirstName" << "Russell" << "LastName" << "Whyte" << "Gender" << "Male"
		<< "Emails" << BSON_ARRAY("Russell@example.com" << "Russell@contoso.com")
		<< "AddressInfo" << BSON_ARRAY(BSON("Address" << "187 Suffolk Ln." << "City" << BSON("CountryRegion" << "United States" << "Name" << "Boise" << "Region" << "ID")))
		<< "RecentAirports" << BSON_ARRAY("KSFO" << "KLAX")
		));
	people_data.push_back(BSON("UserName" << "scottketchum" << "FirstName" << "Scott" << "LastName" << "Ketchum" << "Gender" << "Male"
		<< "Emails" << BSON_ARRAY("Scott@example.com")
		<< "AddressInfo" << BSON_ARRAY(BSON("Address" << "2817 Milton Dr." << "City" << BSON("CountryRegion" << "United States" << "Name" << "Albuquerque" << "Region" << "NM")))
		<< "RecentAirports" << BSON_ARRAY("KLAX")
		));
	people_data.push_back(BSON("UserName" << "ronaldmundy" << "FirstName" << "Ronald" << "LastName" << "Mundy" << "Gender" << "Male"
		<< "Emails" << BSON_ARRAY("Ronald@example.com" << "Ronald@contoso.com")
		<< "AddressInfo" << mongo::BSONArrayBuilder().arr()
		<< "RecentAirports" << BSON_ARRAY("ZSSS" << "ZBAA")
		));
	people_data.push_back(BSON("UserName" << "javieralfred" << "FirstName" << "Javier" << "LastName" << "Alfred" << "Gender" << "Male"
		<< "Emails" << BSON_ARRAY("Javier@example.com" << "Javier@contoso.com")
		<< "AddressInfo" << BSON_ARRAY(BSON("Address" << "89 Jefferson Way Suite 2" << "City" << BSON("CountryRegion" << "United States" << "Name" << "Portland" << "Region" << "WA")))
		<< "RecentAirports" << mongo::BSONArrayBuilder().arr()
		));
	people_data.push_back(BSON("UserName" << "willieashmore" << "FirstName" << "Willie" << "LastName" << "Ashmore" << "Gender" << "Male"
		<< "Emails" << BSON_ARRAY("Willie@example.com" << "Willie@contoso.com")
		<< "AddressInfo" << mongo::BSONArrayBuilder().arr()
		<< "RecentAirports" << mongo::BSONArrayBuilder().arr()
		));

	string airports_collection = db_name + "." + "Airports";
	vector<mongo::BSONObj> airports_data;
	airports_data.push_back(BSON("IcaoCode" << "KSFO" << "Name" << "San Francisco International Airport" << "IataCode" << "SFO"
		<< "Location" << BSON("Address" << "South McDonnell Road, San Francisco, CA 94128"
		<< "City" << BSON("CountryRegion" << "United States" << "Name" << "San Francisco" << "Region" << "California"))
		));
	airports_data.push_back(BSON("IcaoCode" << "KLAX" << "Name" << "Los Angeles International Airport" << "IataCode" << "LAX"
		<< "Location" << BSON("Address" << "1 World Way, Los Angeles, CA, 90045"
		<< "City" << BSON("CountryRegion" << "United States" << "Name" << "Los Angeles" << "Region" << "California"))
		));
	airports_data.push_back(BSON("IcaoCode" << "ZSSS" << "Name" << "Shanghai Hongqiao International Airport" << "IataCode" << "SHA"
		<< "Location" << BSON("Address" << "Hongqiao Road 2550, Changning District"
		<< "City" << BSON("CountryRegion" << "China" << "Name" << "Shanghai" << "Region" << "Shanghai"))
		));
	airports_data.push_back(BSON("IcaoCode" << "ZBAA" << "Name" << "Beijing Capital International Airport" << "IataCode" << "PEK"
		<< "Location" << BSON("Address" << "Airport Road, Chaoyang District, Beijing, 100621"
		<< "City" << BSON("CountryRegion" << "China" << "Name" << "Beijing" << "Region" << "Beijing"))
		));

	c.insert(people_collection, people_data);
	c.insert(airports_collection, airports_data);
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
		auto path_segments = parsed_uri->path()->segments();

		string db_name("trippin");
		string collection_name;
		mongo::BSONObjBuilder query_builder;
		mongo::BSONObjBuilder projection_builder;

		for (auto iter = path_segments.cbegin(); iter != path_segments.cend(); iter++)
		{
			shared_ptr<odata_path_segment> seg = *iter;
			switch (seg->segment_type())
			{
			case odata_path_segment_type::EntitySet:
				{
					shared_ptr<odata_entity_set_segment> entityset_seg = seg->as<odata_entity_set_segment>();
					auto entityset_name = entityset_seg->entity_set()->get_name();
				
					collection_name.assign(entityset_name.cbegin(), entityset_name.cend());
					break;
				}

			case odata_path_segment_type::Key:
				{
					shared_ptr<odata_key_segment> key_seg = seg->as<odata_key_segment>();
					auto keys = key_seg->keys();
					
					for (auto key = keys.cbegin(); key != keys.cend(); key++)
					{
						string key_name_s, key_value_s;
						key_name_s.assign(key->first.cbegin(), key->first.cend());
						auto second = key->second->as<string_t>();
						key_value_s.assign(second.cbegin(), second.cend());
						query_builder << key_name_s << key_value_s;
					}
					break;
				}

			case odata_path_segment_type::StructuralProperty:
				{
					shared_ptr<odata_structural_property_segment> property_seg = seg->as<odata_structural_property_segment>();
					string_t property_name = property_seg->property()->get_name();
					
					string property_name_s;
					property_name_s.assign(property_name.cbegin(), property_name.cend());
					projection_builder << property_name_s << 1;

					break;
				}

			default:

				break;
			}
			
		}

		mongo::DBClientConnection c;
		c.connect("localhost");
		
		collection_name = db_name + "." + collection_name;
		auto cursor = c.query(collection_name, query_builder.obj(), 0, 0, &projection_builder.obj());

		::utility::string_t content;
		
		string retrived_data;
		while (cursor->more())
		{
			auto next = cursor->next();
			next.removeField("_id");
			retrived_data += next.toString();
		}
		content = conversions::to_string_t(retrived_data);

		odata_message_writer writer(m_model, m_service_root);
		
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
	
		message.reply(status_codes::OK, content).then(std::bind(&handle_error, std::placeholders::_1));
	}
	catch (::odata::core::odata_exception &e)
	{
		message.reply(status_codes::OK, U("Exception: ") + e.what()).then(std::bind(&handle_error, std::placeholders::_1));
	}
	////Get odata objects from resorce and odata_path
	//
	//odata_message_writer writer(model);
	//if (it is feed)
	//	odata_feed_writer feed_writer = writer.create_feed_writer();
	//	feed_writer.write_start(feed);
	//	feed_writer.write_start(entry);
	//	feed_writer.write_end();
	//	feed_writer.write_end();
	//else if (it is entry)
	//	odata_entry_writer entry_writer = writer.create_entry_writer();
	//	entry_writer.write_start(entry);
	//	entry_writer.write_end();
}

}}

using namespace ::odata::service;

int main(int argc, char* argv[])
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

	auto model_reader = std::make_shared<::odata::edm::edm_model_reader>(concurrency::streams::bytestream::open_istream(std::string(test_edm_model)));
	model_reader->parse();
	auto model = model_reader->get_model();

	std::shared_ptr<odata_service_document> service_document = std::make_shared<odata_service_document>();
	service_document->add_service_document_element(std::make_shared<odata_service_document_element>(U("People"), U("People"), ENTITY_SET));
	service_document->add_service_document_element(std::make_shared<odata_service_document_element>(U("Airports"), U("Airports"), ENTITY_SET));

	std::wstring address = U("http://localhost:4789");

    odata_test_service listener(address, model, service_document);
    listener.open().wait();

    std::wcout << ::utility::string_t(U("Listening for requests at: ")) << address << std::endl;

    std::string line;
    std::wcout << U("Hit Enter to close the listener.");
    std::getline(std::cin, line);

    listener.close().wait();

    return 0;
}