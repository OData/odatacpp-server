//---------------------------------------------------------------------
// <copyright file="odata_uri_parser.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata/core/odata_uri_parser.h"

#define ERROR_MESSAGE_NOT_RELATIVE_URI() (U("not relative uri"))
#define ERROR_MESSAGE_UNSUPPORTED_SEGMENT(IDENTIFIER) ((IDENTIFIER) + U(" unsupported"))
#define ERROR_MESSAGE_RESOURCE_NOT_FOUND(IDENTIFIER) (U("resource ") + (IDENTIFIER) + U(" not found"))
#define ERROR_MESSAGE_UNEXPECTED_PATH_ROOT(IDENTIFIER) (U("unexpected ") + (IDENTIFIER) + U(" on path root"))
#define ERROR_MESSAGE_UNEXPECTED_PARENTHESIS_EXPRESSION(IDENTIFIER) ((IDENTIFIER) + U(" cannot have parenthesis expression"))
#define ERROR_MESSAGE_EMPTY_SEGMENT_IDENTIFIER() (U("empty segment identifier"))
#define ERROR_MESSAGE_PARENTHESIS_MISMATCH() (U("parenthesis mismatch"))
#define ERROR_MESSAGE_NAME_MUST_BE_PROVIDED() (U("name must be provided for each value"))
#define ERROR_MESSAGE_NAME_MUST_NOT_DUPLICATE(NAME) (U("name ") + (NAME) + U(" must not duplicate"))
#define ERROR_MESSAGE_KEY_COUNT_MISMATCH() (U("key count mismatch"))
#define ERROR_MESSAGE_KEY_NOT_FOUND(NAME) (U("key name ") + (NAME) + U(" not found"))
#define ERROR_MESSAGE_CLOSE_CHAR_EXPECTED(CHAR) (::odata::utility::string_t(U("close char ")) + (CHAR) + U(" expected"))
#define ERROR_MESSAGE_NOT_OPEN_TYPE(NAME) (U("dynamic property ") +(NAME) + U(" under nonopen type"))
#define ERROR_MESSAGE_NO_NAVIGATION_SOURCE() (U("no navigation source for navigation property"))
#define ERROR_MESSAGE_PARAMETER_COUNT_MISMATCH() (U("parameter count mismatch"))
#define ERROR_MESSAGE_PARAMETER_NOT_FOUND(NAME) (U("parameter name ") + (NAME) + U(" not found"))
#define ERROR_MESSAGE_BOUND_OPERATION_IMPORT_FOUND(NAME) (U("bound operation import ") + (NAME) + U(" found"))
#define ERROR_MESSAGE_UNBOUND_OPERATION_FOUND(NAME) (U("unbound operation ") + (NAME) + U(" found"))
#define ERROR_MESSAGE_BINDING_PARAMETER_NOT_FOUND(NAME) (U("binding parameter for ") + (NAME) + U(" not found"))
#define ERROR_MESSAGE_BINDING_TYPE_MISMATCH(NAME) (U("binding type for ") + (NAME) + U(" mismatch"))
#define ERROR_MESSAGE_SEGMENT_FOLLOW(NAME) (::odata::utility::string_t(U("segment follow ")) + (NAME))
#define ERROR_MESSAGE_SEGMENT_FOLLOW_NONCOMPOSABLE(NAME) (U("segment follow noncomposable operation (import) ") + (NAME))
#define ERROR_MESSAGE_INVALID_COUNT_QUERY(QUERY) (U("invalid count query ") + (QUERY))
#define ERROR_MESSAGE_INVALID_CHARACTER(CHAR) (::odata::utility::string_t(U("invalid character ")) + (CHAR))
#define ERROR_MESSAGE_UNEXPECTED_TOKEN(NAME) (U("unexpected token ") + (NAME))
#define ERROR_MESSAGE_TOKEN_KIND_EXPECTED(KIND) (::odata::utility::string_t(U("token kind expected ")) + (KIND))
#define ERROR_MESSAGE_CHAR_KIND_EXPECTED(KIND) (::odata::utility::string_t(U("char kind expected ")) + (KIND))
#define ERROR_MESSAGE_INVALID_VALUE(VALUE) (U("invalid value ") + VALUE)
#define ERROR_MESSAGE_EMPTY_NAME_VALUE_PAIR() (U("empty name-value pair"))
#define ERROR_MESSAGE_DUPLICATE_RANGE_VARIABLE(NAME) (U("duplicate range variable ") + (NAME))
#define ERROR_MESSAGE_FILTER_ON_NONCOLLECTION() (U("$filter on non-collection resource"))
#define ERROR_MESSAGE_ORDERBY_ON_NONCOLLECTION() (U("$orderby on non-collection resource"))
#define ERROR_MESSAGE_SELECT_OR_EXPAND_ON_NONSTRUCTURED_TYPE() (U("$select or $expand on non-structured type"))

static const ::odata::utility::string_t SegmentIdentifierMetadata = U("$metadata");
static const ::odata::utility::string_t SegmentIdentifierBatch = U("$batch");
static const ::odata::utility::string_t SegmentIdentifierCount = U("$count");
static const ::odata::utility::string_t SegmentIdentifierValue = U("$value");
static const ::odata::utility::string_t SegmentIdentifierRef = U("$ref");
static const ::odata::utility::string_t SegmentIdentifierAll = U("$all");
static const ::odata::utility::string_t SegmentIdentifierEntity = U("$entity");
static const ::odata::utility::string_t SegmentIdentifierCrossJoin = U("$crossjoin");

static const ::odata::utility::string_t QueryOptionSelect = U("$select");
static const ::odata::utility::string_t QueryOptionExpand = U("$expand");
static const ::odata::utility::string_t QueryOptionFilter = U("$filter");
static const ::odata::utility::string_t QueryOptionOrderBy = U("$orderby");
static const ::odata::utility::string_t QueryOptionSearch = U("$search");
static const ::odata::utility::string_t QueryOptionTop = U("$top");
static const ::odata::utility::string_t QueryOptionSkip = U("$skip");
static const ::odata::utility::string_t QueryOptionCount = U("$count");

static const ::odata::utility::string_t KeywordTrue = U("true");
static const ::odata::utility::string_t KeywordFalse = U("false");
static const ::odata::utility::string_t KeywordNull = U("null");
static const ::odata::utility::string_t KeywordOr = U("or");
static const ::odata::utility::string_t KeywordAnd = U("and");
static const ::odata::utility::string_t KeywordEqual = U("eq");
static const ::odata::utility::string_t KeywordNotEqual = U("ne");
static const ::odata::utility::string_t KeywordGreaterThan = U("gt");
static const ::odata::utility::string_t KeywordGreaterThanOrEqual = U("ge");
static const ::odata::utility::string_t KeywordLessThan = U("lt");
static const ::odata::utility::string_t KeywordLessThanOrEqual = U("le");
static const ::odata::utility::string_t KeywordHas = U("has");
static const ::odata::utility::string_t KeywordAdd = U("add");
static const ::odata::utility::string_t KeywordSub = U("sub");
static const ::odata::utility::string_t KeywordMultiply = U("mul");
static const ::odata::utility::string_t KeywordDivide = U("div");
static const ::odata::utility::string_t KeywordModulo = U("mod");
static const ::odata::utility::string_t KeywordNot = U("not");
static const ::odata::utility::string_t KeywordAll = U("all");
static const ::odata::utility::string_t KeywordAny = U("any");
static const ::odata::utility::string_t KeywordAsc = U("asc");
static const ::odata::utility::string_t KeywordDesc = U("desc");

static const ::odata::utility::string_t TokenNone = U("");
static const ::odata::utility::string_t TokenLeftParen = U("(");
static const ::odata::utility::string_t TokenRightParen = U(")");
static const ::odata::utility::string_t TokenComma = U(",");
static const ::odata::utility::string_t TokenEqual = U("=");
static const ::odata::utility::string_t TokenSlash = U("/");
static const ::odata::utility::string_t TokenStar = U("*");
static const ::odata::utility::string_t TokenQuestion = U("?");
static const ::odata::utility::string_t TokenMinus = U("-");
static const ::odata::utility::string_t TokenDot = U(".");
static const ::odata::utility::string_t TokenColon = U(":");

static const ::odata::utility::string_t TokenKindIdentifier = U("Identifier");
static const ::odata::utility::string_t TokenKindLiteral = U("Literal");
static const ::odata::utility::string_t TokenKindParameterAlias = U("ParameterAlias");

static const ::odata::utility::string_t CharKindDigit = U("Digit");
static const ::odata::utility::string_t CharKindHexDigit = U("HexDigit");

static const ::odata::utility::string_t DoubleLiteralInf = U("INF");
static const ::odata::utility::string_t DoubleLiteralNegativeInf = U("-INF");
static const ::odata::utility::string_t DoubleLiteralNan = U("NaN");
static const ::odata::utility::string_t LiteralIt = U("$it");

static const ::odata::utility::string_t LiteralPrefixDuration = U("duration");
static const ::odata::utility::string_t LiteralPrefixBinary = U("binary");
static const ::odata::utility::string_t LiteralPrefixGeography = U("geography");
static const ::odata::utility::string_t LiteralPrefixGeometry = U("geometry");

