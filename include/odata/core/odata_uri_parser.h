//---------------------------------------------------------------------
// <copyright file="odata_uri_parser.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/common/nullable.h"
#include "odata/edm/odata_edm.h"
#include "odata/edm/edm_model_utility.h"
#include "odata/core/odata_primitive_value.h"
#include "odata/core/odata_path.h"
#include "odata/core/odata_select_expand_clause.h"
#include "odata/core/odata_filter_clause.h"
#include "odata/core/odata_orderby_clause.h"
#include "odata/core/odata_search_clause.h"
#include "odata/core/odata_uri.h"
#include "odata/core/odata_exception.h"
#include "odata/core/odata_query_node.h"

namespace odata { namespace core
{

class odata_path_parser
{
public:
	odata_path_parser(std::shared_ptr<::odata::edm::edm_model> model)
		: m_model(model) {}
	~odata_path_parser() {}

	std::shared_ptr<::odata::core::odata_path> parse_path(const ::odata::utility::string_t& path);
    
    std::shared_ptr<::odata::edm::edm_named_type> target_type() const { return m_target_type; }
    
    std::shared_ptr<::odata::edm::edm_navigation_source> target_navigation_source() const
    { return m_target_navigation_source.lock(); }

private:
    odata_path_parser(const odata_path_parser &);
    odata_path_parser &operator=(const odata_path_parser &);

	std::shared_ptr<::odata::edm::edm_model> model() const { return m_model.lock(); }

	std::shared_ptr<::odata::core::odata_path_segment> previous_segment() const
	{ return m_bound_segments.empty() ? nullptr : m_bound_segments[m_bound_segments.size() - 1]; }

	static std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> parse_named_values(const ::odata::utility::string_t &exp);
	static void extract_segment_identifier_and_parenthesis_exp(const ::odata::utility::string_t& segment_str, ::odata::utility::string_t& identifier, ::odata::utility::string_t& parenthesis_exp);

	void bind_path(std::vector<::odata::utility::string_t> raw_segments);
	void bind_first_segment(const ::odata::utility::string_t &segment_str);
	void bind_next_segment(const ::odata::utility::string_t &segment_str);

	bool try_bind_as_navigation_source(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &key_exp);
	bool try_bind_as_operation_import(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &parameters_exp);
    bool try_bind_as_value(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &empty_exp);
	bool try_bind_as_count(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &empty_exp);
	bool try_bind_as_ref(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &empty_exp);
    bool try_bind_as_declared_property(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &key_exp);
    bool try_bind_as_type(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &empty_exp);
    bool try_bind_as_operation(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &parameters_exp);
    
    void bind_as_key(const ::odata::utility::string_t &key_exp);
    void bind_as_dynamic_property(const ::odata::utility::string_t &identifier, const ::odata::utility::string_t &empty_exp);
    
	std::shared_ptr<::odata::edm::edm_navigation_source> get_target_navigation_source();
	std::shared_ptr<::odata::edm::edm_named_type> get_target_type();
    void compute_target_navigation_source_and_type();

	static void validate_is_nonempty_relative_uri(const ::odata::utility::string_t& uri);
	static void validate_parameters_match(
		const std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> &parameters_parsed,
		const std::vector<std::shared_ptr<::odata::edm::edm_operation_parameter>> &parameters_from_model,
		bool has_binding_parameter = false);
	void validate_can_bind_next_segment();

private:
	std::weak_ptr<::odata::edm::edm_model> m_model;
	std::vector<std::shared_ptr<::odata::core::odata_path_segment>> m_bound_segments;

	// Use shared_ptr here because we may create a collection type which is not referenced in the model.
    std::shared_ptr<::odata::edm::edm_named_type> m_target_type;

    std::weak_ptr<::odata::edm::edm_navigation_source> m_target_navigation_source;
};

class odata_range_variable
{
public:
    odata_range_variable(const ::odata::utility::string_t &name, std::shared_ptr<::odata::edm::edm_named_type> target_type)
        : m_name(name),
        m_target_type(target_type) {}
    
