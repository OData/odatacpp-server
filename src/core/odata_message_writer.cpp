//---------------------------------------------------------------------
// <copyright file="odata_message_writer.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata/core/odata_message_writer.h"

using namespace odata::utility::json;
using namespace odata::edm;

namespace odata { namespace core
{
	::odata::utility::string_t odata_message_writer::write_service_document(const std::shared_ptr<odata_service_document> service_document)
	{
        auto writer = std::make_shared<::odata::core::odata_json_writer>(m_model, m_service_root);
		return writer->serialize(service_document).to_string();
	}

	::odata::utility::string_t odata_message_writer::write_metadata_document()
	{
        std::ostringstream outstream;;
        std::shared_ptr<edm_model_writer> writer = std::make_shared<edm_model_writer>(outstream);
        writer->write_model(m_model);
        std::string str = outstream.str();

        ::odata::utility::string_t str_t;
        str_t.assign(str.begin(), str.end());

        return str_t;
	}

    ::odata::utility::string_t odata_message_writer::write_odata_value(std::shared_ptr<::odata::core::odata_value> value)
    {
        auto writer = std::make_shared<::odata::core::odata_json_writer>(m_model);
        auto value_context = writer->serialize(value);
        auto ss = value_context.serialize();
        return ss;
    }

    ::odata::utility::string_t odata_message_writer::write_asynchronous_odata_value(std::shared_ptr<::odata::core::odata_value> value, int16_t status_code, ::odata::utility::string_t status_message, std::unordered_map<::odata::utility::string_t, ::odata::utility::string_t> headers)
    {
        ::odata::utility::ostringstream_t stream;
        stream << U("HTTP/1.1 ") << status_code << U(" ") << status_message << U("\n");
        for(auto header = headers.cbegin(); header != headers.cend(); header++)
        {
            stream << header->first << U(": ") <<header->second << U("\n");
        }


        stream << U("\n");
        stream << write_odata_value(value);
        return stream.str();
    }

    ::odata::utility::string_t odata_message_writer::write_odata_batch_value(std::shared_ptr<::odata::core::odata_batch_value> batch_value)
    {
        ::odata::utility::ostringstream_t stream;
        for(auto part = batch_value->cbegin(); part != batch_value->cend(); part++)
        {
            stream << U("--") << batch_value->get_boundary() << U("\n");
            stream << U("Content-Type: application/http");
            stream << U("Content-Transfer-Encoding: binary");
            stream << U("\n");

            stream << U("HTTP/1.1 ") << (*part)->get_status_code() << U(" ") << (*part)->get_status_message() << U("\n");
            for(auto header = (*part)->get_headers().cbegin(); header != (*part)->get_headers().cend(); header++)
            {
                stream << header->first << U(": ") <<header->second << U("\n");
            }

            stream << U("\n");

            stream << write_odata_value((*part)->get_odata_value());
        }

        return stream.str();
    }

    ::odata::utility::string_t odata_message_writer::write_odata_error(std::shared_ptr<::odata::core::odata_error> error)
    {
        auto writer = std::make_shared<::odata::core::odata_json_writer>(m_model);
        auto value_context = writer->serialize(error);
        auto ss = value_context.serialize();
        return ss;
    }

    ::odata::utility::string_t odata_message_writer::write_odata_entity_reference(std::shared_ptr<::odata::core::odata_entity_reference> entity_reference)
    {
        auto writer = std::make_shared<::odata::core::odata_json_writer>(m_model);
        auto value_context = writer->serialize(entity_reference);
        auto ss = value_context.serialize();
        return ss;
    }

    ::odata::utility::string_t odata_message_writer::write_odata_entity_reference_collection(std::shared_ptr<::odata::core::odata_entity_reference_collection> entity_reference_collection)
    {
        auto writer = std::make_shared<::odata::core::odata_json_writer>(m_model);
        auto value_context = writer->serialize(entity_reference_collection);
        auto ss = value_context.serialize();
        return ss;
    }

}}