namespace odata { namespace core
{

// --BEGIN-- odata_path_parser

void odata_path_parser::validate_is_nonempty_relative_uri(const ::odata::utility::string_t& uri)
{
	if (uri.empty() || uri[0] != '/')
	{
		throw odata_exception(ERROR_MESSAGE_NOT_RELATIVE_URI());
	}
}

std::shared_ptr<::odata::core::odata_path> odata_path_parser::parse_path(const ::odata::utility::string_t& path)
{
	validate_is_nonempty_relative_uri(path);

	auto raw_segments = ::odata::utility::uri::split_path(path);

	bind_path(raw_segments);

	auto odata_path = std::make_shared<::odata::core::odata_path>(m_bound_segments);

	return odata_path;
}

void odata_path_parser::validate_can_bind_next_segment()
{
	auto segment = previous_segment();

	switch (segment->segment_type())
    {
	case ::odata::core::odata_path_segment_type::Metadata:
		throw odata_exception(ERROR_MESSAGE_SEGMENT_FOLLOW(SegmentIdentifierMetadata));

	case ::odata::core::odata_path_segment_type::Batch:
		throw odata_exception(ERROR_MESSAGE_SEGMENT_FOLLOW(SegmentIdentifierBatch));

	case ::odata::core::odata_path_segment_type::Value:
		throw odata_exception(ERROR_MESSAGE_SEGMENT_FOLLOW(SegmentIdentifierValue));

	case ::odata::core::odata_path_segment_type::Count:
		throw odata_exception(ERROR_MESSAGE_SEGMENT_FOLLOW(SegmentIdentifierCount));

	case ::odata::core::odata_path_segment_type::Ref:
		throw odata_exception(ERROR_MESSAGE_SEGMENT_FOLLOW(SegmentIdentifierRef));

	case ::odata::core::odata_path_segment_type::Operation:
		{
			auto operation = segment->as<odata_operation_segment>()->operation();
			if (!operation->is_composable())
			{
				throw odata_exception(ERROR_MESSAGE_SEGMENT_FOLLOW_NONCOMPOSABLE(operation->get_name()));
			}
		}
		break;

	case ::odata::core::odata_path_segment_type::OperationImport:
		{
			auto operation_import = segment->as<odata_operation_import_segment>()->operation_import();
			if (!operation_import->get_operation_type()->is_composable())
			{
				throw odata_exception(ERROR_MESSAGE_SEGMENT_FOLLOW_NONCOMPOSABLE(operation_import->get_name()));
			}
		}
		break;
	}
}

void odata_path_parser::bind_path(std::vector<::odata::utility::string_t> raw_segments)
{
	m_bound_segments.clear();

	bool should_be_no_segment = false;

	for (::size_t i = 0; i < raw_segments.size(); ++i)
	{
		if (i == 0)
		{
			bind_first_segment(raw_segments[0]);
		}
		else
		{
			validate_can_bind_next_segment();
			bind_next_segment(raw_segments[i]);
		}
        
        compute_target_navigation_source_and_type();
	}
}

void odata_path_parser::extract_segment_identifier_and_parenthesis_exp(const ::odata::utility::string_t& segment_str, ::odata::utility::string_t& identifier, ::odata::utility::string_t& parenthesis_exp)
{
	auto parenthesis_start = segment_str.find('(');
	if (parenthesis_start == ::odata::utility::string_t::npos)
	{
		// If the segment string does not contain '(', identifier is the string itself and parenthesis expression is empty.
		identifier = segment_str;
		parenthesis_exp.clear();
	}
	else
	{
		// At least we have one char '(' in the string.
		auto parenthesis_end = segment_str.size() - 1;
		if (segment_str[parenthesis_end] == ')')
		{
			if (parenthesis_start < 1)
			{
				throw odata_exception(ERROR_MESSAGE_EMPTY_SEGMENT_IDENTIFIER());
			}

			identifier = segment_str.substr(0, parenthesis_start);
			parenthesis_exp = segment_str.substr(parenthesis_start + 1, parenthesis_end - parenthesis_start - 1);
		}
		else
		{
			throw odata_exception(ERROR_MESSAGE_PARENTHESIS_MISMATCH());
		}
	}
}

void odata_path_parser::bind_first_segment(const ::odata::utility::string_t &segment_str)
{
	::odata::utility::string_t identifier;
	::odata::utility::string_t parenthesis_exp;
	extract_segment_identifier_and_parenthesis_exp(segment_str, identifier, parenthesis_exp);

	if (identifier == SegmentIdentifierMetadata)
	{
		if (!parenthesis_exp.empty())
		{
			throw odata_exception(ERROR_MESSAGE_UNEXPECTED_PARENTHESIS_EXPRESSION(identifier));
		}

		m_bound_segments.push_back(odata_path_segment::create_metadata_segment());

		return;
	}
	else if (identifier == SegmentIdentifierBatch)
	{
		if (!parenthesis_exp.empty())
		{
			throw odata_exception(ERROR_MESSAGE_UNEXPECTED_PARENTHESIS_EXPRESSION(identifier));
		}

		m_bound_segments.push_back(odata_path_segment::create_batch_segment());

		return;
	}
	else if (identifier == SegmentIdentifierCount || identifier == SegmentIdentifierValue || identifier == SegmentIdentifierRef)
	{
		throw odata_exception(ERROR_MESSAGE_UNEXPECTED_PATH_ROOT(identifier));
	}
	else if (identifier == SegmentIdentifierAll || identifier == SegmentIdentifierEntity || identifier == SegmentIdentifierCrossJoin)
	{
		throw odata_exception(ERROR_MESSAGE_UNSUPPORTED_SEGMENT(identifier));
	}
	else if (try_bind_as_navigation_source(identifier, parenthesis_exp))
	{
		return;
	}
	else if (try_bind_as_operation_import(identifier, parenthesis_exp))
	{
		return;
	}
	
	throw odata_exception(ERROR_MESSAGE_RESOURCE_NOT_FOUND(identifier));
}

void odata_path_parser::bind_next_segment(const ::odata::utility::string_t &segment_str)
{
    ::odata::utility::string_t identifier;
    ::odata::utility::string_t parenthesis_exp;
    extract_segment_identifier_and_parenthesis_exp(segment_str, identifier, parenthesis_exp);
    
    if (try_bind_as_value(identifier, parenthesis_exp))
    {
        return;
    }

	if (try_bind_as_count(identifier, parenthesis_exp))
    {
        return;
    }

	if (try_bind_as_ref(identifier, parenthesis_exp))
    {
        return;
    }
    
    if (try_bind_as_declared_property(identifier, parenthesis_exp))
    {
        return;
    }
    
    if (try_bind_as_type(identifier, parenthesis_exp))
    {
        return;
    }
    
    if (try_bind_as_operation(identifier, parenthesis_exp))
    {
        return;
    }
    
    bind_as_dynamic_property(identifier, parenthesis_exp);
}

bool odata_path_parser::try_bind_as_navigation_source(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &key_exp)
{
	auto container = model()->find_container();
	if (container == nullptr)
	{
		return false;
	}

	auto entity_set = container->find_entity_set(identifier);
	if (entity_set != nullptr)
	{
		auto entity_set_segment = odata_path_segment::create_entity_set_segment(entity_set);
		m_bound_segments.push_back(entity_set_segment);
        
        bind_as_key(key_exp);
        
		return true;
	}
	else
	{
		auto singleton = container->find_singleton(identifier);
		if (singleton != nullptr)
		{
			auto singleton_segment = odata_path_segment::create_singleton_segment(singleton);
			m_bound_segments.push_back(singleton_segment);

			if (!key_exp.empty())
			{
				throw odata_exception(ERROR_MESSAGE_UNEXPECTED_PARENTHESIS_EXPRESSION(identifier));
			}
            
			return true;
		}
	}

	return false;
}

std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> odata_path_parser::parse_named_values(const ::odata::utility::string_t &exp)
{
	std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> result;

	// Indicates if there is a pair has its name missing.
	bool has_name_missing = false;
	::size_t num_pairs_parsed = 0;

	auto lexer = odata_expression_lexer::create_lexer(exp);
	while (lexer->current_token()->token_kind() != odata_expression_token_kind::End)
	{
		::odata::utility::string_t name;
		std::shared_ptr<::odata::core::odata_primitive_value> value;

		if (lexer->current_token()->token_kind() == odata_expression_token_kind::Identifier)
		{
			name = lexer->current_token()->text();

			lexer->next_token();
			lexer->eat_token(odata_expression_token_kind::Equal, TokenEqual);

			value = lexer->current_token()->to_primitive_value();
		}
		else if (lexer->current_token()->is_literal())
		{
			has_name_missing = true;

			value = lexer->current_token()->to_primitive_value();
		}
		else
		{
			throw odata_exception(ERROR_MESSAGE_UNEXPECTED_TOKEN(lexer->current_token()->text()));
		}

		if (value == nullptr)
		{
			throw odata_exception(ERROR_MESSAGE_INVALID_VALUE(lexer->current_token()->text()));
		}

		auto pair = std::make_pair(name, value);

		++num_pairs_parsed;

		auto existing = std::find_if(result.begin(), result.end(), [&pair](std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>> p)
		{
			return pair.first == p.first;
		});

		if (!pair.first.empty()	&& existing != result.end())
		{
			throw odata_exception(ERROR_MESSAGE_NAME_MUST_NOT_DUPLICATE(pair.first));
		}

		result.push_back(pair);

		lexer->next_token();
		if (lexer->current_token()->token_kind() == odata_expression_token_kind::Comma)
		{
			lexer->next_token();

			if (lexer->current_token()->token_kind() == odata_expression_token_kind::End)
			{
				throw odata_exception(ERROR_MESSAGE_EMPTY_NAME_VALUE_PAIR());
			}
		}
	}

	if (has_name_missing && num_pairs_parsed > 1)
	{
		// Multiple keys must have name provided for each one.
		throw odata_exception(ERROR_MESSAGE_NAME_MUST_BE_PROVIDED());
	}

	return result;
}

std::shared_ptr<::odata::edm::edm_named_type> odata_path_parser::get_target_type()
{
	auto segment = previous_segment();

    switch (segment->segment_type())
    {
	case ::odata::core::odata_path_segment_type::EntitySet:
		return std::make_shared<::odata::edm::edm_collection_type>(segment->as<odata_entity_set_segment>()->entity_type());
    
	case ::odata::core::odata_path_segment_type::Singleton:
        return segment->as<odata_singleton_segment>()->entity_type();
    
	case ::odata::core::odata_path_segment_type::StructuralProperty:
        return segment->as<odata_structural_property_segment>()->property()->get_property_type();
        
	case ::odata::core::odata_path_segment_type::NavigationProperty:
        return segment->as<odata_navigation_property_segment>()->navigation_type()->get_navigation_type();

	case ::odata::core::odata_path_segment_type::Operation:
		return segment->as<odata_operation_segment>()->operation()->get_operation_return_type();

	case ::odata::core::odata_path_segment_type::OperationImport:
		return segment->as<odata_operation_import_segment>()->operation_import()->get_operation_type()->get_operation_return_type();
        
    case ::odata::core::odata_path_segment_type::Type:
        return segment->as<odata_type_segment>()->type();

	case ::odata::core::odata_path_segment_type::Key:
		return segment->as<odata_key_segment>()->target_entity_type();

    default:
        // Cannot determine the target type.
		return ::odata::edm::edm_named_type::EDM_UNKNOWN();
    }
}

std::shared_ptr<::odata::edm::edm_navigation_source> odata_path_parser::get_target_navigation_source()
{
	auto segment = previous_segment();

    switch (segment->segment_type())
    {
	case ::odata::core::odata_path_segment_type::EntitySet:
        return segment->as<odata_entity_set_segment>()->entity_set();
    
	case ::odata::core::odata_path_segment_type::Singleton:
        return segment->as<odata_singleton_segment>()->singleton();

	case ::odata::core::odata_path_segment_type::NavigationProperty:
        return segment->as<odata_navigation_property_segment>()->navigation_type()->get_binded_navigation_source();

	case ::odata::core::odata_path_segment_type::Key:
		return segment->as<odata_key_segment>()->navigation_source();
    
    default:
        // Cannot determine the target navigation source.
        return nullptr;
    }
}

void odata_path_parser::bind_as_key(const ::odata::utility::string_t &key_exp)
{
    if (key_exp.empty())
    {
        // Skip binding if key expression is empty.
        return;
    }
    
	compute_target_navigation_source_and_type();
	auto target_entity_type = ::odata::edm::edm_model_utility::get_entity_type(target_type());
    
    auto keys = parse_named_values(key_exp);
	auto keys_from_model = target_entity_type->get_key_with_parents();
	if (keys_from_model.size() != keys.size())
	{
		throw odata_exception(ERROR_MESSAGE_KEY_COUNT_MISMATCH());
	}

	for (auto it = keys_from_model.begin(); it != keys_from_model.end(); ++it)
	{
		auto existing = std::find_if(keys.begin(), keys.end(), [it](std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>> p)
		{
			return *it == p.first;
		});

		// Name can be omitted when there is only one key.
		if (existing == keys.end() && !(keys.size() == 1 && keys[0].first.empty()))
		{
			throw odata_exception(ERROR_MESSAGE_KEY_NOT_FOUND(*it));
		}
	}

	if (keys[0].first.empty())
	{
		// Even if the name can be omitted when only one key but we need to fill the name for service to use.
		keys[0] = std::make_pair(keys_from_model[0], keys[0].second);
	}
    
	m_bound_segments.push_back(odata_path_segment::create_key_segment(target_navigation_source(), target_entity_type, std::move(keys)));
}
    
bool odata_path_parser::try_bind_as_value(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &empty_exp)
{
    if (identifier != SegmentIdentifierValue)
    {
        return false;
    }
    
    if (!empty_exp.empty())
    {
        throw odata_exception(ERROR_MESSAGE_UNEXPECTED_PARENTHESIS_EXPRESSION(identifier));
    }
    
    m_bound_segments.push_back(odata_path_segment::create_value_segment());
    
    return true;
}

bool odata_path_parser::try_bind_as_count(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &empty_exp)
{
    if (identifier != SegmentIdentifierCount)
    {
        return false;
    }
    
    if (!empty_exp.empty())
    {
        throw odata_exception(ERROR_MESSAGE_UNEXPECTED_PARENTHESIS_EXPRESSION(identifier));
    }
    
    m_bound_segments.push_back(odata_path_segment::create_count_segment());
    
    return true;
}

bool odata_path_parser::try_bind_as_ref(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &empty_exp)
{
    if (identifier != SegmentIdentifierRef)
    {
        return false;
    }
    
    if (!empty_exp.empty())
    {
        throw odata_exception(ERROR_MESSAGE_UNEXPECTED_PARENTHESIS_EXPRESSION(identifier));
    }
    
    m_bound_segments.push_back(odata_path_segment::create_ref_segment());
    
    return true;
}

bool odata_path_parser::try_bind_as_declared_property(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &key_exp)
{
	auto owning_type = ::odata::edm::edm_model_utility::get_structured_type(target_type());
	if (owning_type == nullptr)
	{
		// Not structured type.
		return false;
	}

    auto property = owning_type->find_property(identifier);
    if (property == nullptr)
    {
        // Property not declared.
        return false;
    }
    
    if (property->get_property_type()->get_type_kind() == ::odata::edm::edm_type_kind_t::Navigation)
    {
        // Navigation property should have binded navigation source.
        auto navigation_type = std::static_pointer_cast<::odata::edm::edm_navigation_type>(property->get_property_type());
		if (navigation_type->get_binded_navigation_source() == nullptr && !navigation_type->is_contained())
		{
			throw odata_exception(ERROR_MESSAGE_NO_NAVIGATION_SOURCE());
		}

		// Single navigation property cannot have key expression.
		if (!::odata::edm::edm_model_utility::is_collection_of_entity(navigation_type->get_navigation_type()) && !key_exp.empty())
		{
			throw odata_exception(ERROR_MESSAGE_UNEXPECTED_PARENTHESIS_EXPRESSION(identifier));
		}

        m_bound_segments.push_back(odata_path_segment::create_navigation_property_segment(owning_type, property, navigation_type));

        bind_as_key(key_exp);
    }
    else
    {
        // Structural property.
        m_bound_segments.push_back(odata_path_segment::create_structural_property_segment(owning_type, property));
        
        // Structural property cannot have key expression.
        if (!key_exp.empty())
        {
            throw odata_exception(ERROR_MESSAGE_UNEXPECTED_PARENTHESIS_EXPRESSION(identifier));
        }
    }
    
    return true;
}

bool odata_path_parser::try_bind_as_type(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &empty_exp)
{
    std::shared_ptr<::odata::edm::edm_named_type> type = model()->find_entity_type(identifier);
    if (type == nullptr)
    {
        type = model()->find_complex_type(identifier);
        
        if (type == nullptr)
        {
            // Not a type segment.
            return false;
        }
    }
    
    if (!empty_exp.empty())
    {
        throw odata_exception(ERROR_MESSAGE_UNEXPECTED_PARENTHESIS_EXPRESSION(identifier));
    }
    
    m_bound_segments.push_back(odata_path_segment::create_type_segment(type));
    
    return true;
}

void odata_path_parser::validate_parameters_match(
	const std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> &parameters_parsed,
	const std::vector<std::shared_ptr<::odata::edm::edm_operation_parameter>> &parameters_from_model,
	bool has_binding_parameter)
{
	::size_t skip = has_binding_parameter ? 1 : 0;

	if (parameters_from_model.size() != parameters_parsed.size() + skip)
	{
		throw odata_exception(ERROR_MESSAGE_PARAMETER_COUNT_MISMATCH());
	}

	for (::size_t i = skip; i < parameters_from_model.size(); ++i)
	{
		auto param_name = parameters_from_model[i]->get_param_name();

		auto existing = std::find_if(parameters_parsed.begin(), parameters_parsed.end(), [&param_name](std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>> p)
		{
			return param_name == p.first;
		});

		if (existing == parameters_parsed.end())
		{
			throw odata_exception(ERROR_MESSAGE_PARAMETER_NOT_FOUND(param_name));
		}
	}
}

bool odata_path_parser::try_bind_as_operation_import(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &parameters_exp)
{
	auto container = model()->find_container();
	if (container == nullptr)
	{
		return false;
	}

	auto operation_import = container->find_operation_import(identifier);
	if (operation_import == nullptr)
	{
		// Operation import not found.
		return false;
	}

	auto operation = operation_import->get_operation_type();
	if (operation->is_bound())
	{
		// Operation import used as path root cannot be bound.
		throw odata_exception(ERROR_MESSAGE_BOUND_OPERATION_IMPORT_FOUND(identifier));
	}

	auto parameters = parse_named_values(parameters_exp);
	auto parameters_from_model = operation->get_operation_parameters();
	validate_parameters_match(parameters, parameters_from_model);

	m_bound_segments.push_back(odata_path_segment::create_operation_import_segment(operation_import, std::move(parameters)));

	return true;
}

bool odata_path_parser::try_bind_as_operation(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &parameters_exp)
{
	auto operation = model()->find_operation_type(identifier);
	if (operation == nullptr)
	{
		// Operation not found.
		return false;
	}

	if (!operation->is_bound())
	{
		// Operation used in path cannot be unbound.
		throw odata_exception(ERROR_MESSAGE_UNBOUND_OPERATION_FOUND(identifier));
	}

	auto parameters = parse_named_values(parameters_exp);
	auto parameters_from_model = operation->get_operation_parameters();

	// Validate the binding parameter.
	if (parameters_from_model.size() < 1)
	{
		throw odata_exception(ERROR_MESSAGE_BINDING_PARAMETER_NOT_FOUND(identifier));
	}

	auto binding_type = parameters_from_model[0]->get_param_type();
	auto previous_type = target_type();
	if (previous_type == nullptr || binding_type->get_full_name() != previous_type->get_full_name())
	{
		// Should consider derived type comparison.
		throw odata_exception(ERROR_MESSAGE_BINDING_TYPE_MISMATCH(identifier));
	}

	// Validate the remaining parameters.
	validate_parameters_match(parameters, parameters_from_model, /*has_binding_parameter*/true);

	m_bound_segments.push_back(odata_path_segment::create_operation_segment(operation, std::move(parameters)));
    
	return true;
}

void odata_path_parser::bind_as_dynamic_property(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &empty_exp)
{
	if (previous_segment()->segment_type() != ::odata::core::odata_path_segment_type::DynamicProperty)
	{
		auto target_structured_type = ::odata::edm::edm_model_utility::get_structured_type(target_type());
		if (target_structured_type == nullptr || !target_structured_type->is_open_type())
		{
			throw odata_exception(ERROR_MESSAGE_NOT_OPEN_TYPE(identifier));
		}

		if (!empty_exp.empty())
		{
			// Dynamic navigation property not supported on open type.
			throw odata_exception(ERROR_MESSAGE_UNEXPECTED_PARENTHESIS_EXPRESSION(identifier));
		}
	}

	m_bound_segments.push_back(odata_path_segment::create_dynamic_property_segment(identifier));
}

void odata_path_parser::compute_target_navigation_source_and_type()
{
    auto new_navigation_source = get_target_navigation_source();
    if (new_navigation_source != nullptr)
    {
        m_target_navigation_source = new_navigation_source;
    }
    
    auto new_type = get_target_type();
    if (new_type != nullptr)
    {
        m_target_type = new_type;
    }
}

// --END-- odata_path_parser

// --BEGIN-- odata_select_item, odata_expand_item

std::shared_ptr<::odata::core::odata_select_item> odata_select_item::create(std::shared_ptr<::odata::core::odata_query_node> end)
{
	return std::make_shared<::odata::core::odata_select_item>(end);
}

std::shared_ptr<::odata::core::odata_expand_item> odata_expand_item::create(std::shared_ptr<::odata::core::odata_query_node> end)
{
	return create(
		end,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		null_value,
		null_value,
		null_value);
}

std::shared_ptr<::odata::core::odata_expand_item> odata_expand_item::create(
	std::shared_ptr<::odata::core::odata_query_node> end,
	std::shared_ptr<::odata::core::odata_select_expand_clause> select_expand_clause,
    std::shared_ptr<::odata::core::odata_filter_clause> filter_clause,
    std::shared_ptr<::odata::core::odata_orderby_clause> orderby_clause,
    std::shared_ptr<::odata::core::odata_search_clause> search_clause,
    ::odata::common::nullable<int64_t> top,
    ::odata::common::nullable<int64_t> skip,
    ::odata::common::nullable<bool> count)
{
	return std::shared_ptr<::odata::core::odata_expand_item>(
		new odata_expand_item(
		end,
		select_expand_clause,
		filter_clause,
		orderby_clause,
		search_clause,
		top,
		skip,
		count));
}

// --END-- odata_select_item, odata_expand_item

// --BEGIN-- odata_query_option_parser

std::shared_ptr<::odata::core::odata_select_expand_clause> odata_query_option_parser::parse_select_and_expand(const ::odata::utility::string_t& select_query, const ::odata::utility::string_t& expand_query)
{
    if (select_query.empty() && expand_query.empty())
    {
        return null_value;
    }
    
	// Should be EntityType, ComplexType, Collection(EntityType) or Collection(ComplexType).
	if (::odata::edm::edm_model_utility::get_structured_type(target_type()) == nullptr)
    {
        throw odata_exception(ERROR_MESSAGE_SELECT_OR_EXPAND_ON_NONSTRUCTURED_TYPE());
    }
    
    auto select_items = parse_select(select_query);
    auto expand_items = parse_expand(expand_query);
    
	auto select_expand_clause = std::make_shared<::odata::core::odata_select_expand_clause>(
        std::move(select_items),
        std::move(expand_items));
	return select_expand_clause;
}

std::shared_ptr<::odata::core::odata_filter_clause> odata_query_option_parser::parse_filter(
    const ::odata::utility::string_t& filter_query)
{
    if (filter_query.empty())
    {
        return null_value;
    }

	if (target_type()->get_type_kind() != ::odata::edm::edm_type_kind_t::Collection)
	{
		throw odata_exception(ERROR_MESSAGE_FILTER_ON_NONCOLLECTION());
	}
    
	auto expression = odata_expression_parser::parse_expression(filter_query, odata_expression_parser_subject::FilterClause);
	auto range_variable = create_implicit_range_variable();
    
	auto filter_clause = std::make_shared<odata_filter_clause>(expression, range_variable);
	return filter_clause;
}

std::shared_ptr<::odata::core::odata_orderby_clause> odata_query_option_parser::parse_orderby(const ::odata::utility::string_t& orderby_query)
{
    if (orderby_query.empty())
    {
        return null_value;
    }

	if (target_type()->get_type_kind() != ::odata::edm::edm_type_kind_t::Collection)
	{
		throw odata_exception(ERROR_MESSAGE_ORDERBY_ON_NONCOLLECTION());
	}

	auto lexer = odata_expression_lexer::create_lexer(orderby_query);

	std::vector<std::pair<std::shared_ptr<::odata::core::odata_query_node>, bool>> items;

	while (lexer->current_token()->token_kind() != odata_expression_token_kind::End)
	{
		auto expression = odata_expression_parser::parse_expression(lexer, odata_expression_parser_subject::OrderByClause);

		bool ascending = true;

		if (lexer->current_token()->is_identifier(KeywordDesc))
		{
			lexer->next_token();

			ascending = false;
		}

		items.push_back(std::make_pair(expression, ascending));

		if (lexer->current_token()->token_kind() != odata_expression_token_kind::Comma)
		{
			break;
		}

		// Skip comma.
		lexer->next_token();
	}

	auto range_variable = create_implicit_range_variable();
    
	auto orderby_clause = std::make_shared<::odata::core::odata_orderby_clause>(std::move(items), range_variable);
	return orderby_clause;
}

std::shared_ptr<::odata::core::odata_search_clause> odata_query_option_parser::parse_search(const ::odata::utility::string_t& search_query)
{
    if (search_query.empty())
    {
        return null_value;
    }

	auto expression = odata_expression_parser::parse_expression(search_query, odata_expression_parser_subject::SearchClause);
    
	auto search_clause = std::make_shared<::odata::core::odata_search_clause>(expression);
	return search_clause;
}

::odata::common::nullable<int64_t> odata_query_option_parser::parse_top(const ::odata::utility::string_t& top_query)
{
	if (top_query.empty())
	{
		return null_value;
	}

	return ::odata::utility::conversions::scan_string<int64_t>(top_query);
}

::odata::common::nullable<int64_t> odata_query_option_parser::parse_skip(const ::odata::utility::string_t& skip_query)
{
	if (skip_query.empty())
	{
		return null_value;
	}

	return ::odata::utility::conversions::scan_string<int64_t>(skip_query);
}

::odata::common::nullable<bool> odata_query_option_parser::parse_count(const ::odata::utility::string_t& count_query)
{
	if (count_query.empty())
	{
		return null_value;
	}
	else if (count_query == KeywordTrue)
	{
		return true;
	}
	else if (count_query == KeywordFalse)
	{
		return false;
	}
	else
	{
		throw odata_exception(ERROR_MESSAGE_INVALID_COUNT_QUERY(count_query));
	}
}

std::shared_ptr<odata_range_variable> odata_query_option_parser::create_implicit_range_variable() const
{
    auto _target_type = ::odata::edm::edm_model_utility::get_collection_element_type(target_type());
    if (_target_type->get_type_kind() == ::odata::edm::edm_type_kind_t::Entity)
    {
        return std::make_shared<odata_range_variable>(LiteralIt, _target_type, target_navigation_source());
    }
    else
    {
        return std::make_shared<odata_range_variable>(LiteralIt, _target_type);
    }
}

std::vector<std::shared_ptr<::odata::core::odata_select_item>> odata_query_option_parser::parse_select(
    const ::odata::utility::string_t& select_query)
{
    auto lexer = odata_expression_lexer::create_lexer(select_query);

	std::vector<std::shared_ptr<::odata::core::odata_select_item>> items;

	while (lexer->current_token()->token_kind() != odata_expression_token_kind::End)
	{
		items.push_back(odata_select_item::create(
			odata_expression_parser::parse_expression(lexer, odata_expression_parser_subject::SelectClause)));

		if (lexer->current_token()->token_kind() != odata_expression_token_kind::Comma)
		{
			break;
		}

		// Skip comma.
		lexer->next_token();
	}
    
    return std::move(items);
}

std::vector<std::shared_ptr<::odata::core::odata_expand_item>> odata_query_option_parser::parse_expand(
    const ::odata::utility::string_t& expand_query)
{
    auto lexer = odata_expression_lexer::create_lexer(expand_query);

	std::vector<std::shared_ptr<::odata::core::odata_expand_item>> items;

	while (lexer->current_token()->token_kind() != odata_expression_token_kind::End)
	{
		auto expression = odata_expression_parser::parse_expression(lexer, odata_expression_parser_subject::ExpandClause);
        
        if (lexer->current_token()->token_kind() == odata_expression_token_kind::LeftParen)
        {
            auto expand_options_text = lexer->advance_through_balanced_parenthesis();
			auto query_options = odata_uri_parser::split_query_options(expand_options_text, true);

			::odata::utility::string_t select_query;
			::odata::utility::string_t expand_query;
			::odata::utility::string_t filter_query;
			::odata::utility::string_t orderby_query;
			::odata::utility::string_t search_query;
			::odata::utility::string_t top_query;
			::odata::utility::string_t skip_query;
			::odata::utility::string_t count_query;

			for (auto iter = query_options.begin(); iter != query_options.end(); ++iter)
			{
				auto &option = *iter;

				if (option.first == QueryOptionSelect)
				{
					select_query = option.second;
				}
				else if (option.first == QueryOptionExpand)
				{
					expand_query = option.second;
				}
				else if (option.first == QueryOptionFilter)
				{
					filter_query = option.second;
				}
				else if (option.first == QueryOptionOrderBy)
				{
					orderby_query = option.second;
				}
				else if (option.first == QueryOptionSearch)
				{
					search_query = option.second;
				}
				else if (option.first == QueryOptionTop)
				{
					top_query = option.second;
				}
				else if (option.first == QueryOptionSkip)
				{
					skip_query = option.second;
				}
				else if (option.first == QueryOptionCount)
				{
					count_query = option.second;
				}
			}

			items.push_back(odata_expand_item::create(
				expression,
				parse_select_and_expand(select_query, expand_query),
				parse_filter(filter_query),
				parse_orderby(orderby_query),
				parse_search(search_query),
				parse_top(top_query),
				parse_skip(skip_query),
				parse_count(count_query)));
        }
		else
		{
			items.push_back(odata_expand_item::create(expression));
		}
        
		if (lexer->current_token()->token_kind() != odata_expression_token_kind::Comma)
		{
			break;
		}

		// Skip comma.
		lexer->next_token();
	}
    
    return std::move(items);
}

// --END-- odata_query_option_parser

// --BEGIN-- odata_expression_lexer

std::shared_ptr<odata_expression_token> odata_expression_token::create_token(int token_kind, const ::odata::utility::string_t &text, ::size_t pos)
{
	return std::make_shared<odata_expression_token>(token_kind, text, pos);
}

bool odata_expression_token::is_literal() const
{
	switch (m_token_kind)
	{
	case odata_expression_token_kind::BinaryLiteral:
	case odata_expression_token_kind::BooleanLiteral:
	case odata_expression_token_kind::DateTimeOffsetLiteral:
	case odata_expression_token_kind::DecimalLiteral:
	case odata_expression_token_kind::DoubleLiteral:
	case odata_expression_token_kind::DurationLiteral:
	case odata_expression_token_kind::GuidLiteral:
	case odata_expression_token_kind::Int64Literal:
	case odata_expression_token_kind::Int32Literal:
	case odata_expression_token_kind::NullLiteral:
	case odata_expression_token_kind::QuotedLiteral:
	case odata_expression_token_kind::SingleLiteral:
	case odata_expression_token_kind::StringLiteral:
		return true;

	default:
		return false;
	}
}

bool odata_expression_token::is_identifier() const
{
	return m_token_kind == odata_expression_token_kind::Identifier;
}

bool odata_expression_token::is_identifier(const ::odata::utility::string_t &text) const
{
	return is_identifier() && m_text == text;
}

std::shared_ptr<::odata::core::odata_primitive_value> odata_expression_token::to_primitive_value() const
{
	switch (m_token_kind)
	{
	case odata_expression_token_kind::BooleanLiteral:
		return std::make_shared<odata_primitive_value>(::odata::edm::edm_primitive_type::BOOLEAN(), m_text);

	case odata_expression_token_kind::DoubleLiteral:
		return std::make_shared<odata_primitive_value>(::odata::edm::edm_primitive_type::DOUBLE(), m_text);

	case odata_expression_token_kind::Int32Literal:
		return std::make_shared<odata_primitive_value>(::odata::edm::edm_primitive_type::INT32(), m_text);

	case odata_expression_token_kind::StringLiteral:
		if (m_text.size() > 1)
		{
			auto string_rep = m_text.substr(1, m_text.size() - 2);
			return std::make_shared<odata_primitive_value>(::odata::edm::edm_primitive_type::STRING(), string_rep);
		}
		else
		{
			return std::make_shared<odata_primitive_value>(::odata::edm::edm_primitive_type::STRING(), m_text);
		}

	default:
		return nullptr;
	}
}

std::shared_ptr<odata_expression_lexer> odata_expression_lexer::create_lexer(const ::odata::utility::string_t &expression)
{
	return std::make_shared<odata_expression_lexer>(expression);
}

void odata_expression_lexer::next_token()
{
	scan_whitespaces();

	auto token_start = m_pos;
	switch (current_char())
	{
	case U('('):
		next_char();
		m_token = odata_expression_token::create_token(odata_expression_token_kind::LeftParen, TokenLeftParen, token_start);
		return;

	case U(')'):
		next_char();
		m_token = odata_expression_token::create_token(odata_expression_token_kind::RightParen, TokenRightParen, token_start);
		return;

	case U(','):
		next_char();
		m_token = odata_expression_token::create_token(odata_expression_token_kind::Comma, TokenComma, token_start);
		return;

	case U('='):
		next_char();
		m_token = odata_expression_token::create_token(odata_expression_token_kind::Equal, TokenEqual, token_start);
		return;

	case U('/'):
		next_char();
		m_token = odata_expression_token::create_token(odata_expression_token_kind::Slash, TokenSlash, token_start);
		return;

	case U('.'):
		next_char();
		m_token = odata_expression_token::create_token(odata_expression_token_kind::Dot, TokenDot, token_start);
		return;

	case U('*'):
		next_char();
		m_token = odata_expression_token::create_token(odata_expression_token_kind::Star, TokenStar, token_start);
		return;
	
	case U(':'):
		next_char();
		m_token = odata_expression_token::create_token(odata_expression_token_kind::Colon, TokenColon, token_start);
		return;

	case U('-'):
		if (::odata::utility::is_digit(peek_char()))
		{
			auto kind = scan_number();
			if (kind == odata_expression_token_kind::GuidLiteral)
			{
				// Guid literals cannot have sign so rewind.
				m_pos = token_start;
			}
			else
			{
				m_token = odata_expression_token::create_token(kind, extract_text(token_start), token_start);
				return;
			}
		}
		else if (peek_char() == U('I') || peek_char() == U('N'))
		{
			// Advance to 'I' or 'N'
			next_char();

			scan_identifier();

			auto text = extract_text(token_start);

			if (text == DoubleLiteralNegativeInf)
			{
				m_token = odata_expression_token::create_token(odata_expression_token_kind::DoubleLiteral, text, token_start);
				return;
			}

			m_pos = token_start;
		}

		next_char();
		m_token = odata_expression_token::create_token(odata_expression_token_kind::Minus, TokenMinus, token_start);
		return;

	case U('\''):
		scan_string();
		m_token = odata_expression_token::create_token(odata_expression_token_kind::StringLiteral, extract_text(token_start), token_start);
		return;

	case U('@'):
		next_char();
		scan_identifier();
		m_token = odata_expression_token::create_token(odata_expression_token_kind::ParameterAlias, extract_text(token_start), token_start);
		return;

	case U('{'):
	case U('['):
		scan_json_object_or_array();
		m_token = odata_expression_token::create_token(odata_expression_token_kind::JsonObjectOrArray, extract_text(token_start), token_start);
		return;

	case 0:
		m_token = odata_expression_token::create_token(odata_expression_token_kind::End, TokenNone, m_expression.size());
		return;
	}

	if (is_valid_start_of_identifier())
	{
		scan_identifier();

		if (current_char() == U('-'))
		{
			if (try_scan_guid(token_start))
			{
				m_token = odata_expression_token::create_token(odata_expression_token_kind::GuidLiteral, extract_text(token_start), token_start);
				return;
			}
		}

		auto text = extract_text(token_start);

		if (text == DoubleLiteralInf || text == DoubleLiteralNan)
		{
			m_token = odata_expression_token::create_token(odata_expression_token_kind::DoubleLiteral, text, token_start);
			return;
		}

		if (text == KeywordTrue || text == KeywordFalse)
		{
			m_token = odata_expression_token::create_token(odata_expression_token_kind::BooleanLiteral, text, token_start);
			return;
		}

		if (text == KeywordNull)
		{
			m_token = odata_expression_token::create_token(odata_expression_token_kind::NullLiteral, text, token_start);
			return;
		}

		auto kind = try_scan_quoted_literal(token_start);
		if (kind != odata_expression_token_kind::None)
		{
			m_token = odata_expression_token::create_token(kind, extract_text(token_start), token_start);
			return;
		}

		m_token = odata_expression_token::create_token(odata_expression_token_kind::Identifier, text, token_start);
		return;
	}

	if (::odata::utility::is_digit(current_char()))
	{
		auto kind = scan_number();
		m_token = odata_expression_token::create_token(kind, extract_text(token_start), token_start);
		return;
	}

	throw odata_exception(ERROR_MESSAGE_INVALID_CHARACTER(current_char()));
}

std::shared_ptr<odata_expression_token> odata_expression_lexer::peek_token()
{
	auto orig_pos = m_pos;
	auto orig_token = m_token;

	next_token();

	auto next = current_token();

	m_pos = orig_pos;
	m_token = orig_token;

	return next;
}

std::shared_ptr<odata_expression_token> odata_expression_lexer::current_token()
{
	if (m_token == nullptr)
	{
		next_token();
	}

	return m_token;
}

std::shared_ptr<odata_expression_token> odata_expression_lexer::eat_token(int expected_kind, const ::odata::utility::string_t &_expected_token)
{
    auto token = current_token();
    
	if (current_token()->token_kind() != expected_kind)
	{
		throw odata_exception(ERROR_MESSAGE_TOKEN_KIND_EXPECTED(_expected_token));
	}

	next_token();
    
    return token;
}

::odata::utility::string_t odata_expression_lexer::eat_identifier()
{
    auto token = current_token();
    
    if (!token->is_identifier())
    {
        throw odata_exception(ERROR_MESSAGE_TOKEN_KIND_EXPECTED(TokenKindIdentifier));
    }
    
    next_token();
    
    return token->text();
}

::odata::utility::string_t odata_expression_lexer::eat_dotted_identifier()
{
    auto identifier = eat_identifier();
    
    while (current_token()->token_kind() == odata_expression_token_kind::Dot)
    {
        next_token();
        
        identifier += TokenDot + eat_identifier();
    }
    
    return identifier;
}

std::shared_ptr<::odata::core::odata_primitive_value> odata_expression_lexer::eat_literal()
{
    auto token = current_token();
    
    if (!token->is_literal())
    {
        throw odata_exception(ERROR_MESSAGE_TOKEN_KIND_EXPECTED(TokenKindLiteral));
    }
    
    next_token();
    
    return token->to_primitive_value();
}

::odata::utility::string_t odata_expression_lexer::advance_through_balanced_parenthesis()
{
	eat_token(odata_expression_token_kind::LeftParen, TokenLeftParen);

	::odata::utility::string_t text;
	auto start = m_pos;

	::size_t depth = 1;
	while (depth > 0)
	{
		if (current_char() == U('\''))
		{
			scan_string();
		}

		if (current_char() == U('('))
		{
			++depth;
		}
		else if (current_char() == U(')'))
		{
			if (depth == 0)
			{
				throw odata_exception(ERROR_MESSAGE_PARENTHESIS_MISMATCH());
			}

			--depth;

			if (depth == 0)
			{
				text = extract_text(start);
			}
		}
		else if (current_char() == 0)
		{
			throw odata_exception(ERROR_MESSAGE_PARENTHESIS_MISMATCH());
		}

		next_char();
	}

	next_token();

	return std::move(text);
}

void odata_expression_lexer::next_char()
{
	::size_t new_pos = m_pos + 1;

	if (new_pos <= m_expression.size())
	{
		m_pos = new_pos;
	}
}

::odata::utility::char_t odata_expression_lexer::eat_char(std::function<bool (::odata::utility::char_t)> validator, const ::odata::utility::string_t &_expected_char)
{
	auto _current_char = current_char();
	if (!validator(_current_char))
	{
		throw odata_exception(ERROR_MESSAGE_CHAR_KIND_EXPECTED(_expected_char));
	}

	next_char();

	return _current_char;
}

void odata_expression_lexer::scan_whitespaces()
{
	while (current_char() == U(' '))
	{
		next_char();
	}
}

::size_t odata_expression_lexer::scan_digits()
{
	::size_t count = 0;

	while (::odata::utility::is_digit(current_char()))
	{
		++count;
		next_char();
	}

	return count;
}

::size_t odata_expression_lexer::scan_hex_digits()
{
	::size_t count = 0;

	while (::odata::utility::is_hex_digit(current_char()))
	{
		++count;
		next_char();
	}

	return count;
}

void odata_expression_lexer::scan_identifier()
{
	while (::odata::utility::is_letter_or_digit(current_char()))
	{
		next_char();
	}
}

void odata_expression_lexer::scan_json_object_or_array()
{
	::odata::utility::char_t close_char = current_char() == U('{') ? U('}') : U(']');

	// Move to next close char or EOF.
	while (current_char() != 0 && current_char() != close_char)
	{
		next_char();
	}

	// If EOF met before close char, throw.
	if (!current_char())
	{
		throw odata_exception(ERROR_MESSAGE_CLOSE_CHAR_EXPECTED(close_char));
	}

	// Skip close char.
	next_char();
}

void odata_expression_lexer::scan_string()
{
	while (current_char() == U('\''))
	{
		// Skip the current quote.
		next_char();

		// Move to next quote or EOF.
		while (current_char() != 0 && current_char() != U('\''))
		{
			next_char();
		}

		// If EOF met before string closes, throw.
		if (!current_char())
		{
			throw odata_exception(ERROR_MESSAGE_CLOSE_CHAR_EXPECTED(U('\'')));
		}

		// Skip the current quote.
		next_char();
	}
}

int odata_expression_lexer::scan_number()
{
	auto token_start = m_pos;

	if (current_char() == U('-'))
	{
		// Skip -
		next_char();
	}

	eat_char(::odata::utility::is_digit, CharKindDigit);

	scan_digits();

	// First assume it to be an integer.
	auto kind = determine_best_int_kind(token_start);

	if (current_char() == U('-'))
	{
		if (try_scan_datetimeoffset(token_start))
		{
			return odata_expression_token_kind::DateTimeOffsetLiteral;
		}

		if (try_scan_guid(token_start))
		{
			return odata_expression_token_kind::GuidLiteral;
		}
	}

	// Guid could contain letters as well.
	if (::odata::utility::is_letter(current_char()))
	{
		if (try_scan_guid(token_start))
		{
			return odata_expression_token_kind::GuidLiteral;
		}
	}

	if (current_char() == U('.'))
	{
		// Skip dot.
		next_char();

		eat_char(::odata::utility::is_digit, CharKindDigit);
		scan_digits();

		// Dot met. Assume it to be a decimal or double or float number.
		kind = determine_best_decimal_kind(token_start, true);
	}

	// Handle exponents.
	if (current_char() == U('e') || current_char() == U('E'))
	{
		// Skip e or E.
		next_char();

		if (current_char() == U('+') || current_char() == U('-'))
		{
			// Skip exponent sign.
			next_char();
		}

		eat_char(::odata::utility::is_digit, CharKindDigit);
		scan_digits();

		// Dot met. Assume it to be a double or float number.
		kind = determine_best_decimal_kind(token_start, false);
	}

	// Shall we make a best guess before returning the kind?
	return kind;
}

#define SCAN_CHAR(CHAR) \
	do \
	{ \
		if (current_char() != (CHAR)) \
		{ \
			m_pos = orig_pos; \
			return false; \
		} \
		\
		next_char(); \
	} \
	while (false)

#define SCAN_DASH() SCAN_CHAR(U('-'))
#define SCAN_T() SCAN_CHAR(U('T'))
#define SCAN_COLON() SCAN_CHAR(U(':'))

#define SCAN_M_N_DIGITS(M, N) \
	do \
	{ \
		auto count = scan_digits(); \
		if (count < (M) || count > (N)) \
		{ \
			m_pos = orig_pos; \
			return false; \
		} \
	} \
	while (false)

#define SCAN_N_DIGITS(N) SCAN_M_N_DIGITS((N), (N))
#define SCAN_N_INF_DIGITS(N) SCAN_M_N_DIGITS((N), SIZE_MAX)

bool odata_expression_lexer::try_scan_datetimeoffset(::size_t start)
{
	auto orig_pos = m_pos;
	m_pos = start;

	if (current_char() == U('-'))
	{
		// Skip -
		next_char();
	}

	// Scan year.
	SCAN_N_INF_DIGITS(4);
	SCAN_DASH();

	// Scan month.
	SCAN_N_DIGITS(2);
	SCAN_DASH();

	// Scan day.
	SCAN_N_DIGITS(2);
	SCAN_T();

	// Scan hour.
	SCAN_N_DIGITS(2);
	SCAN_COLON();

	// Scan minute.
	SCAN_N_DIGITS(2);

	if (current_char() == U(':'))
	{
		// Skip :
		next_char();

		// Scan second.
		SCAN_N_DIGITS(2);

		if (current_char() == U('.'))
		{
			// Skip dot
			next_char();

			SCAN_M_N_DIGITS(1, 12);
		}
	}

	if (current_char() == U('Z'))
	{
		// Skip Z
		next_char();

		return true;
	}

	// Must be a sign.
	if (current_char() == U('+') || current_char() == U('-'))
	{
		// Skip sign.
		next_char();
	}
	else
	{
		return false;
	}

	// Scan hour.
	SCAN_N_DIGITS(2);
	SCAN_COLON();

	// Scan minute.
	SCAN_N_DIGITS(2);

	return true;
}

#define SCAN_N_HEX_DIGITS(N) \
	do \
	{ \
		if (scan_hex_digits() != (N)) \
		{ \
			m_pos = orig_pos; \
			return false; \
		} \
	} \
	while (false)

bool odata_expression_lexer::try_scan_guid(::size_t start)
{
	auto orig_pos = m_pos;
	m_pos = start;

	SCAN_N_HEX_DIGITS(8);
	SCAN_DASH();

	for (int k = 0; k < 3; ++k)
	{
		SCAN_N_HEX_DIGITS(4);
		SCAN_DASH();
	}

	SCAN_N_HEX_DIGITS(12);

	return true;
}

int odata_expression_lexer::try_scan_quoted_literal(::size_t start)
{
	int kind = odata_expression_token_kind::None;

	if (current_char() == U('\''))
	{
		auto text = extract_text(start);

		if (text == LiteralPrefixDuration)
		{
			kind = odata_expression_token_kind::DurationLiteral;
		}
		else if (text == LiteralPrefixBinary)
		{
			kind = odata_expression_token_kind::BinaryLiteral;
		}
		else
		{
			// For other unsupported quoted literals.
			kind = odata_expression_token_kind::QuotedLiteral;
		}

		if (kind != odata_expression_token_kind::None)
		{
			scan_string();
		}
	}

	return kind;
}

int odata_expression_lexer::determine_best_int_kind(::size_t start) const
{
	::odata::utility::string_t text = extract_text(start);

	auto int64_value = ::odata::utility::conversions::scan_string<int64_t>(text);
	if (int64_value < INT32_MIN || int64_value > INT32_MAX)
	{
		return odata_expression_token_kind::Int64Literal;
	}

	return odata_expression_token_kind::Int32Literal;
}

int odata_expression_lexer::determine_best_decimal_kind(::size_t start, bool can_be_decimal) const
{
	if (can_be_decimal)
	{
		return odata_expression_token_kind::DecimalLiteral;
	}

	::odata::utility::string_t text = extract_text(start);

	auto dbl_value = ::odata::utility::conversions::scan_string<double>(text);

	if (dbl_value < FLT_MIN || dbl_value > FLT_MAX)
	{
		return odata_expression_token_kind::DoubleLiteral;
	}
	
	return odata_expression_token_kind::SingleLiteral;
}

// --END-- odata_expression_lexer

// --BEGIN-- odata_expression_parser

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_expression(const ::odata::utility::string_t &expression, int parser_subject)
{
	auto lexer = odata_expression_lexer::create_lexer(expression);
	return parse_expression(lexer, parser_subject);
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_expression(std::shared_ptr<odata_expression_lexer> lexer, int parser_subject)
{
	auto parser = std::make_shared<odata_expression_parser>(lexer, parser_subject);
	return parser->parse_logical_or();
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_logical_or()
{
	auto node = parse_logical_and();
	while (lexer()->current_token()->is_identifier(KeywordOr))
	{
		// Skip 'or'
		lexer()->next_token();

		node = odata_query_node::create_operator_or_node(node, parse_logical_and());
	}

	return node;
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_logical_and()
{
	auto node = parse_comparison();
	while (lexer()->current_token()->is_identifier(KeywordAnd))
	{
		// Skip 'and'
		lexer()->next_token();

		node = odata_query_node::create_operator_and_node(node, parse_comparison());
	}

	return node;
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_comparison()
{
	auto node = parse_additive();

	while (lexer()->current_token()->is_identifier())
	{
		if (lexer()->current_token()->text() == KeywordEqual)
		{
			lexer()->next_token();
			node = odata_query_node::create_operator_eq_node(node, parse_additive());
		}
		else if (lexer()->current_token()->text() == KeywordNotEqual)
		{
			lexer()->next_token();
			node = odata_query_node::create_operator_ne_node(node, parse_additive());
		}
		else if (lexer()->current_token()->text() == KeywordGreaterThan)
		{
			lexer()->next_token();
			node = odata_query_node::create_operator_gt_node(node, parse_additive());
		}
		else if (lexer()->current_token()->text() == KeywordGreaterThanOrEqual)
		{
			lexer()->next_token();
			node = odata_query_node::create_operator_ge_node(node, parse_additive());
		}
		else if (lexer()->current_token()->text() == KeywordLessThan)
		{
			lexer()->next_token();
			node = odata_query_node::create_operator_lt_node(node, parse_additive());
		}
		else if (lexer()->current_token()->text() == KeywordLessThanOrEqual)
		{
			lexer()->next_token();
			node = odata_query_node::create_operator_le_node(node, parse_additive());
		}
		else if (lexer()->current_token()->text() == KeywordHas)
		{
			lexer()->next_token();
			node = odata_query_node::create_operator_has_node(node, parse_additive());
		}
		else
		{
			break;
		}
	}

	return node;
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_additive()
{
	auto node = parse_multiplicative();

	while (lexer()->current_token()->is_identifier())
	{
		if (lexer()->current_token()->text() == KeywordAdd)
		{
			lexer()->next_token();
			node = odata_query_node::create_operator_add_node(node, parse_multiplicative());
		}
		else if (lexer()->current_token()->text() == KeywordSub)
		{
			lexer()->next_token();
			node = odata_query_node::create_operator_sub_node(node, parse_multiplicative());
		}
		else
		{
			break;
		}
	}

	return node;
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_multiplicative()
{
	auto node = parse_unary();

	while (lexer()->current_token()->is_identifier())
	{
		if (lexer()->current_token()->text() == KeywordMultiply)
		{
			lexer()->next_token();
			node = odata_query_node::create_operator_mul_node(node, parse_unary());
		}
		else if (lexer()->current_token()->text() == KeywordDivide)
		{
			lexer()->next_token();
			node = odata_query_node::create_operator_div_node(node, parse_unary());
		}
		else if (lexer()->current_token()->text() == KeywordModulo)
		{
			lexer()->next_token();
			node = odata_query_node::create_operator_mod_node(node, parse_unary());
		}
		else
		{
			break;
		}
	}

	return node;
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_unary()
{
	if (lexer()->current_token()->token_kind() == odata_expression_token_kind::Minus)
	{
		lexer()->next_token();
		return odata_query_node::create_operator_neg_node(parse_unary());
	}
	else if (lexer()->current_token()->is_identifier(KeywordNot))
	{
		lexer()->next_token();
		return odata_query_node::create_operator_not_node(parse_unary());
	}

	return parse_postfix();
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_star(
    std::shared_ptr<::odata::core::odata_query_node> parent)
{
    return odata_query_node::create_property_access_node(TokenStar, parent);
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_postfix()
{
    std::shared_ptr<::odata::core::odata_query_node> node;
    
    if (lexer()->peek_token()->token_kind() == odata_expression_token_kind::Slash)
    {
        node = parse_identifier(nullptr);
    }
    else
    {
        return parse_primary();
    }
    
    do
    {
        // Skip '/'
        lexer()->next_token();
        
        auto current = lexer()->current_token();
        auto next = lexer()->peek_token();
        if (current->is_identifier(KeywordAll) || current->is_identifier(KeywordAny))
        {
            node = parse_lambda(node);
        }
        else if (next->token_kind() == odata_expression_token_kind::Slash)
        {
            // In path
            node = parse_identifier(node);
        }
        else
        {
            // End of path
            if (current->token_kind() == odata_expression_token_kind::Star)
            {
                node = parse_star(node);
            }
            else
            {
                node = parse_identifier(node);
            }
        }
    }
    while (lexer()->current_token()->token_kind() == odata_expression_token_kind::Slash);

    return node;
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_primary()
{
	switch (lexer()->current_token()->token_kind())
	{
    case odata_expression_token_kind::Star:
        return parse_star(nullptr);
        
	case odata_expression_token_kind::ParameterAlias:
		return parse_parameter_alias();

	case odata_expression_token_kind::LeftParen:
		return parse_parenthesis();
        
    case odata_expression_token_kind::Identifier:
        // Path of single identifier like:
        //   $filter=Items
        return parse_identifier(nullptr);

	default:
		return parse_literal();
	}
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_parameter_alias()
{
    auto token = lexer()->eat_token(odata_expression_token_kind::ParameterAlias, TokenKindParameterAlias);
    
    return odata_query_node::create_parameter_alias_node(token->text());
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_parenthesis()
{
    lexer()->eat_token(odata_expression_token_kind::LeftParen, TokenLeftParen);
    
    auto node = parse_logical_or();
    
    lexer()->eat_token(odata_expression_token_kind::RightParen, TokenRightParen);
    
    return node;
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_literal()
{
    return odata_query_node::create_constant_node(lexer()->eat_literal());
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_lambda(
    std::shared_ptr<::odata::core::odata_query_node> parent)
{
    auto identifier = lexer()->eat_identifier();
    
    bool is_any = identifier == KeywordAny;
    
    lexer()->eat_token(odata_expression_token_kind::LeftParen, TokenLeftParen);
    
    auto parameter = lexer()->eat_identifier();

	if (m_lambda_parameters.find(parameter) != m_lambda_parameters.end())
	{
		throw odata_exception(ERROR_MESSAGE_DUPLICATE_RANGE_VARIABLE(parameter));
	}

	m_lambda_parameters.insert(parameter);
    
    lexer()->eat_token(odata_expression_token_kind::Colon, TokenColon);
    
    auto expression = parse_logical_or();
    
    lexer()->eat_token(odata_expression_token_kind::RightParen, TokenRightParen);
    
	// forget about the range variable after parsing the expression for this lambda.
	m_lambda_parameters.erase(parameter);
    
    return odata_query_node::create_lambda_node(is_any, expression, parameter, parent);
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::parse_identifier(
    std::shared_ptr<::odata::core::odata_query_node> parent)
{
    auto identifier = lexer()->eat_dotted_identifier();

	if (m_parser_subject == odata_expression_parser_subject::SearchClause)
	{
		// If we are parsing search expression, treat identifiers as string literals.
		return odata_query_node::create_constant_node(::odata::core::odata_primitive_value::make_primitive_value(identifier));
	}
    
    // Parse function call (not available in $expand).
	if (lexer()->current_token()->token_kind() == odata_expression_token_kind::LeftParen
		&& m_parser_subject != odata_expression_parser_subject::ExpandClause)
    {
        return expand_to_function_call(identifier, parent);
    }
    
    // Identifier is one of the lambda parameters & at top level (not property name).
    if (m_lambda_parameters.find(identifier) != m_lambda_parameters.end() && parent == nullptr)
    {
        return odata_query_node::create_range_variable_node(identifier);
    }
    
	if (identifier.find(U('.')) != ::odata::utility::string_t::npos)
	{
		// Dotted identifier is type cast.
		return odata_query_node::create_type_cast_node(identifier, parent);
	}

    return odata_query_node::create_property_access_node(identifier, parent);
}

std::shared_ptr<::odata::core::odata_query_node> odata_expression_parser::expand_to_function_call(
    const ::odata::utility::string_t &identifier,
    std::shared_ptr<::odata::core::odata_query_node> parent)
{
    std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<odata_query_node>>> parameters;
    
    lexer()->eat_token(odata_expression_token_kind::LeftParen, TokenLeftParen);
    
    if (lexer()->current_token()->token_kind() == odata_expression_token_kind::RightParen)
    {
        // Empty function parameters.
        lexer()->next_token();
    }
    else
    {
        parameters.push_back(parse_named_value());
        
        while (lexer()->current_token()->token_kind() == odata_expression_token_kind::Comma)
        {
            // Skip ','
            lexer()->next_token();
            
            parameters.push_back(parse_named_value());
        }
        
        lexer()->eat_token(odata_expression_token_kind::RightParen, TokenRightParen);
    }
    
    return odata_query_node::create_function_call_node(identifier, std::move(parameters));
}

std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_query_node>>
odata_expression_parser::parse_named_value()
{
    ::odata::utility::string_t name;
            
    if (lexer()->peek_token()->token_kind() == odata_expression_token_kind::Equal)
    {
		name = lexer()->eat_identifier();
        lexer()->next_token();
    }
    
    auto value = parse_logical_or();
    
    return std::make_pair(name, value);
}

// --END-- odata_expression_parser

// --BEGIN-- odata_uri_parser

std::shared_ptr<::odata::core::odata_path> odata_uri_parser::parse_path(const ::odata::utility::string_t& path)
{
    return path_parser()->parse_path(path);
}

std::shared_ptr<::odata::core::odata_select_expand_clause> odata_uri_parser::parse_select_and_expand(
    const ::odata::utility::string_t& select_query, const ::odata::utility::string_t& expand_query)
{
	return query_option_parser()->parse_select_and_expand(select_query, expand_query);
}

std::shared_ptr<::odata::core::odata_filter_clause> odata_uri_parser::parse_filter(const ::odata::utility::string_t& filter_query)
{
	return query_option_parser()->parse_filter(filter_query);
}

std::shared_ptr<::odata::core::odata_orderby_clause> odata_uri_parser::parse_orderby(
    const ::odata::utility::string_t& orderby_query)
{
	return query_option_parser()->parse_orderby(orderby_query);
}

std::shared_ptr<::odata::core::odata_search_clause> odata_uri_parser::parse_search(const ::odata::utility::string_t& search_query)
{
	return query_option_parser()->parse_search(search_query);
}

::odata::common::nullable<int64_t> odata_uri_parser::parse_top(const ::odata::utility::string_t& top_query)
{
	return query_option_parser()->parse_top(top_query);
}

::odata::common::nullable<int64_t> odata_uri_parser::parse_skip(const ::odata::utility::string_t& skip_query)
{
	return query_option_parser()->parse_skip(skip_query);
}

::odata::common::nullable<bool> odata_uri_parser::parse_count(const ::odata::utility::string_t& count_query)
{
	return query_option_parser()->parse_count(count_query);
}

std::shared_ptr<::odata::core::odata_uri> odata_uri_parser::parse_uri(const ::odata::utility::uri &uri)
{
	auto path = parse_path(::odata::utility::uri::decode(uri.path()));

	auto query_options = split_query_options(uri.query());

	::odata::utility::string_t select_query;
	::odata::utility::string_t expand_query;
	::odata::utility::string_t filter_query;
	::odata::utility::string_t orderby_query;
	::odata::utility::string_t search_query;
	::odata::utility::string_t top_query;
	::odata::utility::string_t skip_query;
	::odata::utility::string_t count_query;

	for (auto iter = query_options.begin(); iter != query_options.end(); ++iter)
	{
		auto &option = *iter;

		if (option.first == QueryOptionSelect)
		{
			select_query = ::odata::utility::uri::decode(option.second);
		}
		else if (option.first == QueryOptionExpand)
		{
			expand_query = ::odata::utility::uri::decode(option.second);
		}
		else if (option.first == QueryOptionFilter)
		{
			filter_query = ::odata::utility::uri::decode(option.second);
		}
		else if (option.first == QueryOptionOrderBy)
		{
			orderby_query = ::odata::utility::uri::decode(option.second);
		}
		else if (option.first == QueryOptionSearch)
		{
			search_query = ::odata::utility::uri::decode(option.second);
		}
		else if (option.first == QueryOptionTop)
		{
			top_query = ::odata::utility::uri::decode(option.second);
		}
		else if (option.first == QueryOptionSkip)
		{
			skip_query = ::odata::utility::uri::decode(option.second);
		}
		else if (option.first == QueryOptionCount)
		{
			count_query = ::odata::utility::uri::decode(option.second);
		}
	}

	auto select_expand_clause = parse_select_and_expand(select_query, expand_query);
	auto filter_clause = parse_filter(filter_query);
	auto orderby_clause = parse_orderby(orderby_query);
	auto search_clause = parse_search(search_query);
	auto top = parse_top(top_query);
	auto skip = parse_skip(skip_query);
	auto count = parse_count(count_query);

	return ::odata::core::odata_uri::create_uri(
		path,
		select_expand_clause,
		filter_clause,
		orderby_clause,
		search_clause,
		top,
		skip,
		count);
}

::size_t odata_uri_parser::advance_through_string_literal(const ::odata::utility::string_t &query, ::size_t start)
{
	auto index = start;

	while (index < query.size() && query[index] == U('\''))
	{
		++index;

		while (index < query.size() && query[index] != U('\''))
		{
			++index;
		}
        
        if (index >= query.size())
        {
            throw odata_exception(ERROR_MESSAGE_CLOSE_CHAR_EXPECTED(U('\'')));
        }
			
		++index;
	}

	return index;
}

::size_t odata_uri_parser::advance_through_balanced_parenthesis(const ::odata::utility::string_t &query, ::size_t start)
{
	auto index = start;

	if (query[index] == U('('))
	{
		++index;

		::size_t depth = 1;

		while (index < query.size() && depth > 0)
		{
			if (query[index] == U('\''))
			{
				index = advance_through_string_literal(query, index);
				continue;
			}
			else if (query[index] == U('('))
			{
				++depth;
			}
			else if (query[index] == U(')'))
			{
                if (depth == 0)
                {
                    throw odata_exception(ERROR_MESSAGE_PARENTHESIS_MISMATCH());
                }
				
                --depth;
			}

			++index;
		}
        
        if (depth > 0)
        {
            throw odata_exception(ERROR_MESSAGE_PARENTHESIS_MISMATCH());
        }
	}

	return index;
}

std::map<::odata::utility::string_t, ::odata::utility::string_t> odata_uri_parser::split_query_options(const ::odata::utility::string_t &query, bool is_semicolon_separated)
{
	std::map<::odata::utility::string_t, ::odata::utility::string_t> options;

	if (query.empty())
	{
		return options;
	}

	::odata::utility::char_t delim = is_semicolon_separated ? U(';') : U('&');

	::odata::utility::string_t name;
	::odata::utility::string_t value;

	::size_t token_start = 0;
	::size_t index = 0;
	while (index < query.size())
	{
		if (query[index] == U('\''))
		{
			index = advance_through_string_literal(query, index);
			continue;
		}
		else if (query[index] == U('('))
		{
			index = advance_through_balanced_parenthesis(query, index);
			continue;
		}
		else if (query[index] == delim)
		{
			value = query.substr(token_start, index - token_start);
			token_start = index + 1;

			options.insert(std::make_pair(std::move(name), std::move(value)));
		}
		else if (query[index] == U('='))
		{
			name = query.substr(token_start, index - token_start);
			token_start = index + 1;
		}

		++index;
	}

	value = query.substr(token_start);
	options.insert(std::make_pair(std::move(name), std::move(value)));

	return std::move(options);
}

std::shared_ptr<odata_path_parser> odata_uri_parser::path_parser()
{
    if (m_path_parser == nullptr)
    {
        m_path_parser = std::make_shared<odata_path_parser>(model());
    }
    
    return m_path_parser;
}

std::shared_ptr<odata_query_option_parser> odata_uri_parser::query_option_parser()
{
    if (m_query_option_parser == nullptr)
    {
        m_query_option_parser = std::make_shared<odata_query_option_parser>(
            model(),
            path_parser()->target_type(),
            path_parser()->target_navigation_source());
    }
    
    return m_query_option_parser;
}

// --END-- odata_uri_parser

}}