    odata_range_variable(
        const ::odata::utility::string_t &name,
        std::shared_ptr<::odata::edm::edm_named_type> target_type,
        std::shared_ptr<::odata::edm::edm_navigation_source> target_navigation_source)
        : m_name(name),
        m_target_type(target_type),
        m_target_navigation_source(target_navigation_source) {}
    
    const ::odata::utility::string_t &name() const { return m_name; }
    std::shared_ptr<::odata::edm::edm_named_type> target_type() const { return m_target_type.lock(); }
    std::shared_ptr<::odata::edm::edm_navigation_source> target_navigation_source() const
    { return m_target_navigation_source.lock(); }

private:
    odata_range_variable(const odata_range_variable &);
    odata_range_variable &operator=(const odata_range_variable &);
    
private:
    ::odata::utility::string_t m_name;
    std::weak_ptr<::odata::edm::edm_named_type> m_target_type;
    std::weak_ptr<::odata::edm::edm_navigation_source> m_target_navigation_source;
};

class odata_select_item
{
public:
    odata_select_item(std::shared_ptr<::odata::core::odata_query_node> end)
        : m_end(end) {}
    ~odata_select_item() {}

	static std::shared_ptr<::odata::core::odata_select_item> create(
		std::shared_ptr<::odata::core::odata_query_node> end);
    
    std::shared_ptr<::odata::core::odata_query_node> end() const
    { return m_end; }

private:
    odata_select_item(const odata_select_item &);
    odata_select_item &operator=(const odata_select_item &);
    
private:
    std::shared_ptr<::odata::core::odata_query_node> m_end;
};

class odata_expand_item
{
public:
    odata_expand_item(
        std::shared_ptr<::odata::core::odata_query_node> end,
        std::shared_ptr<::odata::core::odata_select_expand_clause> select_expand_clause,
        std::shared_ptr<::odata::core::odata_filter_clause> filter_clause,
        std::shared_ptr<::odata::core::odata_orderby_clause> orderby_clause,
        std::shared_ptr<::odata::core::odata_search_clause> search_clause,
        ::odata::common::nullable<int64_t> top,
        ::odata::common::nullable<int64_t> skip,
        ::odata::common::nullable<bool> count)
        : m_end(end),
        m_select_expand_clause(select_expand_clause),
        m_filter_clause(filter_clause),
        m_orderby_clause(orderby_clause),
        m_search_clause(search_clause),
        m_top(top),
        m_skip(skip),
        m_count(count) {}
    ~odata_expand_item() {}

	static std::shared_ptr<::odata::core::odata_expand_item> create(
		std::shared_ptr<::odata::core::odata_query_node> end);

	static std::shared_ptr<::odata::core::odata_expand_item> create(
		std::shared_ptr<::odata::core::odata_query_node> end,
		std::shared_ptr<::odata::core::odata_select_expand_clause> select_expand_clause,
        std::shared_ptr<::odata::core::odata_filter_clause> filter_clause,
        std::shared_ptr<::odata::core::odata_orderby_clause> orderby_clause,
        std::shared_ptr<::odata::core::odata_search_clause> search_clause,
        ::odata::common::nullable<int64_t> top,
        ::odata::common::nullable<int64_t> skip,
        ::odata::common::nullable<bool> count);
    
    std::shared_ptr<::odata::core::odata_query_node> end() const
    { return m_end; }
    
    std::shared_ptr<::odata::core::odata_select_expand_clause> select_expand_clause() const
    { return m_select_expand_clause; }
    
    std::shared_ptr<::odata::core::odata_filter_clause> filter_clause() const
    { return m_filter_clause; }
    
    std::shared_ptr<::odata::core::odata_orderby_clause> orderby_clause() const
    { return m_orderby_clause; }
    
    std::shared_ptr<::odata::core::odata_search_clause> search_clause() const
    { return m_search_clause; }
    
    ::odata::common::nullable<int64_t> top() const
    { return m_top; }
    
    ::odata::common::nullable<int64_t> skip() const
    { return m_skip; }
    
