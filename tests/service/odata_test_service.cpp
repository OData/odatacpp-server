//---------------------------------------------------------------------
// <copyright file="odata_test_service.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata_test_service.h"

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;
using namespace odata::edm;
using namespace odata::core;

namespace odata { namespace service
{

    // TODO replace ::odata::utility::string_t with web::url
    odata_test_service::odata_test_service(::odata::utility::string_t url, std::shared_ptr<::odata::edm::edm_model> model, std::shared_ptr<odata_service_document> service_document)
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
            bool is_async = false;
            auto prefer_header = message.headers().find(U("Prefer"));
            if (prefer_header != message.headers().end())
            {
                if (prefer_header->second.find(U("respond-async")) != ::odata::utility::string_t::npos)
                {
                    is_async = true;
                }
            }


            auto parsed_uri = m_uri_parser->parse_uri(message.relative_uri());

            odata_message_writer writer(m_model, m_service_root);
            odata_context_url_builder context_url_builder(m_model, m_service_root);
            odata_metadata_builder metadata_builder(m_model, m_service_root);
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
                if (parsed_uri->path()->size() >= 1)
                {
                    if (parsed_uri->path()->segment_at(0)->segment_type() == odata_path_segment_type::EntitySet)
                    {
                        auto entity_set_segment = parsed_uri->path()->segment_at(0)->as<odata_entity_set_segment>();
                        if (entity_set_segment->entity_set()->get_name() == U("People"))
                        {
                            if (parsed_uri->path()->size() == 1)
                            {
                                auto people = get_people();
                                auto context_url = context_url_builder.get_context_uri_for_collection_of_entities(entity_set_segment->entity_set());
                                people->set_context_url(context_url);
                   
                                if (is_async)
                                {
                                    std::unordered_map<string_t, string_t> headers;
                                    headers[U("OData-Version")] = U("4.0");
                                    headers[U("Content-Type")] = U("application/json;odata.metadata=full");

                                    content = writer.write_asynchronous_odata_value(people, 200, U("OK"), headers);
                                }
                                else
                                {
                                    content = writer.write_odata_value(people);
                                }
                            }
                            else if (parsed_uri->path()->segment_at(1)->segment_type() == odata_path_segment_type::Key)
                            {
                                auto key_segment = parsed_uri->path()->segment_at(1)->as<odata_key_segment>();
                                auto key = key_segment->keys()[0].second->as<::odata::utility::string_t>();

                                auto single_person = get_single_people(key);
                                single_person->set_is_top_level(true);
                                auto context_url = context_url_builder.get_context_uri_for_entity(entity_set_segment->entity_set());
                                single_person->set_context_url(context_url);
                                
                                auto id = metadata_builder.get_entity_id(single_person, entity_set_segment->entity_set());
                                single_person->set_id(id);

                                auto read_link = metadata_builder.get_read_link(single_person, entity_set_segment->entity_set());
                                single_person->set_read_link(read_link);

                                auto edit_link = metadata_builder.get_edit_link(single_person, entity_set_segment->entity_set());
                                single_person->set_edit_link(edit_link);



                                content = writer.write_odata_value(single_person);
                            }
                        }
                    }
                }
            }

