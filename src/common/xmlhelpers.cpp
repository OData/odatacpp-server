//---------------------------------------------------------------------
// <copyright file="xmlhelpers.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

/***
* ==++==
*
* Copyright (c) Microsoft Corporation. All rights reserved.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* xmlhelpers.cpp
*
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#include "odata/common/xmlhelpers.h"

#ifdef WIN32
#include "odata/common/xmlstream.h"
#else
#include "odata/common/asyncrt_utils.h"
typedef int XmlNodeType;
#define XmlNodeType_Element XML_READER_TYPE_ELEMENT
#define XmlNodeType_Text XML_READER_TYPE_TEXT
#define XmlNodeType_EndElement XML_READER_TYPE_END_ELEMENT
#endif

using std::istream;
using std::string;
using namespace ::odata::utility;

namespace odata { namespace edm {

    void xml_reader::initialize(istream& stream)
    {
        remove_bom(stream);

#ifdef WIN32
        HRESULT hr;
        CComPtr<IStream> pInputStream;
        pInputStream.Attach(xmlstring_istream::create(stream));

        if (pInputStream == nullptr)
        {
            auto error = ERROR_NOT_ENOUGH_MEMORY;
            string msg("XML reader IStream creation failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }

        if (FAILED(hr = CreateXmlReader(__uuidof(IXmlReader), (void**)&m_reader, NULL)))
        {
            auto error = GetLastError();
            string msg("XML reader CreateXmlReader failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }

        if (FAILED(hr = m_reader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit)))
        {
            auto error = GetLastError();
            string msg("XML reader SetProperty failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }

        if (FAILED(hr = m_reader->SetInput(pInputStream)))
        {
            auto error = GetLastError();
            string msg("XML reader SetInput failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
#else
        m_data = read_to_end(stream);
        const char* xmlBuffer = m_data.data();
        unsigned int size = static_cast<unsigned int>(m_data.size());
        xmlInitParser();
        m_reader = xmlReaderForMemory(xmlBuffer, size, "", "", XML_PARSE_RECOVER);
#endif
    }

    /// <summary>
    /// Remove Byte Order Mark from the stream
    /// </summary>
    void xml_reader::remove_bom(istream& stream)
    {
        // Synchronous.
        if (stream.peek() == 0xEF
            && stream.peek() == 0xBB
            && stream.peek() == 0xBF)
        {
            stream.get();
            stream.get();
            stream.get();
            return;
        }
    }

    bool xml_reader::parse()
    {
        if (m_streamDone) return false;
        // Set this to true each time the parse routine is invoked. Most derived readers will only invoke parse once.
        m_continueParsing = true;

        // read until there are no more nodes
#ifdef WIN32
        HRESULT hr;
        XmlNodeType nodeType;
        while (m_continueParsing && S_OK == (hr = m_reader->Read(&nodeType)))
        {
#else
        if (m_reader == nullptr)
            return !m_continueParsing; // no XML document to read

        while (m_continueParsing && xmlTextReaderRead(m_reader))
        {
            auto nodeType = xmlTextReaderNodeType(m_reader);
#endif
            switch (nodeType)
            {

            case XmlNodeType_Element:
            {
                auto name = get_current_element_name();
#ifdef WIN32
                BOOL is_empty_element = m_reader->IsEmptyElement();
#else
                bool is_empty_element = xmlTextReaderIsEmptyElement(m_reader);
#endif
                m_elementStack.push_back(name);
                handle_begin_element(name);

                if (is_empty_element)
                {
                    handle_end_element(name);
                    m_elementStack.pop_back();
                }
            }
                break;

            case XmlNodeType_Text:
                handle_element(m_elementStack.back());
                break;

            case XmlNodeType_EndElement:
                handle_end_element(m_elementStack.back());
                m_elementStack.pop_back();
                break;

            default:
                break;
            }
        }

        // If the loop was terminated because there was no more to read from the stream, set m_streamDone to true, so exit early
        // the next time parse is invoked.
        if (m_continueParsing) m_streamDone = true;
        // Return false if the end of the stream was reached and true if parsing was paused. The return value indicates whether
        // parsing can be resumed.
        return !m_continueParsing;
    }

    string_t xml_reader::get_parent_element_name(size_t pos)
    {
        if (m_elementStack.size() > pos + 1)
        {
            size_t currentDepth = m_elementStack.size() - 1;
            size_t parentDepth = currentDepth - 1;

            if (pos <= parentDepth)
            {
                return m_elementStack[parentDepth - pos];
            }
        }

        // return empty string
        return string_t();
    }

    string_t xml_reader::get_current_element_name()
    {
#ifdef WIN32
        HRESULT hr;
        const wchar_t * pwszLocalName = NULL;

        if (FAILED(hr = m_reader->GetLocalName(&pwszLocalName, NULL)))
        {
            auto error = GetLastError();
            string msg("XML reader GetLocalName failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
        return string_t(pwszLocalName);
#else
        xmlChar* valueXml = xmlTextReaderLocalName(m_reader);
        std::string input((char*)valueXml);
        ::odata::utility::string_t stringt = ::odata::utility::conversions::to_string_t(input);
        xmlFree(valueXml);
        return stringt;
#endif
    }

    string_t xml_reader::get_current_element_name_with_prefix()
    {
#ifdef WIN32
        HRESULT hr;
        const wchar_t * pwszName = NULL;

        if (FAILED(hr = m_reader->GetQualifiedName(&pwszName, NULL)))
        {
            auto error = GetLastError();
            string msg("XML reader GetQualified failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
        return string_t(pwszName);
#else
        throw std::runtime_error("Not implemented");
#endif
    }

    string_t xml_reader::get_current_element_text()
    {
#ifdef WIN32
        HRESULT hr;
        const wchar_t * pwszValue;

        if (FAILED(hr = m_reader->GetValue(&pwszValue, NULL)))
        {
            auto error = GetLastError();
            string msg("XML reader GetValue failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }

        return string_t(pwszValue);
#else
        xmlChar* valueXml = xmlTextReaderValue(m_reader);
        std::string input((char*)valueXml);
        ::odata::utility::string_t stringt = ::odata::utility::conversions::to_string_t(input);
        xmlFree(valueXml);
        return stringt;
#endif
    }

    bool xml_reader::move_to_first_attribute()
    {
#ifdef WIN32
        HRESULT hr;
        if (FAILED(hr = m_reader->MoveToFirstAttribute()))
        {
            auto error = GetLastError();
            string msg("XML reader MoveToFirstAttribute failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
        return (hr == S_OK);
#else
        return xmlTextReaderMoveToFirstAttribute(m_reader) > 0;
#endif
    }

    bool xml_reader::move_to_next_attribute()
    {
#ifdef WIN32
        HRESULT hr;
        if (FAILED(hr = m_reader->MoveToNextAttribute()))
        {
            auto error = GetLastError();
            string msg("XML reader MoveToNextAttribute failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
        return (hr == S_OK);
#else
        return xmlTextReaderMoveToNextAttribute(m_reader) > 0;
#endif
    }
    
#ifdef WIN32
    ::odata::utility::string_t xml_reader::read_to_end(std::istream&)
    {
        throw std::runtime_error("xml_reader::read_to_end() should not be called in Windows");
    }
#else // LINUX
    ::odata::utility::string_t xml_reader::read_to_end(std::istream& stream)
    {
        ::odata::utility::string_t text;
        while (stream.good())
        {
            int c = stream.get();
            if (c == -1)
            {
                break;
            }
            text.push_back(c);
        }
        return text;
    }
#endif

    void xml_writer::initialize(std::ostream& stream)
    {
#ifdef WIN32
        HRESULT hr;
        CComPtr<IStream> pStream;
        pStream.Attach(xmlstring_ostream::create(stream));

        if (pStream == nullptr)
        {
            auto error = ERROR_NOT_ENOUGH_MEMORY;
            string msg("XML writer IStream creation failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }

        if (FAILED(hr = CreateXmlWriter(__uuidof(IXmlWriter), (void**)&m_writer, NULL)))
        {
            auto error = GetLastError();
            string msg("XML writer CreateXmlWriter failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }

        if (FAILED(hr = m_writer->SetOutput(pStream)))
        {
            auto error = GetLastError();
            string msg("XML writer SetOutput failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }

        if (FAILED(hr = m_writer->SetProperty(XmlWriterProperty_Indent, TRUE)))
        {
            auto error = GetLastError();
            string msg("XML writer SetProperty failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }

        if (FAILED(hr = m_writer->WriteStartDocument(XmlStandalone_Omit)))
        {
            auto error = GetLastError();
            string msg("XML writer WriteStartDocument failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
#else // LINUX
        m_writer = xmlNewTextWriterDoc(&m_doc, 0);
        m_stream = &stream;
#endif
    }

    void xml_writer::finalize()
    {
#ifdef WIN32
        HRESULT hr;

        if (FAILED(hr = m_writer->WriteEndDocument()))
        {
            auto error = GetLastError();
            string msg("XML writer WriteEndDocument failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
        if (FAILED(hr = m_writer->Flush()))
        {
            auto error = GetLastError();
            string msg("XML writer Flush failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
#else // LINUX
        xmlChar *memory;
        int size;
        xmlDocDumpMemory(m_doc, &memory, &size);
        *m_stream << memory;
        xmlFree(memory);

#endif
    }

    void xml_writer::write_start_element_with_prefix(const string_t& elementPrefix, const string_t& elementName, const string_t& namespaceName)
    {
#ifdef WIN32
        HRESULT hr;
        if (FAILED(hr = m_writer->WriteStartElement(elementPrefix.c_str(), elementName.c_str(), namespaceName.empty() ? NULL : namespaceName.c_str())))
        {
            auto error = GetLastError();
            string msg("XML writer WriteStartElement with prefix failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
#else 
        xmlChar* valueXml = (xmlChar*) ::odata::utility::conversions::to_utf8string(elementName).c_str();
        xmlTextWriterStartElement(m_writer, valueXml);
        xmlFree(valueXml);
#endif
    }

    void xml_writer::write_start_element(const string_t& elementName, const string_t& namespaceName)
    {
#ifdef WIN32
        HRESULT hr;

        if (FAILED(hr = m_writer->WriteStartElement(NULL, elementName.c_str(), namespaceName.empty() ? NULL : namespaceName.c_str())))
        {
            auto error = GetLastError();
            string msg("XML writer WriteStartElement failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
#else 
        write_start_element_with_prefix(U(""), elementName, namespaceName);
#endif
    }

    void xml_writer::write_end_element()
    {
#ifdef WIN32
        HRESULT hr;

        if (FAILED(hr = m_writer->WriteEndElement()))
        {
            auto error = GetLastError();
            string msg("XML writer WriteEndElement failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
#else
        xmlTextWriterEndElement(m_writer);
#endif
    }

    void xml_writer::write_full_end_element()
    {
#ifdef WIN32
        HRESULT hr;

        if (FAILED(hr = m_writer->WriteFullEndElement()))
        {
            auto error = GetLastError();
            string msg("XML writer WriteFullEndElement failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
#else
        throw std::runtime_error("Not implemented");
#endif
    }

    void xml_writer::write_string(const string_t& str)
    {
#ifdef WIN32
        HRESULT hr;

        if (FAILED(hr = m_writer->WriteString(str.c_str())))
        {
            auto error = GetLastError();
            string msg("XML writer WriteString failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
#else
        UNREFERENCED_PARAMETER(str);
        throw std::runtime_error("Not implemented");
#endif
    }


    void xml_writer::write_attribute_string(const string_t& prefix, const string_t& name, const string_t& namespaceUri, const string_t& value)
    {
#ifdef WIN32
        HRESULT hr;

        if (FAILED(hr = m_writer->WriteAttributeString(prefix.empty() ? NULL : prefix.c_str(),
            name.empty() ? NULL : name.c_str(),
            namespaceUri.empty() ? NULL : namespaceUri.c_str(),
            value.empty() ? NULL : value.c_str())))
        {
            auto error = GetLastError();
            string msg("XML writer WriteAttributeString failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
#else
        xmlChar* nameXml = (xmlChar*) ::odata::utility::conversions::to_utf8string(name).c_str();
        xmlChar* valueXml = (xmlChar*) ::odata::utility::conversions::to_utf8string(value).c_str();
        xmlTextWriterWriteAttribute(m_writer, nameXml, valueXml);
        xmlFree(nameXml);
        xmlFree(valueXml);
#endif
    }

    void xml_writer::write_element(const string_t& elementName, const string_t& value)
    {
#ifdef WIN32
        HRESULT hr;

        if (FAILED(hr = m_writer->WriteElementString(NULL, elementName.c_str(), NULL, value.c_str())))
        {
            auto error = GetLastError();
            string msg("XML writer WriteElementString failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
#else // LINUX
        write_element_with_prefix(U(""), elementName, value);
#endif
    }

    void xml_writer::write_element_with_prefix(const string_t& prefix, const string_t& elementName, const string_t& value)
    {
#ifdef WIN32
        HRESULT hr;

        if (FAILED(hr = m_writer->WriteElementString(prefix.c_str(), elementName.c_str(), NULL, value.c_str())))
        {
            auto error = GetLastError();
            string msg("XML writer WriteElementStringWithPrefix failed");
            log_error_message(msg, error);
            throw std::runtime_error(msg);
        }
#else
        write_start_element_with_prefix(prefix, elementName);
        write_string(value);
        write_end_element();
#endif
    }
}}
// namespace odata::edm