    ::odata::common::nullable<bool> count() const
    { return m_count; }
    
private:
    odata_expand_item(const odata_expand_item &);
    odata_expand_item &operator=(const odata_expand_item &);
    
private:
    std::shared_ptr<::odata::core::odata_query_node> m_end;
    std::shared_ptr<::odata::core::odata_select_expand_clause> m_select_expand_clause;
    std::shared_ptr<::odata::core::odata_filter_clause> m_filter_clause;
    std::shared_ptr<::odata::core::odata_orderby_clause> m_orderby_clause;
    std::shared_ptr<::odata::core::odata_search_clause> m_search_clause;
    ::odata::common::nullable<int64_t> m_top;
    ::odata::common::nullable<int64_t> m_skip;
    ::odata::common::nullable<bool> m_count;
};

class odata_query_option_parser
{
public:
	odata_query_option_parser(
        std::shared_ptr<::odata::edm::edm_model> model,
        std::shared_ptr<::odata::edm::edm_named_type> target_type,
        std::shared_ptr<::odata::edm::edm_navigation_source> target_navigation_source)
		: m_model(model),
        m_target_type(target_type),
        m_target_navigation_source(target_navigation_source) {}
	~odata_query_option_parser() {}
    
    std::shared_ptr<::odata::edm::edm_named_type> target_type() const { return m_target_type; }
    
    std::shared_ptr<::odata::edm::edm_navigation_source> target_navigation_source() const
    { return m_target_navigation_source.lock(); }

	std::shared_ptr<::odata::core::odata_select_expand_clause> parse_select_and_expand(
        const ::odata::utility::string_t& select_query,
        const ::odata::utility::string_t& expand_query);
	std::shared_ptr<::odata::core::odata_filter_clause> parse_filter(const ::odata::utility::string_t& filter_query);
	std::shared_ptr<::odata::core::odata_orderby_clause> parse_orderby(const ::odata::utility::string_t& orderby_query);
	std::shared_ptr<::odata::core::odata_search_clause> parse_search(const ::odata::utility::string_t& search_query);
	::odata::common::nullable<int64_t> parse_top(const ::odata::utility::string_t& top_query);
	::odata::common::nullable<int64_t> parse_skip(const ::odata::utility::string_t& skip_query);
	::odata::common::nullable<bool> parse_count(const ::odata::utility::string_t& count_query);

private:
    odata_query_option_parser(const odata_query_option_parser &);
    odata_query_option_parser &operator=(const odata_query_option_parser &);

	std::shared_ptr<::odata::edm::edm_model> model() const { return m_model.lock(); }
	std::shared_ptr<odata_range_variable> create_implicit_range_variable() const;
    
    std::vector<std::shared_ptr<::odata::core::odata_select_item>> parse_select(const ::odata::utility::string_t& select_query);
    std::vector<std::shared_ptr<::odata::core::odata_expand_item>> parse_expand(const ::odata::utility::string_t& expand_query);

private:
	std::weak_ptr<::odata::edm::edm_model> m_model;
    std::shared_ptr<::odata::edm::edm_named_type> m_target_type;
    std::weak_ptr<::odata::edm::edm_navigation_source> m_target_navigation_source;
};

namespace odata_expression_token_kind {
enum {
	None,
	End,
	Equal,
	Identifier,
	NullLiteral,
	BooleanLiteral,
	StringLiteral,
	Int32Literal,
	Int64Literal,
	SingleLiteral,
	DateTimeOffsetLiteral,
	DurationLiteral,
	DecimalLiteral,
	DoubleLiteral,
	GuidLiteral,
	BinaryLiteral,
	LeftParen,
	RightParen,
	Comma,
	Colon,
	Minus,
	Slash,
	Dot,
	Star,
	ParameterAlias,
	JsonObjectOrArray,
	QuotedLiteral
};
}

class odata_expression_token
{
public:
	odata_expression_token(int token_kind, const ::odata::utility::string_t &text, ::size_t pos)
		: m_token_kind(token_kind),
		m_text(text),
		m_pos(pos) {}
	~odata_expression_token() {}

	ODATACPP_API static std::shared_ptr<odata_expression_token> create_token(
        int token_kind,
        const ::odata::utility::string_t &text,
        ::size_t pos);

	int token_kind() const { return m_token_kind; }
	const ::odata::utility::string_t &text() const { return m_text; }
	::size_t position() const { return m_pos; }