            message.reply(status_codes::OK, content).then(std::bind(&handle_error, std::placeholders::_1));
        }
        catch (::odata::core::odata_exception &e)
        {
            message.reply(status_codes::BadRequest, U("Exception: ") + e.what()).then(std::bind(&handle_error, std::placeholders::_1));
        }
        ////Get odata objects from resorce and odata_path
        //

    }

    std::shared_ptr<odata_value> odata_test_service::get_people()
    {
        auto people_type = m_model->find_entity_type(U("Person"));
        auto people_collection_type = std::make_shared<edm_collection_type>(people_type);
        auto people_collection = std::make_shared<odata_collection_value>(people_collection_type);

        people_collection->add_collection_value(get_single_people(U("russellwhyte")));

        people_collection->set_is_top_level(true);
        people_collection->set_next_link(U("http://localhost:4789/$metadata#People?$skiptoken=1"));
        people_collection->set_delta_link(U("http://localhost:4789/$metadata#People?$deltatoken=0826"));
        people_collection->set_count(1);
        return people_collection;
    }

    std::shared_ptr<odata_entity_value> odata_test_service::get_single_people(::odata::utility::string_t name)
    {
        if (name != U("russellwhyte"))
        {
            return std::shared_ptr<odata_entity_value>();
        }
        auto people_type = m_model->find_entity_type(U("Person"));
        
        auto people1 = std::make_shared<odata_entity_value>(people_type);
        people1->set_value(U("UserName"), U("russellwhyte"));
        people1->set_value(U("FirstName"), U("Russell"));
        people1->set_value(U("LastName"),U("Whyte"));

        auto string_collection_type = std::make_shared<edm_collection_type>(::odata::edm::edm_primitive_type::STRING());
        auto emails_value = std::make_shared<odata_collection_value>(string_collection_type);

        emails_value->add_collection_value(std::make_shared<odata_primitive_value>(::odata::edm::edm_primitive_type::STRING(), U("Russell@example.com")));
        emails_value->add_collection_value(std::make_shared<odata_primitive_value>(::odata::edm::edm_primitive_type::STRING(), U("Russell@contoso.com")));
        people1->set_value(U("Emails"),emails_value);

        auto location_type = m_model->find_complex_type(U("Location"));
        auto location_collection_type = std::make_shared<edm_collection_type>( location_type);
        auto location_collection = std::make_shared<odata_collection_value>(location_collection_type);
        auto location1 = std::make_shared<odata_complex_value>(location_type);
        location1->set_value(U("Address"), U("187 Suffolk Ln."));

        auto city_type = m_model->find_complex_type(U("City"));
        auto city1 = std::make_shared<odata_complex_value>(city_type);
        city1->set_value(U("CountryRegion"), U("United States"));
        city1->set_value(U("Name"), U("Boise"));
        city1->set_value(U("Region"), U("ID"));
        location1->set_value(U("City"), city1);
        location_collection->add_collection_value(location1);
        people1->set_value(U("AddressInfo"), location_collection);

        auto gender_type = m_model->find_enum_type(U("PersonGender"));
        auto gender = std::make_shared<odata_enum_value>(gender_type, U("Male"));
        people1->set_value(U("Gender"),gender);
        people1->set_value(U("Concurrency"),635435960069149000L);

        people1->set_etag(U("test etag"));


        return people1;
    }

}}

using namespace ::odata::service;

int _tmain(int argc, _TCHAR* argv[])
{
    ::odata::utility::string_t directory = argv[0];
    directory.erase(directory.find_last_of('\\')+1);
    ::odata::utility::string_t model_file = directory + U("odata_test_service_metadata.xml");

	std::ifstream ifs(model_file);
    auto model_reader = std::make_shared<edm_model_reader>(ifs);

    model_reader->parse();
    auto model = model_reader->get_model();

    std::shared_ptr<odata_service_document> service_document = std::make_shared<odata_service_document>();
    service_document->add_service_document_element(std::make_shared<odata_service_document_element>(U("People"), U("People"), ENTITY_SET));
    service_document->add_service_document_element(std::make_shared<odata_service_document_element>(U("Customers"), U("Customers"), ENTITY_SET));
    service_document->add_service_document_element(std::make_shared<odata_service_document_element>(U("Employees"), U("Employees"), ENTITY_SET));
    service_document->add_service_document_element(std::make_shared<odata_service_document_element>(U("Products"), U("Products"), ENTITY_SET));
    service_document->add_service_document_element(std::make_shared<odata_service_document_element>(U("Orders"), U("Orders"), ENTITY_SET));
    service_document->add_service_document_element(std::make_shared<odata_service_document_element>(U("Company"), U("Company"), SINGLETON));
    service_document->add_service_document_element(std::make_shared<odata_service_document_element>(U("GetProductsByAccessLevel"), U("GetProductsByAccessLevel"), FUNCTION_IMPORT));

    std::wstring address = U("http://localhost:4789");

    odata_test_service listener(address, model, service_document);
    listener.open().wait();

    std::wcout << ::odata::utility::string_t(U("Listening for requests at: ")) << address << std::endl;

    std::string line;
    std::wcout << U("Hit Enter to close the listener.");
    std::getline(std::cin, line);

    listener.close().wait();

    return 0;
}