	bool is_literal() const;
	bool is_identifier() const;
	bool is_identifier(const ::odata::utility::string_t &text) const;

	ODATACPP_API std::shared_ptr<::odata::core::odata_primitive_value> to_primitive_value() const;

private:
	odata_expression_token(const odata_expression_token &);
	odata_expression_token &operator=(const odata_expression_token &);

private:
	int m_token_kind;
	::odata::utility::string_t m_text;
	::size_t m_pos;
};

class odata_expression_lexer
{
public:
	odata_expression_lexer(const ::odata::utility::string_t &expression)
		: m_expression(expression),
		m_pos(0) {}
	~odata_expression_lexer() {}

	ODATACPP_API static std::shared_ptr<odata_expression_lexer> create_lexer(const ::odata::utility::string_t &expression);

	ODATACPP_API void next_token();
	ODATACPP_API std::shared_ptr<odata_expression_token> peek_token();
	ODATACPP_API std::shared_ptr<odata_expression_token> current_token();
	ODATACPP_API std::shared_ptr<odata_expression_token> eat_token(
        int token_kind,
        const ::odata::utility::string_t &_expected_token);
    
    ODATACPP_API ::odata::utility::string_t eat_identifier();
    ODATACPP_API ::odata::utility::string_t eat_dotted_identifier();
    ODATACPP_API std::shared_ptr<::odata::core::odata_primitive_value> eat_literal();

	ODATACPP_API ::odata::utility::string_t advance_through_balanced_parenthesis();

private:
    odata_expression_lexer(const odata_expression_lexer &);
    odata_expression_lexer &operator=(const odata_expression_lexer &);

	::odata::utility::char_t char_at(::size_t i) const { return i < m_expression.size() ? m_expression[i] : 0; }
	::odata::utility::char_t current_char() const { return char_at(m_pos); }
	::odata::utility::char_t peek_char() const { return char_at(m_pos + 1); }
	void next_char();
	::odata::utility::char_t eat_char(
        std::function<bool (::odata::utility::char_t)> validator,
        const ::odata::utility::string_t &_expected_char);

	bool is_valid_start_of_identifier() const
	{ return ::odata::utility::is_letter(current_char()) || current_char() == U('_') || current_char() == U('$'); }

	::odata::utility::string_t extract_text(::size_t start) const { return m_expression.substr(start, m_pos - start); }

	void scan_whitespaces();
	::size_t scan_digits();
	::size_t scan_hex_digits();
	void scan_identifier();
	void scan_json_object_or_array();
	void scan_string();
	int scan_number();

	bool try_scan_datetimeoffset(::size_t start);
	bool try_scan_guid(::size_t start);
	int try_scan_quoted_literal(::size_t start);

	int determine_best_int_kind(::size_t start) const;
	int determine_best_decimal_kind(::size_t start, bool can_be_decimal) const;

private:
	::odata::utility::string_t m_expression;
	::size_t m_pos;
	std::shared_ptr<odata_expression_token> m_token;
};

namespace odata_expression_parser_subject {
enum {
	SelectClause,
	ExpandClause,
	FilterClause,
	OrderByClause,
	SearchClause
};
}

class odata_expression_parser
{
public:
	odata_expression_parser(std::shared_ptr<odata_expression_lexer> lexer, int parser_subject)
		: m_lexer(lexer),
		m_parser_subject(parser_subject) {}
	~odata_expression_parser() {}

	ODATACPP_API static std::shared_ptr<::odata::core::odata_query_node> parse_expression(
        const ::odata::utility::string_t &expression, int parser_subject = odata_expression_parser_subject::FilterClause);

	ODATACPP_API static std::shared_ptr<::odata::core::odata_query_node> parse_expression(
		std::shared_ptr<odata_expression_lexer> lexer, int parser_subject = odata_expression_parser_subject::FilterClause);

private:
    odata_expression_parser(const odata_expression_parser &);
    odata_expression_parser &operator=(const odata_expression_parser &);

	std::shared_ptr<odata_expression_lexer> lexer() const { return m_lexer.lock(); }

	std::shared_ptr<::odata::core::odata_query_node> parse_logical_or();
	std::shared_ptr<::odata::core::odata_query_node> parse_logical_and();
	std::shared_ptr<::odata::core::odata_query_node> parse_comparison();
	std::shared_ptr<::odata::core::odata_query_node> parse_additive();
	std::shared_ptr<::odata::core::odata_query_node> parse_multiplicative();
	std::shared_ptr<::odata::core::odata_query_node> parse_unary();
	std::shared_ptr<::odata::core::odata_query_node> parse_postfix();
	std::shared_ptr<::odata::core::odata_query_node> parse_primary();
    
    std::shared_ptr<::odata::core::odata_query_node> parse_parameter_alias();
    std::shared_ptr<::odata::core::odata_query_node> parse_parenthesis();
    std::shared_ptr<::odata::core::odata_query_node> parse_literal();
    std::shared_ptr<::odata::core::odata_query_node> parse_lambda(
        std::shared_ptr<::odata::core::odata_query_node> parent);
    std::shared_ptr<::odata::core::odata_query_node> parse_identifier(
        std::shared_ptr<::odata::core::odata_query_node> parent);
    std::shared_ptr<::odata::core::odata_query_node> expand_to_function_call(
        const ::odata::utility::string_t &identifier,
        std::shared_ptr<::odata::core::odata_query_node> parent);
    std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_query_node>> parse_named_value();
	std::shared_ptr<::odata::core::odata_query_node> parse_star(
		std::shared_ptr<::odata::core::odata_query_node> parent);

private:
	std::weak_ptr<odata_expression_lexer> m_lexer;
    std::unordered_set<::odata::utility::string_t> m_lambda_parameters;
	int m_parser_subject;
};

class odata_uri_parser
{
public:
    odata_uri_parser(std::shared_ptr<::odata::edm::edm_model> model)
		: m_model(model) {}
	~odata_uri_parser() {}

	ODATACPP_API std::shared_ptr<::odata::core::odata_path> parse_path(const ::odata::utility::string_t& path);
	ODATACPP_API std::shared_ptr<::odata::core::odata_select_expand_clause> parse_select_and_expand(
        const ::odata::utility::string_t& select_query,
        const ::odata::utility::string_t& expand_query);
	ODATACPP_API std::shared_ptr<::odata::core::odata_filter_clause> parse_filter(
        const ::odata::utility::string_t& filter_query);
	ODATACPP_API std::shared_ptr<::odata::core::odata_orderby_clause> parse_orderby(
        const ::odata::utility::string_t& orderby_query);
	ODATACPP_API std::shared_ptr<::odata::core::odata_search_clause> parse_search(
        const ::odata::utility::string_t& search_query);
	ODATACPP_API ::odata::common::nullable<int64_t> parse_top(const ::odata::utility::string_t& top_query);
	ODATACPP_API ::odata::common::nullable<int64_t> parse_skip(const ::odata::utility::string_t& skip_query);
	ODATACPP_API ::odata::common::nullable<bool> parse_count(const ::odata::utility::string_t& count_query);
	ODATACPP_API std::shared_ptr<::odata::core::odata_uri> parse_uri(const ::odata::utility::uri &uri);
    
    std::shared_ptr<::odata::edm::edm_model> model() const { return m_model.lock(); }

	static std::map<::odata::utility::string_t, ::odata::utility::string_t> split_query_options(const ::odata::utility::string_t &query, bool is_semicolon_separated = false);
    
private:
    odata_uri_parser(const odata_uri_parser &);
    odata_uri_parser &operator=(const odata_uri_parser &);
    
    std::shared_ptr<odata_path_parser> path_parser();
	std::shared_ptr<odata_query_option_parser> query_option_parser();

	static ::size_t advance_through_string_literal(const ::odata::utility::string_t &query, ::size_t start);
	static ::size_t advance_through_balanced_parenthesis(const ::odata::utility::string_t &query, ::size_t start);

private:
    std::weak_ptr<::odata::edm::edm_model> m_model;
	std::shared_ptr<odata_path_parser> m_path_parser;
	std::shared_ptr<odata_query_option_parser> m_query_option_parser;
};

}}