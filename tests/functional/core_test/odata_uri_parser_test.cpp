//---------------------------------------------------------------------
// <copyright file="odata_uri_parser_test.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "../odata_tests.h"
#include "odata/common/json.h"
#include "odata/core/odata_uri_parser.h"
#include "odata/core/odata_path_segment_visitor.h"
#include "odata/core/odata_query_node_visitor.h"

using namespace ::odata::edm;
using namespace ::odata::core;

namespace tests { namespace functional { namespace _odata {

SUITE(odata_uri_parser_test_cases)
{

static std::shared_ptr<edm_model> get_model_from_csdl(const char *csdl)
{
	std::istringstream iss(std::move(std::string(csdl)));
	auto model_reader = std::make_shared<edm_model_reader>(iss);
    
	model_reader->parse();
    
	return model_reader->get_model();
}

static std::shared_ptr<edm_model> get_test_model_with_person_with_one_key()
{
	static std::shared_ptr<edm_model> model;

	if (model == nullptr)
	{
		model = get_model_from_csdl( 
"<edmx:Edmx xmlns:edmx=\"http://docs.oasis-open.org/odata/ns/edmx\" Version=\"4.0\">\
    <edmx:DataServices>\
	  <Schema xmlns=\"http://docs.oasis-open.org/odata/ns/edm\" Namespace=\"MyNS\">\
	    <ComplexType Name=\"Address\">\
          <Property Name=\"Country\" Type=\"Edm.String\" Nullable=\"false\"/>\
        </ComplexType>\
		<EntityType Name=\"Person\">\
			<Key>\
			    <PropertyRef Name=\"ID\"/>\
			</Key>\
			<Property Name=\"ID\" Type=\"Edm.Int32\" Nullable=\"false\"/>\
            <Property Name=\"ChineseName\" Type=\"Edm.String\" Nullable=\"true\"/>\
			<Property Name=\"Age\" Type=\"Edm.Int32\" Nullable=\"false\"/>\
			<Property Name=\"Address\" Type=\"MyNS.Address\" Nullable=\"false\"/>\
			<Property Name=\"FormerAddresses\" Type=\"Collection(MyNS.Address)\" Nullable=\"false\"/>\
            <NavigationProperty Name=\"Parent\" Type=\"MyNS.Person\" Nullable=\"false\"/>\
            <NavigationProperty Name=\"Friends\" Type=\"Collection(MyNS.Person)\" Nullable=\"false\"/>\
			<NavigationProperty Name=\"Mates\" Type=\"Collection(MyNS.Person)\" Nullable=\"false\"/>\
			<NavigationProperty Name=\"Brothers\" Type=\"Collection(MyNS.Person)\" Nullable=\"false\" ContainsTarget=\"true\"/>\
		</EntityType>\
		<EntityType Name=\"VIP\" BaseType=\"MyNS.Person\"/>\
        <Function Name=\"AddIntAndDouble\" IsBound=\"false\" IsComposable=\"true\"> \
	      <Parameter Name=\"a\" Type=\"Edm.Int32\" Nullable=\"false\"/> \
	      <Parameter Name=\"b\" Type=\"Edm.Double\" Nullable=\"false\"/> \
          <ReturnType Type=\"Edm.Int32\" Nullable=\"false\"/> \
        </Function> \
		<Function Name=\"AddIntAndDouble1\" IsBound=\"true\" IsComposable=\"true\"> \
	      <Parameter Name=\"a\" Type=\"Edm.Int32\" Nullable=\"false\"/> \
	      <Parameter Name=\"b\" Type=\"Edm.Double\" Nullable=\"false\"/> \
          <ReturnType Type=\"Edm.Double\" Nullable=\"false\"/> \
        </Function> \
	    <Function Name=\"AddIntAndDouble2\" IsBound=\"false\" IsComposable=\"false\"> \
	      <Parameter Name=\"a\" Type=\"Edm.Int32\" Nullable=\"false\"/> \
	      <Parameter Name=\"b\" Type=\"Edm.Double\" Nullable=\"false\"/> \
          <ReturnType Type=\"Edm.Double\" Nullable=\"false\"/> \
        </Function> \
		<Function Name=\"GetFavNum\" IsBound=\"true\" IsComposable=\"true\"> \
	      <Parameter Name=\"person\" Type=\"MyNS.Person\" Nullable=\"false\"/> \
          <ReturnType Type=\"Edm.Int32\" Nullable=\"false\"/> \
        </Function> \
		<Function Name=\"AddOne\" IsBound=\"true\" IsComposable=\"true\"> \
	      <Parameter Name=\"x\" Type=\"Edm.Int32\" Nullable=\"false\"/> \
          <ReturnType Type=\"Edm.Int32\" Nullable=\"false\"/> \
        </Function> \
		<Function Name=\"AddTwo\" IsBound=\"true\" IsComposable=\"false\"> \
	      <Parameter Name=\"x\" Type=\"Edm.Int32\" Nullable=\"false\"/> \
          <ReturnType Type=\"Edm.Int32\" Nullable=\"false\"/> \
        </Function> \
		<Function Name=\"AddWith\" IsBound=\"true\"> \
	      <Parameter Name=\"x\" Type=\"Edm.Int32\" Nullable=\"false\"/> \
		  <Parameter Name=\"y\" Type=\"Edm.Int32\" Nullable=\"false\"/> \
          <ReturnType Type=\"Edm.Int32\" Nullable=\"false\"/> \
        </Function> \
		<Function Name=\"GetCountry\" IsBound=\"true\"> \
	      <Parameter Name=\"address\" Type=\"MyNS.Address\" Nullable=\"false\"/> \
          <ReturnType Type=\"Edm.String\" Nullable=\"false\"/> \
        </Function> \
		<EntityContainer Name=\"MyContainer\">\
		  <EntitySet Name=\"People\" EntityType=\"MyNS.Person\">\
            <NavigationPropertyBinding Path=\"Parent\" Target=\"People\" />\
			<NavigationPropertyBinding Path=\"Mates\" Target=\"People\" />\
          </EntitySet>\
		  <Singleton Name=\"CEO\" Type=\"MyNS.Person\"/>\
		  <FunctionImport Name=\"AddIntAndDouble\" Function=\"MyNS.AddIntAndDouble\"/> \
		  <FunctionImport Name=\"AddIntAndDouble1\" Function=\"MyNS.AddIntAndDouble1\"/> \
		</EntityContainer>\
	  </Schema> \
	</edmx:DataServices> \
	</edmx:Edmx>  \
");
	}

	return model;
}

static std::shared_ptr<edm_model> get_test_model_with_person_with_two_keys()
{
	static std::shared_ptr<edm_model> model;

	if (model == nullptr)
	{
		model = get_model_from_csdl( 
"<edmx:Edmx xmlns:edmx=\"http://docs.oasis-open.org/odata/ns/edmx\" Version=\"4.0\">\
    <edmx:DataServices>\
	  <Schema xmlns=\"http://docs.oasis-open.org/odata/ns/edm\" Namespace=\"MyNS\">\
	    <ComplexType Name=\"Address\">\
          <Property Name=\"Country\" Type=\"Edm.String\" Nullable=\"false\"/>\
        </ComplexType>\
		<EntityType Name=\"Person\">\
			<Key>\
			    <PropertyRef Name=\"ID1\"/>\
				<PropertyRef Name=\"ID2\"/>\
			</Key>\
			<Property Name=\"ID1\" Type=\"Edm.Int32\" Nullable=\"false\"/>\
			<Property Name=\"ID2\" Type=\"Edm.String\" Nullable=\"false\"/>\
			<Property Name=\"Address\" Type=\"MyNS.Address\" Nullable=\"false\"/>\
		</EntityType>\
		<EntityType Name=\"VIP\" BaseType=\"MyNS.Person\"/>\
		<EntityContainer Name=\"MyContainer\">\
		  <EntitySet Name=\"People\" EntityType=\"MyNS.Person\"/>\
		  <Singleton Name=\"CEO\" Type=\"MyNS.Person\"/>\
		</EntityContainer>\
	  </Schema> \
	</edmx:DataServices> \
	</edmx:Edmx>  \
");
	}

	return model;
}

static std::shared_ptr<edm_model> get_test_model_with_open_person()
{
	static std::shared_ptr<edm_model> model;

	if (model == nullptr)
	{
		model = get_model_from_csdl( 
"<edmx:Edmx xmlns:edmx=\"http://docs.oasis-open.org/odata/ns/edmx\" Version=\"4.0\">\
    <edmx:DataServices>\
	  <Schema xmlns=\"http://docs.oasis-open.org/odata/ns/edm\" Namespace=\"MyNS\">\
	    <ComplexType Name=\"Address\" OpenType=\"true\">\
          <Property Name=\"Country\" Type=\"Edm.String\" Nullable=\"false\"/>\
        </ComplexType>\
		<EntityType Name=\"Person\" OpenType=\"true\">\
			<Key>\
			    <PropertyRef Name=\"ID\"/>\
			</Key>\
			<Property Name=\"ID\" Type=\"Edm.Int32\" Nullable=\"false\"/>\
			<Property Name=\"Address\" Type=\"MyNS.Address\" Nullable=\"false\"/>\
		</EntityType>\
		<EntityType Name=\"VIP\" BaseType=\"MyNS.Person\"/>\
		<EntityContainer Name=\"MyContainer\">\
		  <EntitySet Name=\"People\" EntityType=\"MyNS.Person\"/>\
		  <Singleton Name=\"CEO\" Type=\"MyNS.Person\"/>\
		</EntityContainer>\
	  </Schema> \
	</edmx:DataServices> \
	</edmx:Edmx>  \
");
	}

	return model;
}

// --BEGIN-- odata_path_parser

TEST(invalid_relative_uri)
{
	odata_uri_parser parser(std::make_shared<edm_model>());
    
	VERIFY_THROWS(parser.parse_path(U("People(ID='abc')")), odata_exception);
}

TEST(empty_segment_identifier)
{
	odata_uri_parser parser(std::make_shared<edm_model>());
    
	VERIFY_THROWS(parser.parse_path(U("(ID='abc')")), odata_exception);
}  

TEST(multiple_keys_without_name)
{
	odata_uri_parser parser(get_test_model_with_person_with_two_keys());
    
	VERIFY_THROWS(parser.parse_path(U("/People('abc',123)")), odata_exception);
}

TEST(duplicate_key_name)
{
	odata_uri_parser parser(get_test_model_with_person_with_two_keys());
    
	VERIFY_THROWS(parser.parse_path(U("/People(ID1='abc',ID1=123)")), odata_exception);
}

TEST(parenthesis_mismatch)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
    
	VERIFY_THROWS(parser.parse_path(U("/People(ID='abc'")), odata_exception);
}

TEST(single_quote_mismatch)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
    
	VERIFY_THROWS(parser.parse_path(U("/People(ID='abc)")), odata_exception);
}

TEST(unexpected_parenthesis_expression)
{
	odata_uri_parser parser(std::make_shared<edm_model>());
    
	VERIFY_THROWS(parser.parse_path(U("/$metadata('123')")), odata_exception);
	VERIFY_THROWS(parser.parse_path(U("/$batch('123')")), odata_exception);
}

TEST(unexpected_path_root)
{
	odata_uri_parser parser(std::make_shared<edm_model>());
    
	VERIFY_THROWS(parser.parse_path(U("/$count")), odata_exception);
	VERIFY_THROWS(parser.parse_path(U("/$value")), odata_exception);
	VERIFY_THROWS(parser.parse_path(U("/$ref")), odata_exception);
}

TEST(unsupported_segment)
{
	odata_uri_parser parser(std::make_shared<edm_model>());
    
	VERIFY_THROWS(parser.parse_path(U("/$all")), odata_exception);
	VERIFY_THROWS(parser.parse_path(U("/$entity")), odata_exception);
	VERIFY_THROWS(parser.parse_path(U("/$crossjoin")), odata_exception);
}

TEST(resource_not_found)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
    
	VERIFY_THROWS(parser.parse_path(U("/People1(1)")), odata_exception);
}

TEST(key_count_mismatch)
{
	odata_uri_parser parser(get_test_model_with_person_with_two_keys());
    
	VERIFY_THROWS(parser.parse_path(U("/People(ID1=123)")), odata_exception);
}

static void verify_entity_set_segment(std::shared_ptr<odata_path_segment> segment, const ::odata::utility::string_t &name, const ::odata::utility::string_t &type)
{
	VERIFY_ARE_EQUAL(name, segment->as<odata_entity_set_segment>()->entity_set()->get_name());
	VERIFY_ARE_EQUAL(type, segment->as<odata_entity_set_segment>()->entity_type()->get_name());
}

static void verify_singleton_segment(std::shared_ptr<odata_path_segment> segment, const ::odata::utility::string_t &name, const ::odata::utility::string_t &type)
{
	VERIFY_ARE_EQUAL(name, segment->as<odata_singleton_segment>()->singleton()->get_name());
	VERIFY_ARE_EQUAL(type, segment->as<odata_singleton_segment>()->entity_type()->get_name());
}

static void verify_key_segment(std::shared_ptr<odata_path_segment> segment, const ::odata::utility::string_t &name, const ::odata::utility::string_t &value, ::size_t i = 0)
{
    VERIFY_ARE_EQUAL(name, segment->as<odata_key_segment>()->key_at(i).first);
	VERIFY_ARE_EQUAL(value, segment->as<odata_key_segment>()->key_at(i).second->to_string());
}

TEST(bind_entity_set_1)
{
	odata_uri_parser parser(get_test_model_with_person_with_two_keys());
	auto path = parser.parse_path(U("/People(ID1=123,ID2='abc')"));
    
	VERIFY_ARE_EQUAL(2, path->segments().size());
    
    verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(2, path->segment_at(1)->as<odata_key_segment>()->keys().size());
    verify_key_segment(path->segment_at(1), U("ID1"), U("123"));
    verify_key_segment(path->segment_at(1), U("ID2"), U("abc"), 1);
}

TEST(bind_entity_set_2)
{
	odata_uri_parser parser(get_test_model_with_person_with_two_keys());
	auto path = parser.parse_path(U("/People(ID2='abc',ID1=123)"));
    
	VERIFY_ARE_EQUAL(2, path->segments().size());
    
    verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(2, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID2"), U("abc"), 0);
    verify_key_segment(path->segment_at(1), U("ID1"), U("123"), 1);
}

TEST(comma_in_key_name)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(ID='ab,c')"));
    
	VERIFY_ARE_EQUAL(2, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
    verify_key_segment(path->segment_at(1), U("ID"), U("ab,c"));
}

TEST(bind_singleton)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/CEO"));
    
	VERIFY_ARE_EQUAL(1, path->segments().size());
    
    verify_singleton_segment(path->segment_at(0), U("CEO"), U("Person"));
}

TEST(unexpected_parenthesis_expression_after_singleton)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
    
	VERIFY_THROWS(parser.parse_path(U("/CEO(ID=12)")), odata_exception);
}

TEST(key_name_mismatch_1)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
    
	VERIFY_THROWS(parser.parse_path(U("/People(ID1=123)")), odata_exception);
}

TEST(key_name_mismatch_2)
{
	odata_uri_parser parser(get_test_model_with_person_with_two_keys());
    
	VERIFY_THROWS(parser.parse_path(U("/People(ID1=123,ID3='abc')")), odata_exception);
}

TEST(omit_name_when_only_one_key)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(123)"));
    
    VERIFY_ARE_EQUAL(2, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("123"));
}

static void verify_structural_property_segment(std::shared_ptr<odata_path_segment> segment, const ::odata::utility::string_t &owning_type, const ::odata::utility::string_t &property)
{
    VERIFY_ARE_EQUAL(owning_type, segment->as<odata_structural_property_segment>()->owning_type()->get_name());
    VERIFY_ARE_EQUAL(property, segment->as<odata_structural_property_segment>()->property()->get_name());
}

TEST(bind_entity_set_structural_property)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(123)/ID"));
    
    VERIFY_ARE_EQUAL(3, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("123"));
    
    verify_structural_property_segment(path->segment_at(2), U("Person"), U("ID"));
}

TEST(bind_singleton_structural_property)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/CEO/ID"));
    
	VERIFY_ARE_EQUAL(2, path->segments().size());
    
    verify_singleton_segment(path->segment_at(0), U("CEO"), U("Person"));
    
    verify_structural_property_segment(path->segment_at(1), U("Person"), U("ID"));
}

TEST(unexpected_parenthesis_expression_after_value)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
    
    VERIFY_THROWS(parser.parse_path(U("/CEO/ID/$value(1)")), odata_exception);
}

TEST(bind_value_after_structural_property)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(123)/ID/$value"));
    
    VERIFY_ARE_EQUAL(4, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("123"));
    
    verify_structural_property_segment(path->segment_at(2), U("Person"), U("ID"));
    
	VERIFY_ARE_EQUAL(::odata::core::odata_path_segment_type::Value, path->segment_at(3)->segment_type());
}

TEST(bind_type_after_entity_set)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(123)/MyNS.VIP"));
    
    VERIFY_ARE_EQUAL(3, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("123"));
    
    VERIFY_ARE_EQUAL(U("VIP"), path->segment_at(2)->as<odata_type_segment>()->type()->get_name());
}

TEST(bind_type_after_singleton)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/CEO/MyNS.VIP"));
    
    VERIFY_ARE_EQUAL(2, path->segments().size());
    
	verify_singleton_segment(path->segment_at(0), U("CEO"), U("Person"));
    
    VERIFY_ARE_EQUAL(U("VIP"), path->segment_at(1)->as<odata_type_segment>()->type()->get_name());
}

TEST(not_open_entity_type)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
    
	VERIFY_THROWS(parser.parse_path(U("/People(1)/NotDeclared")), odata_exception);
}

TEST(not_open_complex_type)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
    
	VERIFY_THROWS(parser.parse_path(U("/People(1)/Address/NotDeclared")), odata_exception);
}

TEST(bind_dynamic_property_on_open_entity_type)
{
    odata_uri_parser parser(get_test_model_with_open_person());
	auto path = parser.parse_path(U("/People(1)/NotDeclared"));
    
	VERIFY_ARE_EQUAL(3, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("1"));
    
	VERIFY_ARE_EQUAL(U("NotDeclared"), path->segment_at(2)->as<odata_dynamic_property_segment>()->property_name());
}

TEST(bind_dynamic_property_on_open_complex_type)
{
    odata_uri_parser parser(get_test_model_with_open_person());
	auto path = parser.parse_path(U("/People(1)/Address/NotDeclared"));
    
	VERIFY_ARE_EQUAL(4, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("1"));
    
    verify_structural_property_segment(path->segment_at(2), U("Person"), U("Address"));
    
	VERIFY_ARE_EQUAL(U("NotDeclared"), path->segment_at(3)->as<odata_dynamic_property_segment>()->property_name());
}

TEST(bind_multilevel_dynamic_property_on_open_type)
{
    odata_uri_parser parser(get_test_model_with_open_person());
	auto path = parser.parse_path(U("/People(1)/Address/NotDeclared1/NotDeclared2/NotDeclared3"));
    
	VERIFY_ARE_EQUAL(6, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("1"));
    
    verify_structural_property_segment(path->segment_at(2), U("Person"), U("Address"));
    
	VERIFY_ARE_EQUAL(U("NotDeclared1"), path->segment_at(3)->as<odata_dynamic_property_segment>()->property_name());
    
	VERIFY_ARE_EQUAL(U("NotDeclared2"), path->segment_at(4)->as<odata_dynamic_property_segment>()->property_name());
    
	VERIFY_ARE_EQUAL(U("NotDeclared3"), path->segment_at(5)->as<odata_dynamic_property_segment>()->property_name());
}

TEST(unexpected_parenthesis_expression_after_structural_property)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
    
	VERIFY_THROWS(parser.parse_path(U("/People(1)/ID(2)")), odata_exception);
}

static void verify_navigation_property_segment(
    std::shared_ptr<odata_path_segment> segment,
    const ::odata::utility::string_t &name,
    const ::odata::utility::string_t &type,
    const ::odata::utility::string_t &target)
{
    VERIFY_ARE_EQUAL(name, segment->as<odata_navigation_property_segment>()->property()->get_name());
	VERIFY_ARE_EQUAL(type, segment->as<odata_navigation_property_segment>()->navigation_type()->get_navigation_type()->get_name());
	VERIFY_ARE_EQUAL(target, segment->as<odata_navigation_property_segment>()->navigation_type()->get_binded_navigation_source()->get_name());
}

static void verify_navigation_property_segment(
    std::shared_ptr<odata_path_segment> segment,
    const ::odata::utility::string_t &name,
    const ::odata::utility::string_t &type)
{
    VERIFY_ARE_EQUAL(name, segment->as<odata_navigation_property_segment>()->property()->get_name());
	VERIFY_ARE_EQUAL(type, segment->as<odata_navigation_property_segment>()->navigation_type()->get_navigation_type()->get_name());
	VERIFY_IS_NULL(segment->as<odata_navigation_property_segment>()->navigation_type()->get_binded_navigation_source());
}

TEST(bind_single_value_navigation_property_without_key)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(1)/Parent"));
    
	VERIFY_ARE_EQUAL(3, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("1"));
    
	verify_navigation_property_segment(path->segment_at(2), U("Parent"), U("Person"), U("People"));
}

TEST(bind_navigation_property_without_key)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(1)/Mates"));
    
	VERIFY_ARE_EQUAL(3, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("1"));
    
    verify_navigation_property_segment(path->segment_at(2), U("Mates"), U("Collection(MyNS.Person)"), U("People"));
}

TEST(unexpected_parenthesis_expression_after_single_value_navigation_property)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
    
	VERIFY_THROWS(parser.parse_path(U("/People(1)/Parent(2)")), odata_exception);
}

TEST(bind_navigation_property_with_key)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(1)/Mates(3)"));
    
    VERIFY_ARE_EQUAL(4, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("1"));
    
    verify_navigation_property_segment(path->segment_at(2), U("Mates"), U("Collection(MyNS.Person)"), U("People"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(3)->as<odata_key_segment>()->keys().size());
    verify_key_segment(path->segment_at(3), U("ID"), U("3"));
}

TEST(no_navigation_source)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
    
	VERIFY_THROWS(parser.parse_path(U("/People(1)/Friends(3)")), odata_exception);
}

TEST(bind_contained_navigation_property_without_key)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(1)/Brothers"));
    
	VERIFY_ARE_EQUAL(3, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("1"));
    
    verify_navigation_property_segment(path->segment_at(2), U("Brothers"), U("Collection(MyNS.Person)"));
}

TEST(bind_contained_navigation_property_with_key)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(1)/Brothers(3)"));
    
    VERIFY_ARE_EQUAL(4, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("1"));
    
    verify_navigation_property_segment(path->segment_at(2), U("Brothers"), U("Collection(MyNS.Person)"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(3)->as<odata_key_segment>()->keys().size());
    verify_key_segment(path->segment_at(3), U("ID"), U("3"));
}

static void verify_operation_import_segment_parameter(std::shared_ptr<odata_path_segment> segment, const ::odata::utility::string_t &name, const ::odata::utility::string_t &value, ::size_t i = 0)
{
	VERIFY_ARE_EQUAL(name, segment->as<odata_operation_import_segment>()->parameter_at(i).first);
	VERIFY_ARE_EQUAL(value, segment->as<odata_operation_import_segment>()->parameter_at(i).second->to_string());
}

TEST(bind_operation_import_1)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/AddIntAndDouble(a=1,b=2)"));

	VERIFY_ARE_EQUAL(1, path->segments().size());

	VERIFY_ARE_EQUAL(U("AddIntAndDouble"), path->segment_at(0)->as<odata_operation_import_segment>()->operation_import()->get_name());
	VERIFY_ARE_EQUAL(2, path->segment_at(0)->as<odata_operation_import_segment>()->parameters().size());
	verify_operation_import_segment_parameter(path->segment_at(0), U("a"), U("1"), 0);
	verify_operation_import_segment_parameter(path->segment_at(0),  U("b"),  U("2"), 1);
}

TEST(bind_operation_import_2)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/AddIntAndDouble(b=1,a=2)"));

	VERIFY_ARE_EQUAL(1, path->segments().size());

	VERIFY_ARE_EQUAL(U("AddIntAndDouble"), path->segment_at(0)->as<odata_operation_import_segment>()->operation_import()->get_name());
	VERIFY_ARE_EQUAL(2, path->segment_at(0)->as<odata_operation_import_segment>()->parameters().size());
	verify_operation_import_segment_parameter(path->segment_at(0), U("b"), U("1"), 0);
	verify_operation_import_segment_parameter(path->segment_at(0),  U("a"),  U("2"), 1);
}

TEST(parameter_count_mismatch_1)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/AddIntAndDouble(a=2)")), odata_exception);
}

TEST(parameter_count_mismatch_2)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/AddIntAndDouble(a=2,b=3,c=4)")), odata_exception);
}

TEST(duplicate_parameter)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/AddIntAndDouble(a=2,a=1)")), odata_exception);
}

TEST(parameter_name_mismatch)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/AddIntAndDouble(a=2,c=1)")), odata_exception);
}

TEST(missing_parameter_value)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/AddIntAndDouble(a=2,c=)")), odata_exception);
}

TEST(missing_parameter_name)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/AddIntAndDouble(a=2,=1)")), odata_exception);
}

TEST(missing_parenthesis_in_parameter)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/AddIntAndDouble(a=2,b=1")), odata_exception);
}

TEST(bind_bound_operation_import)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/AddIntAndDouble1(a=2,b=1)")), odata_exception);
}

TEST(bind_operation_to_entity)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(1)/MyNS.GetFavNum"));
    
	VERIFY_ARE_EQUAL(3, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("1"));
    
	VERIFY_ARE_EQUAL(U("GetFavNum"), path->segment_at(2)->as<odata_operation_segment>()->operation()->get_name());
	VERIFY_IS_TRUE(path->segment_at(2)->as<odata_operation_segment>()->parameters().empty());
}

TEST(parameter_mismatch_for_bound_operation)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/People(1)/MyNS.GetFavNum(a=1)")), odata_exception);
}

TEST(bind_operation_to_primitive)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(1)/ID/MyNS.AddOne"));
    
	VERIFY_ARE_EQUAL(4, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("1"));

	verify_structural_property_segment(path->segment_at(2), U("Person"), U("ID"));
    
	VERIFY_ARE_EQUAL(U("AddOne"), path->segment_at(3)->as<odata_operation_segment>()->operation()->get_name());
	VERIFY_IS_TRUE(path->segment_at(3)->as<odata_operation_segment>()->parameters().empty());
}

static void verify_operation_segment_parameter(std::shared_ptr<odata_path_segment> segment, const ::odata::utility::string_t &name, const ::odata::utility::string_t &value, ::size_t i = 0)
{
	VERIFY_ARE_EQUAL(name, segment->as<odata_operation_segment>()->parameter_at(i).first);
	VERIFY_ARE_EQUAL(value, segment->as<odata_operation_segment>()->parameter_at(i).second->to_string());
}

TEST(bind_operation_to_primitive_1)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(1)/ID/MyNS.AddWith(y=1)"));
    
	VERIFY_ARE_EQUAL(4, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("1"));

	verify_structural_property_segment(path->segment_at(2), U("Person"), U("ID"));
    
	VERIFY_ARE_EQUAL(U("AddWith"), path->segment_at(3)->as<odata_operation_segment>()->operation()->get_name());
	VERIFY_ARE_EQUAL(1, path->segment_at(3)->as<odata_operation_segment>()->parameters().size());
	verify_operation_segment_parameter(path->segment_at(3), U("y"), U("1"));
}

TEST(parameter_mismatch_for_bound_operation_1)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/People(1)/ID/MyNS.AddWith()")), odata_exception);
}

TEST(parameter_mismatch_for_bound_operation_2)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/People(1)/ID/MyNS.AddWith(y=1,z=2)")), odata_exception);
}

TEST(bind_operation_to_complex)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(1)/Address/MyNS.GetCountry"));
    
	VERIFY_ARE_EQUAL(4, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("1"));

	verify_structural_property_segment(path->segment_at(2), U("Person"), U("Address"));
    
	VERIFY_ARE_EQUAL(U("GetCountry"), path->segment_at(3)->as<odata_operation_segment>()->operation()->get_name());
	VERIFY_IS_TRUE(path->segment_at(3)->as<odata_operation_segment>()->parameters().empty());
}

TEST(binding_type_mismatch)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/People(1)/ID/MyNS.GetFavNum")), odata_exception);
}

TEST(compose_bound_operations)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(1)/ID/MyNS.AddOne/MyNS.AddOne"));
    
	VERIFY_ARE_EQUAL(5, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("1"));

	verify_structural_property_segment(path->segment_at(2), U("Person"), U("ID"));
    
	VERIFY_ARE_EQUAL(U("AddOne"), path->segment_at(3)->as<odata_operation_segment>()->operation()->get_name());
	VERIFY_IS_TRUE(path->segment_at(3)->as<odata_operation_segment>()->parameters().empty());

	VERIFY_ARE_EQUAL(U("AddOne"), path->segment_at(4)->as<odata_operation_segment>()->operation()->get_name());
	VERIFY_IS_TRUE(path->segment_at(4)->as<odata_operation_segment>()->parameters().empty());
}

TEST(compose_bound_operation_with_unbound_one)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/AddIntAndDouble(a=1,b=2)/MyNS.AddOne"));

	VERIFY_ARE_EQUAL(2, path->segments().size());

	VERIFY_ARE_EQUAL(U("AddIntAndDouble"), path->segment_at(0)->as<odata_operation_import_segment>()->operation_import()->get_name());
	VERIFY_ARE_EQUAL(2, path->segment_at(0)->as<odata_operation_import_segment>()->parameters().size());
	verify_operation_import_segment_parameter(path->segment_at(0), U("a"), U("1"), 0);
	verify_operation_import_segment_parameter(path->segment_at(0),  U("b"),  U("2"), 1);

	VERIFY_ARE_EQUAL(U("AddOne"), path->segment_at(1)->as<odata_operation_segment>()->operation()->get_name());
	VERIFY_IS_TRUE(path->segment_at(1)->as<odata_operation_segment>()->parameters().empty());
}

TEST(compose_noncomposable_bound_operation)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/People(1)/ID/MyNS.AddTwo/MyNS.AddOne")), odata_exception);
}

TEST(compose_noncomposable_unbound_operation_import)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/AddIntAndDouble2(a=1,b=2)/MyNS.AddOne")), odata_exception);
}

TEST(segment_follow_metadata)
{
	odata_uri_parser parser(std::make_shared<edm_model>());

	VERIFY_THROWS(parser.parse_path(U("/$metadata/MyNS.AddOne")), odata_exception);
}

TEST(segment_follow_value)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/People(1)/ID/$value/MyNS.AddOne")), odata_exception);
}

TEST(segment_follow_batch)
{
	odata_uri_parser parser(std::make_shared<edm_model>());

	VERIFY_THROWS(parser.parse_path(U("/$batch/MyNS.AddOne")), odata_exception);
}

TEST(segment_follow_count)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/People/$count/MyNS.AddOne")), odata_exception);
}

TEST(segment_follow_ref)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());

	VERIFY_THROWS(parser.parse_path(U("/People(1)/$ref/MyNS.AddOne")), odata_exception);
}

TEST(count_after_entity_set)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People/$count"));
    
    VERIFY_ARE_EQUAL(2, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(::odata::core::odata_path_segment_type::Count, path->segment_at(1)->as<odata_count_segment>()->segment_type());
}

TEST(ref_after_entity)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto path = parser.parse_path(U("/People(1)/$ref"));
    
	VERIFY_ARE_EQUAL(3, path->segments().size());
    
	verify_entity_set_segment(path->segment_at(0), U("People"), U("Person"));
    
	VERIFY_ARE_EQUAL(1, path->segment_at(1)->as<odata_key_segment>()->keys().size());
	verify_key_segment(path->segment_at(1), U("ID"), U("1"));

	VERIFY_ARE_EQUAL(::odata::core::odata_path_segment_type::Ref, path->segment_at(2)->as<odata_ref_segment>()->segment_type());
}

TEST(batch_on_path_root)
{
	odata_uri_parser parser(std::make_shared<edm_model>());
	auto path = parser.parse_path(U("/$batch"));
    
	VERIFY_ARE_EQUAL(1, path->segments().size());

	VERIFY_ARE_EQUAL(::odata::core::odata_path_segment_type::Batch, path->segment_at(0)->as<odata_batch_segment>()->segment_type());
}

TEST(metadata_on_path_root)
{
	odata_uri_parser parser(std::make_shared<edm_model>());
	auto path = parser.parse_path(U("/$metadata"));
    
	VERIFY_ARE_EQUAL(1, path->segments().size());

	VERIFY_ARE_EQUAL(::odata::core::odata_path_segment_type::Metadata, path->segment_at(0)->as<odata_metadata_segment>()->segment_type());
}

// --END-- odata_path_parser

// --BEGIN-- odata_expression_lexer

TEST(scan_guid_literal)
{
	auto token = odata_expression_lexer::create_lexer(U("01234567-89ab-cdef-0123-456789abcdef"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::GuidLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("01234567-89ab-cdef-0123-456789abcdef"), token->text());
}

TEST(scan_guid_literal_start_with_letter)
{
	auto token = odata_expression_lexer::create_lexer(U("a1234567-89ab-cdef-0123-456789abcdef"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::GuidLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("a1234567-89ab-cdef-0123-456789abcdef"), token->text());
}

TEST(scan_guid_literal_negative)
{
	auto token = odata_expression_lexer::create_lexer(U("-01234567-89ab-cdef-0123-456789abcdef"))->current_token();

	VERIFY_ARE_NOT_EQUAL(odata_expression_token_kind::GuidLiteral, token->token_kind());
}

TEST(scan_guid_literal_wrong_char)
{
	auto token = odata_expression_lexer::create_lexer(U("01234g67-89ab-cdef-0123-456789abcdef"))->current_token();

	VERIFY_ARE_NOT_EQUAL(odata_expression_token_kind::GuidLiteral, token->token_kind());
}

TEST(scan_guid_literal_wrong_length)
{
	auto token = odata_expression_lexer::create_lexer(U("01234567-89ab-cdef-456789abcdef"))->current_token();

	VERIFY_ARE_NOT_EQUAL(odata_expression_token_kind::GuidLiteral, token->token_kind());
}

TEST(scan_datetimeoffset_no_seconds)
{
	auto token = odata_expression_lexer::create_lexer(U("2012-09-03T13:52Z"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::DateTimeOffsetLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("2012-09-03T13:52Z"), token->text());
}

TEST(scan_datetimeoffset_seconds)
{
	auto token = odata_expression_lexer::create_lexer(U("2012-09-03T08:09:02Z"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::DateTimeOffsetLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("2012-09-03T08:09:02Z"), token->text());
}

TEST(scan_datetimeoffset_frac_seconds)
{
	auto token = odata_expression_lexer::create_lexer(U("2012-08-31T18:19:22.1Z"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::DateTimeOffsetLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("2012-08-31T18:19:22.1Z"), token->text());
}

TEST(scan_datetimeoffset_year_zero)
{
	auto token = odata_expression_lexer::create_lexer(U("0000-01-01T00:00Z"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::DateTimeOffsetLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("0000-01-01T00:00Z"), token->text());
}

TEST(scan_datetimeoffset_negative)
{
	auto token = odata_expression_lexer::create_lexer(U("-10000-04-01T00:00Z"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::DateTimeOffsetLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("-10000-04-01T00:00Z"), token->text());
}

TEST(scan_datetimeoffset_tz2)
{
	auto token = odata_expression_lexer::create_lexer(U("2012-09-03T14:53+02:00"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::DateTimeOffsetLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("2012-09-03T14:53+02:00"), token->text());
}

TEST(scan_datetimeoffset_tzn2)
{
	auto token = odata_expression_lexer::create_lexer(U("2012-09-03T14:53-02:00"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::DateTimeOffsetLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("2012-09-03T14:53-02:00"), token->text());
}

TEST(scan_datetimeoffset_wrong_year)
{
	auto token = odata_expression_lexer::create_lexer(U("201-09-03T14:53-02:00"))->current_token();

	VERIFY_ARE_NOT_EQUAL(odata_expression_token_kind::DateTimeOffsetLiteral, token->token_kind());
}

TEST(scan_datetimeoffset_wrong_month)
{
	auto token = odata_expression_lexer::create_lexer(U("2012-091-03T14:53-02:00"))->current_token();

	VERIFY_ARE_NOT_EQUAL(odata_expression_token_kind::DateTimeOffsetLiteral, token->token_kind());
}

TEST(scan_datetimeoffset_wrong_hour)
{
	auto token = odata_expression_lexer::create_lexer(U("2012-09-03T1:53-02:00"))->current_token();

	VERIFY_ARE_NOT_EQUAL(odata_expression_token_kind::DateTimeOffsetLiteral, token->token_kind());
}

TEST(scan_datetimeoffset_wrong_format)
{
	auto token = odata_expression_lexer::create_lexer(U("2012-09-03T14::53-02:00"))->current_token();

	VERIFY_ARE_NOT_EQUAL(odata_expression_token_kind::DateTimeOffsetLiteral, token->token_kind());
}

TEST(scan_datetimeoffset_wrong_tz)
{
	auto token = odata_expression_lexer::create_lexer(U("2012-09-03T14:53-2:00"))->current_token();

	VERIFY_ARE_NOT_EQUAL(odata_expression_token_kind::DateTimeOffsetLiteral, token->token_kind());
}

TEST(scan_int64_max)
{
	auto token = odata_expression_lexer::create_lexer(U("9223372036854775807"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::Int64Literal, token->token_kind());
	VERIFY_ARE_EQUAL(U("9223372036854775807"), token->text());
}

TEST(scan_int64_min)
{
	auto token = odata_expression_lexer::create_lexer(U("-9223372036854775808"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::Int64Literal, token->token_kind());
	VERIFY_ARE_EQUAL(U("-9223372036854775808"), token->text());
}

TEST(scan_int32_max)
{
	auto token = odata_expression_lexer::create_lexer(U("2147483647"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::Int32Literal, token->token_kind());
	VERIFY_ARE_EQUAL(U("2147483647"), token->text());
}

TEST(scan_int32_min)
{
	auto token = odata_expression_lexer::create_lexer(U("-2147483648"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::Int32Literal, token->token_kind());
	VERIFY_ARE_EQUAL(U("-2147483648"), token->text());
}

TEST(scan_int32)
{
	auto token = odata_expression_lexer::create_lexer(U("10"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::Int32Literal, token->token_kind());
	VERIFY_ARE_EQUAL(U("10"), token->text());
}

TEST(scan_decimal)
{
	auto token = odata_expression_lexer::create_lexer(U("1.175"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::DecimalLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("1.175"), token->text());
}

TEST(scan_single_min)
{
	auto token = odata_expression_lexer::create_lexer(U("1.175494351e-38"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::SingleLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("1.175494351e-38"), token->text());
}

TEST(scan_single_max)
{
	auto token = odata_expression_lexer::create_lexer(U("3.402823466e+38"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::SingleLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("3.402823466e+38"), token->text());
}

TEST(scan_double_min)
{
	auto token = odata_expression_lexer::create_lexer(U("2.2250738585072014e-308"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::DoubleLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("2.2250738585072014e-308"), token->text());
}

TEST(scan_double_max)
{
	auto token = odata_expression_lexer::create_lexer(U("1.7976931348623158e+308"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::DoubleLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("1.7976931348623158e+308"), token->text());
}

TEST(scan_double_inf)
{
	auto token = odata_expression_lexer::create_lexer(U("INF"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::DoubleLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("INF"), token->text());
}

TEST(scan_double_negative_inf)
{
	auto token = odata_expression_lexer::create_lexer(U("-INF"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::DoubleLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("-INF"), token->text());
}

TEST(scan_double_nan)
{
	auto token = odata_expression_lexer::create_lexer(U("NaN"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::DoubleLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("NaN"), token->text());
}

TEST(scan_true)
{
	auto token = odata_expression_lexer::create_lexer(U("true"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::BooleanLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("true"), token->text());
}

TEST(scan_false)
{
	auto token = odata_expression_lexer::create_lexer(U("false"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::BooleanLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("false"), token->text());
}

TEST(scan_null)
{
	auto token = odata_expression_lexer::create_lexer(U("null"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::NullLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("null"), token->text());
}

TEST(scan_duration_literal)
{
	auto token = odata_expression_lexer::create_lexer(U("duration'P6DT23H59M59.9999S'"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::DurationLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("duration'P6DT23H59M59.9999S'"), token->text());
}

TEST(scan_binary_literal)
{
	auto token = odata_expression_lexer::create_lexer(U("binary'Zm9vYmE='"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::BinaryLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("binary'Zm9vYmE='"), token->text());
}

TEST(scan_quoted_literal)
{
	auto token = odata_expression_lexer::create_lexer(U("geography'SRID=0;LineString(142.1 64.1,3.14 2.78)'"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::QuotedLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("geography'SRID=0;LineString(142.1 64.1,3.14 2.78)'"), token->text());
}

TEST(scan_string_literal)
{
	auto token = odata_expression_lexer::create_lexer(U("'abc'"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::StringLiteral, token->token_kind());
	VERIFY_ARE_EQUAL(U("'abc'"), token->text());
	VERIFY_ARE_EQUAL(U("abc"), token->to_primitive_value()->to_string());
}

TEST(scan_parameter_alias)
{
	auto token = odata_expression_lexer::create_lexer(U("@word"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::ParameterAlias, token->token_kind());
	VERIFY_ARE_EQUAL(U("@word"), token->text());
}

TEST(scan_json_object)
{
	auto token = odata_expression_lexer::create_lexer(U("{\"Name\":\"Value\"}"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::JsonObjectOrArray, token->token_kind());
	VERIFY_ARE_EQUAL(U("{\"Name\":\"Value\"}"), token->text());
}

TEST(scan_json_array)
{
	auto token = odata_expression_lexer::create_lexer(U("[1, 2,3]"))->current_token();

	VERIFY_ARE_EQUAL(odata_expression_token_kind::JsonObjectOrArray, token->token_kind());
	VERIFY_ARE_EQUAL(U("[1, 2,3]"), token->text());
}

// --END-- odata_expression_lexer

// --BEGIN-- odata_expression_parser

static void verify_is_operator_or_node(std::shared_ptr<odata_query_node> node)
{
	VERIFY_ARE_EQUAL(odata_query_node_kind::BinaryOperator, node->node_kind());
	VERIFY_ARE_EQUAL(binary_operator_kind::Or, node->as<odata_binary_operator_node>()->operator_kind());
}
    
static void verify_is_operator_and_node(std::shared_ptr<odata_query_node> node)
{
    VERIFY_ARE_EQUAL(odata_query_node_kind::BinaryOperator, node->node_kind());
    VERIFY_ARE_EQUAL(binary_operator_kind::And, node->as<odata_binary_operator_node>()->operator_kind());
}

static void verify_is_true(std::shared_ptr<odata_query_node> node)
{
	VERIFY_ARE_EQUAL(odata_query_node_kind::Constant, node->node_kind());
	VERIFY_ARE_EQUAL(::odata::edm::edm_primitive_type::BOOLEAN(), node->as<odata_constant_node>()->value()->get_value_type());
	VERIFY_ARE_EQUAL(U("true"), node->as<odata_constant_node>()->value()->to_string());
}

static void verify_is_false(std::shared_ptr<odata_query_node> node)
{
	VERIFY_ARE_EQUAL(odata_query_node_kind::Constant, node->node_kind());
	VERIFY_ARE_EQUAL(::odata::edm::edm_primitive_type::BOOLEAN(), node->as<odata_constant_node>()->value()->get_value_type());
	VERIFY_ARE_EQUAL(U("false"), node->as<odata_constant_node>()->value()->to_string());
}

TEST(parse_logical_or)
{
	auto node = odata_expression_parser::parse_expression(U("true or false or true"));
    
	verify_is_operator_or_node(node);
    
	verify_is_true(node->as<odata_binary_operator_node>()->right());
    
	auto node1 = node->as<odata_binary_operator_node>()->left();
	verify_is_operator_or_node(node1);
	verify_is_true(node1->as<odata_binary_operator_node>()->left());
	verify_is_false(node1->as<odata_binary_operator_node>()->right());
}
    
TEST(parse_logical_and)
{
    auto node = odata_expression_parser::parse_expression(U("true and false and true"));
    
    verify_is_operator_and_node(node);
    
    verify_is_true(node->as<odata_binary_operator_node>()->right());
    
    auto node1 = node->as<odata_binary_operator_node>()->left();
    verify_is_operator_and_node(node1);
    verify_is_true(node1->as<odata_binary_operator_node>()->left());
    verify_is_false(node1->as<odata_binary_operator_node>()->right());
}
    
TEST(parse_logical_and_or)
{
    auto node = odata_expression_parser::parse_expression(U("true or false and true"));
    
    verify_is_operator_or_node(node);
    
    verify_is_true(node->as<odata_binary_operator_node>()->left());
    
    auto node1 = node->as<odata_binary_operator_node>()->right();
    verify_is_operator_and_node(node1);
    verify_is_false(node1->as<odata_binary_operator_node>()->left());
    verify_is_true(node1->as<odata_binary_operator_node>()->right());
}
    
static void verify_is_operator_not_node(std::shared_ptr<odata_query_node> node)
{
    VERIFY_ARE_EQUAL(odata_query_node_kind::UnaryOperator, node->node_kind());
    VERIFY_ARE_EQUAL(unary_operator_kind::Not, node->as<odata_unary_operator_node>()->operator_kind());
}
    
TEST(parse_logical_not)
{
    auto node = odata_expression_parser::parse_expression(U("true or not false"));
    
    verify_is_operator_or_node(node);
    
    verify_is_true(node->as<odata_binary_operator_node>()->left());
    
    auto node1 = node->as<odata_binary_operator_node>()->right();
    verify_is_operator_not_node(node1);
    verify_is_false(node1->as<odata_unary_operator_node>()->operand());
}

static void verify_is_operator_lt_node(std::shared_ptr<odata_query_node> node)
{
    VERIFY_ARE_EQUAL(odata_query_node_kind::BinaryOperator, node->node_kind());
    VERIFY_ARE_EQUAL(binary_operator_kind::LessThan, node->as<odata_binary_operator_node>()->operator_kind());
}

static void verify_is_int(std::shared_ptr<odata_query_node> node, const ::odata::utility::string_t &str)
{
	VERIFY_ARE_EQUAL(odata_query_node_kind::Constant, node->node_kind());
	VERIFY_ARE_EQUAL(::odata::edm::edm_primitive_type::INT32(), node->as<odata_constant_node>()->value()->get_value_type());
	VERIFY_ARE_EQUAL(str, node->as<odata_constant_node>()->value()->to_string());
}

TEST(parse_comparsion)
{
    auto node = odata_expression_parser::parse_expression(U("1 lt 2"));
    
    verify_is_operator_lt_node(node);
    
    verify_is_int(node->as<odata_binary_operator_node>()->left(), U("1"));
    verify_is_int(node->as<odata_binary_operator_node>()->right(), U("2"));
}

static void verify_is_operator_add_node(std::shared_ptr<odata_query_node> node)
{
    VERIFY_ARE_EQUAL(odata_query_node_kind::BinaryOperator, node->node_kind());
    VERIFY_ARE_EQUAL(binary_operator_kind::Add, node->as<odata_binary_operator_node>()->operator_kind());
}

static void verify_is_operator_mul_node(std::shared_ptr<odata_query_node> node)
{
    VERIFY_ARE_EQUAL(odata_query_node_kind::BinaryOperator, node->node_kind());
    VERIFY_ARE_EQUAL(binary_operator_kind::Multiply, node->as<odata_binary_operator_node>()->operator_kind());
}

static void verify_is_operator_neg_node(std::shared_ptr<odata_query_node> node)
{
    VERIFY_ARE_EQUAL(odata_query_node_kind::UnaryOperator, node->node_kind());
    VERIFY_ARE_EQUAL(unary_operator_kind::Negate, node->as<odata_unary_operator_node>()->operator_kind());
}

TEST(parse_add_mul_neg)
{
    auto node = odata_expression_parser::parse_expression(U("1 mul 2 add 2 mul - 3"));
    
    verify_is_operator_add_node(node);
    
    auto node1 = node->as<odata_binary_operator_node>()->left();
    verify_is_operator_mul_node(node1);
    verify_is_int(node1->as<odata_binary_operator_node>()->left(), U("1"));
    verify_is_int(node1->as<odata_binary_operator_node>()->right(), U("2"));
    
    auto node2 = node->as<odata_binary_operator_node>()->right();
    verify_is_operator_mul_node(node2);
    verify_is_int(node2->as<odata_binary_operator_node>()->left(), U("2"));
    
    auto node3 = node2->as<odata_binary_operator_node>()->right();
    verify_is_operator_neg_node(node3);
    verify_is_int(node3->as<odata_unary_operator_node>()->operand(), U("3"));
}

TEST(parse_parenthesis)
{
    auto node = odata_expression_parser::parse_expression(U("(1 add 2) mul (2 add 3)"));
    
    verify_is_operator_mul_node(node);
    
    auto node1 = node->as<odata_binary_operator_node>()->left();
    verify_is_operator_add_node(node1);
    verify_is_int(node1->as<odata_binary_operator_node>()->left(), U("1"));
    verify_is_int(node1->as<odata_binary_operator_node>()->right(), U("2"));
    
    auto node2 = node->as<odata_binary_operator_node>()->right();
    verify_is_operator_add_node(node2);
    verify_is_int(node2->as<odata_binary_operator_node>()->left(), U("2"));
    verify_is_int(node2->as<odata_binary_operator_node>()->right(), U("3"));
}

static void verify_is_property_access_node(std::shared_ptr<odata_query_node> node, const ::odata::utility::string_t &text)
{
	VERIFY_ARE_EQUAL(odata_query_node_kind::PropertyAccess, node->node_kind());
	VERIFY_ARE_EQUAL(text, node->as<odata_property_access_node>()->property_name());
}

static void verify_is_type_cast_node(std::shared_ptr<odata_query_node> node, const ::odata::utility::string_t &text)
{
	VERIFY_ARE_EQUAL(odata_query_node_kind::TypeCast, node->node_kind());
	VERIFY_ARE_EQUAL(text, node->as<odata_type_cast_node>()->type_name());
}

TEST(parse_segment)
{
    auto node = odata_expression_parser::parse_expression(U("Items"));
    
    verify_is_property_access_node(node, U("Items"));
}

TEST(parse_segments)
{
    auto node = odata_expression_parser::parse_expression(U("Item/Quantity"));
    
    verify_is_property_access_node(node, U("Quantity"));
    VERIFY_ARE_EQUAL(U("Item"), node->as<odata_property_access_node>()->parent()->as<odata_property_access_node>()->property_name());
}

TEST(parse_type_cast)
{
    auto node = odata_expression_parser::parse_expression(U("Item/MyNS.ConcreteItem"));
    
    verify_is_type_cast_node(node, U("MyNS.ConcreteItem"));
    VERIFY_ARE_EQUAL(U("Item"), node->as<odata_property_access_node>()->parent()->as<odata_type_cast_node>()->type_name());
}

static void verify_is_operator_gt_node(std::shared_ptr<odata_query_node> node)
{
    VERIFY_ARE_EQUAL(odata_query_node_kind::BinaryOperator, node->node_kind());
    VERIFY_ARE_EQUAL(binary_operator_kind::GreaterThan, node->as<odata_binary_operator_node>()->operator_kind());
}

static void verify_is_range_variable_node(std::shared_ptr<odata_query_node> node, const ::odata::utility::string_t &name)
{
	VERIFY_ARE_EQUAL(odata_query_node_kind::RangeVariable, node->node_kind());
    VERIFY_ARE_EQUAL(name, node->as<odata_range_variable_node>()->name());
}

TEST(parse_lambda)
{
    auto node = odata_expression_parser::parse_expression(U("Items/any(d:d/Quantity gt 100)"));
    
    VERIFY_ARE_EQUAL(odata_query_node_kind::Lambda, node->node_kind());
	
	auto node1 = node->as<odata_lambda_node>();
	VERIFY_IS_TRUE(node1->is_any());
	VERIFY_ARE_EQUAL(U("d"), node1->parameter());

	auto node2 = node1->expression();
	verify_is_operator_gt_node(node2);

	auto node3 = node2->as<odata_binary_operator_node>()->left();
	verify_is_property_access_node(node3, U("Quantity"));
	verify_is_range_variable_node(node3->as<odata_property_access_node>()->parent(), U("d"));

	auto node4 = node2->as<odata_binary_operator_node>()->right();
	verify_is_int(node4, U("100"));
}

TEST(duplicate_lambda_parameter)
{
    VERIFY_THROWS(odata_expression_parser::parse_expression(U("Items/any(d:d/Quantity gt 100 and Items/any(d:d/Quantity gt 100))")), odata_exception);
}

TEST(parse_multiple_lambda)
{
    auto node = odata_expression_parser::parse_expression(U("Items/any(d:d/Quantity gt 100) and Items/all(d:d/Quantity lt 100)"));
    
    verify_is_operator_and_node(node);
    
    auto node_left = node->as<odata_binary_operator_node>()->left();
    
    VERIFY_ARE_EQUAL(odata_query_node_kind::Lambda, node_left->node_kind());
	
	auto node1 = node_left->as<odata_lambda_node>();
	VERIFY_IS_TRUE(node1->is_any());
	VERIFY_ARE_EQUAL(U("d"), node1->parameter());

	auto node2 = node1->expression();
	verify_is_operator_gt_node(node2);

	auto node3 = node2->as<odata_binary_operator_node>()->left();
	verify_is_property_access_node(node3, U("Quantity"));
	verify_is_range_variable_node(node3->as<odata_property_access_node>()->parent(), U("d"));

	auto node4 = node2->as<odata_binary_operator_node>()->right();
	verify_is_int(node4, U("100"));
    
    auto node_right = node->as<odata_binary_operator_node>()->right();
    
    VERIFY_ARE_EQUAL(odata_query_node_kind::Lambda, node_right->node_kind());
	
	auto node5 = node_right->as<odata_lambda_node>();
	VERIFY_IS_FALSE(node5->is_any());
	VERIFY_ARE_EQUAL(U("d"), node1->parameter());

	auto node6 = node5->expression();
	verify_is_operator_lt_node(node6);

	auto node7 = node6->as<odata_binary_operator_node>()->left();
	verify_is_property_access_node(node7, U("Quantity"));
	verify_is_range_variable_node(node7->as<odata_property_access_node>()->parent(), U("d"));

	auto node8 = node6->as<odata_binary_operator_node>()->right();
	verify_is_int(node8, U("100"));
}

static void verify_is_function_call_node(std::shared_ptr<odata_query_node> node, const ::odata::utility::string_t &name)
{
    VERIFY_ARE_EQUAL(odata_query_node_kind::FunctionCall, node->node_kind());
    VERIFY_ARE_EQUAL(name, node->as<odata_function_call_node>()->name());
}

static void verify_function_call_parameter(
    std::shared_ptr<odata_query_node> node,
    ::size_t i,
    const ::odata::utility::string_t &name,
    int value_kind)
{
    VERIFY_ARE_EQUAL(name, node->as<odata_function_call_node>()->parameter_at(i).first);
    VERIFY_ARE_EQUAL(value_kind, node->as<odata_function_call_node>()->parameter_at(i).second->node_kind());
}

static void verify_is_string(std::shared_ptr<odata_query_node> node, const ::odata::utility::string_t &str)
{
	VERIFY_ARE_EQUAL(odata_query_node_kind::Constant, node->node_kind());
	VERIFY_ARE_EQUAL(::odata::edm::edm_primitive_type::STRING(), node->as<odata_constant_node>()->value()->get_value_type());
	VERIFY_ARE_EQUAL(str, node->as<odata_constant_node>()->value()->to_string());
}

TEST(parse_canonical_function)
{
    auto node = odata_expression_parser::parse_expression(U("contains(CompanyName,'Alfreds')"));
    verify_is_function_call_node(node, U("contains"));
	verify_function_call_parameter(node, 0, U(""), odata_query_node_kind::PropertyAccess);
    verify_function_call_parameter(node, 1, U(""), odata_query_node_kind::Constant);
    verify_is_string(node->as<odata_function_call_node>()->parameter_at(1).second, U("Alfreds"));
}

TEST(parse_bound_operation)
{
    auto node = odata_expression_parser::parse_expression(U("MyNS.GetFavouriteColors(ID=1,Name='Foo')"));
    verify_is_function_call_node(node, U("MyNS.GetFavouriteColors"));
    verify_function_call_parameter(node, 0, U("ID"), odata_query_node_kind::Constant);
    verify_is_int(node->as<odata_function_call_node>()->parameter_at(0).second, U("1"));
    verify_function_call_parameter(node, 1, U("Name"), odata_query_node_kind::Constant);
    verify_is_string(node->as<odata_function_call_node>()->parameter_at(1).second, U("Foo"));
}

// --END-- odata_expression_parser

// --BEGIN-- odata_query_option_parser

TEST(parse_top_empty)
{
	odata_uri_parser parser(std::make_shared<edm_model>());
	auto top = parser.parse_top(::odata::utility::string_t());
    
	VERIFY_IS_TRUE(null_value == top);
}

TEST(parse_top_value)
{
	odata_uri_parser parser(std::make_shared<edm_model>());
	auto top = parser.parse_top(U("12345678901234"));
    
	VERIFY_ARE_EQUAL((int64_t)12345678901234LL, top);
}

TEST(parse_skip_empty)
{
	odata_uri_parser parser(std::make_shared<edm_model>());
	auto skip = parser.parse_skip(::odata::utility::string_t());
    
	VERIFY_IS_TRUE(null_value == skip);
}

TEST(parse_skip_value)
{
	odata_uri_parser parser(std::make_shared<edm_model>());
	auto skip = parser.parse_skip(U("12345678901234"));
    
	VERIFY_ARE_EQUAL((int64_t)12345678901234LL, skip);
}

TEST(parse_count_empty)
{
	odata_uri_parser parser(std::make_shared<edm_model>());
	auto count = parser.parse_count(::odata::utility::string_t());
    
	VERIFY_IS_TRUE(nullptr == count);
}

TEST(parse_count_true)
{
	odata_uri_parser parser(std::make_shared<edm_model>());
	auto count = parser.parse_count(U("true"));
    
	VERIFY_IS_TRUE(count);
}

TEST(parse_count_false)
{
	odata_uri_parser parser(std::make_shared<edm_model>());
	auto count = parser.parse_count(U("false"));
    
	VERIFY_IS_FALSE(count);
}

TEST(parse_invalid_count)
{
	odata_uri_parser parser(std::make_shared<edm_model>());

	VERIFY_THROWS(parser.parse_count(U("abc")), odata_exception);
}

TEST(parse_select_expand_empty)
{
    odata_uri_parser parser(std::make_shared<edm_model>());
    auto select_expand_clause = parser.parse_select_and_expand(::odata::utility::string_t(), ::odata::utility::string_t());
    
    VERIFY_IS_TRUE(nullptr == select_expand_clause);
}

TEST(parse_filter_empty)
{
    odata_uri_parser parser(std::make_shared<edm_model>());
    auto filter_clause = parser.parse_filter(::odata::utility::string_t());
    
    VERIFY_IS_TRUE(nullptr == filter_clause);
}

TEST(parse_orderby_empty)
{
    odata_uri_parser parser(std::make_shared<edm_model>());
    auto orderby_clause = parser.parse_orderby(::odata::utility::string_t());
    
    VERIFY_IS_TRUE(nullptr == orderby_clause);
}

TEST(parse_search_empty)
{
    odata_uri_parser parser(std::make_shared<edm_model>());
    auto search_clause = parser.parse_search(::odata::utility::string_t());
    
    VERIFY_IS_TRUE(nullptr == search_clause);
}

static void verify_range_variable(std::shared_ptr<odata_range_variable> range_variable, const ::odata::utility::string_t &name, const ::odata::utility::string_t &type)
{
	VERIFY_ARE_EQUAL(name, range_variable->name());
	VERIFY_ARE_EQUAL(type, range_variable->target_type()->get_full_name());
    VERIFY_IS_TRUE(nullptr == range_variable->target_navigation_source());
}

static void verify_range_variable(std::shared_ptr<odata_range_variable> range_variable, const ::odata::utility::string_t &name, const ::odata::utility::string_t &type, const ::odata::utility::string_t &source)
{
	VERIFY_ARE_EQUAL(name, range_variable->name());
	VERIFY_ARE_EQUAL(type, range_variable->target_type()->get_full_name());
    VERIFY_ARE_EQUAL(source, range_variable->target_navigation_source()->get_name());
}

TEST(parse_filter_after_entity_set)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People?$filter=ID gt 1"));
	auto parsed_uri = parser.parse_uri(uri);
	auto filter_clause = parsed_uri->filter_clause();

	verify_is_operator_gt_node(filter_clause->expression());
	verify_range_variable(filter_clause->range_variable(), U("$it"), U("MyNS.Person"), U("People"));
}

TEST(parse_filter_after_collection_navigation_property)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)/Mates?$filter=ID gt 1"));
	auto parsed_uri = parser.parse_uri(uri);
	auto filter_clause = parsed_uri->filter_clause();

	verify_is_operator_gt_node(filter_clause->expression());
	verify_range_variable(filter_clause->range_variable(), U("$it"), U("MyNS.Person"), U("People"));
}

static void verify_is_operator_eq_node(std::shared_ptr<odata_query_node> node)
{
    VERIFY_ARE_EQUAL(odata_query_node_kind::BinaryOperator, node->node_kind());
	VERIFY_ARE_EQUAL(binary_operator_kind::Equal, node->as<odata_binary_operator_node>()->operator_kind());
}

TEST(parse_filter_after_collection_structural_property)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)/FormerAddresses?$filter=Country eq 'abc'"));
	auto parsed_uri = parser.parse_uri(uri);
	auto filter_clause = parsed_uri->filter_clause();

	verify_is_operator_eq_node(filter_clause->expression());
	verify_range_variable(filter_clause->range_variable(), U("$it"), U("MyNS.Address"));
}

TEST(parse_filter_after_single_navigation_property)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)/Parent?$filter=ID gt 1"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_filter_after_single_entity)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)?$filter=ID gt 1"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_filter_after_dollar_count)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People/$count?$filter=ID gt 1"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_filter_after_dollar_value)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)/ID/$value?$filter=ID gt 1"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_filter_after_dollar_ref)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)/ID/$ref?$filter=ID gt 1"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_filter_after_dollar_batch)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/$batch?$filter=ID gt 1"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_filter_after_dollar_metadata)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/$metadata?$filter=ID gt 1"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

static void verify_orderby_item(std::shared_ptr<odata_orderby_clause> orderby_clause, ::size_t i, const ::odata::utility::string_t &property_name, bool asc)
{
	VERIFY_ARE_EQUAL(property_name, orderby_clause->item_at(i).first->as<odata_property_access_node>()->property_name());
	VERIFY_ARE_EQUAL(asc, orderby_clause->item_at(i).second);
}

TEST(parse_orderby_after_entity_set)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People?$orderby=ID desc,Age asc"));
	auto parsed_uri = parser.parse_uri(uri);
	auto orderby_clause = parsed_uri->orderby_clause();

	VERIFY_ARE_EQUAL(2, orderby_clause->items().size());
	verify_orderby_item(orderby_clause, 0, U("ID"), false);
	verify_orderby_item(orderby_clause, 1, U("Age"), true);

	verify_range_variable(orderby_clause->range_variable(), U("$it"), U("MyNS.Person"), U("People"));
}

TEST(parse_orderby_after_collection_navigation_property)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)/Mates?$orderby=ID desc,Age asc"));
	auto parsed_uri = parser.parse_uri(uri);
	auto orderby_clause = parsed_uri->orderby_clause();

	VERIFY_ARE_EQUAL(2, orderby_clause->items().size());
	verify_orderby_item(orderby_clause, 0, U("ID"), false);
	verify_orderby_item(orderby_clause, 1, U("Age"), true);

	verify_range_variable(orderby_clause->range_variable(), U("$it"), U("MyNS.Person"), U("People"));
}

TEST(parse_orderby_after_collection_structural_property)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)/FormerAddresses?$orderby=Country"));
	auto parsed_uri = parser.parse_uri(uri);
	auto orderby_clause = parsed_uri->orderby_clause();

	VERIFY_ARE_EQUAL(1, orderby_clause->items().size());
	verify_orderby_item(orderby_clause, 0, U("Country"), true);

	verify_range_variable(orderby_clause->range_variable(), U("$it"), U("MyNS.Address"));
}

TEST(parse_orderby_after_single_navigation_property)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)/Parent?$orderby=ID desc,Age asc"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_orderby_after_single_entity)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)?$orderby=ID desc,Age asc"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_orderby_after_dollar_count)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People/$count?$orderby=ID desc,Age asc"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_orderby_after_dollar_value)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)/ID/$value?$orderby=ID desc,Age asc"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_orderby_after_dollar_ref)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)/ID/$ref?$orderby=ID desc,Age asc"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_orderby_after_dollar_batch)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/$batch?$orderby=ID desc,Age asc"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_orderby_after_dollar_metadata)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/$metadata?$orderby=ID desc,Age asc"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_search)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People?$search=ABC or BCD"));
	auto parsed_uri = parser.parse_uri(uri);
	auto search_clause = parsed_uri->search_clause();

	verify_is_operator_or_node(search_clause->expression());

	verify_is_string(search_clause->expression()->as<odata_binary_operator_node>()->left(), U("ABC"));
    verify_is_string(search_clause->expression()->as<odata_binary_operator_node>()->right(), U("BCD"));
}

TEST(parse_select_after_nonstructured_type)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People/ID?$select=ID"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_expand_after_nonstructured_type)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People/ID?$expand=Mates"));

	VERIFY_THROWS(parser.parse_uri(uri), odata_exception);
}

TEST(parse_select_single_structural_property)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)?$select=ID"));
	auto parsed_uri = parser.parse_uri(uri);
	auto select_expand_clause = parsed_uri->select_expand_clause();
    
    VERIFY_ARE_EQUAL(1, select_expand_clause->select_items().size());
    verify_is_property_access_node(select_expand_clause->select_item_at(0)->end(), U("ID"));
}

TEST(parse_select_structural_properties)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)?$select=ID,Age"));
	auto parsed_uri = parser.parse_uri(uri);
	auto select_expand_clause = parsed_uri->select_expand_clause();
    
    VERIFY_ARE_EQUAL(2, select_expand_clause->select_items().size());
    verify_is_property_access_node(select_expand_clause->select_item_at(0)->end(), U("ID"));
    verify_is_property_access_node(select_expand_clause->select_item_at(1)->end(), U("Age"));
}

TEST(parse_select_structural_property_path)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)?$select=ID,Address/Country"));
	auto parsed_uri = parser.parse_uri(uri);
	auto select_expand_clause = parsed_uri->select_expand_clause();
    
    VERIFY_ARE_EQUAL(2, select_expand_clause->select_items().size());
    verify_is_property_access_node(select_expand_clause->select_item_at(0)->end(), U("ID"));
    verify_is_property_access_node(select_expand_clause->select_item_at(1)->end(), U("Country"));
    verify_is_property_access_node(select_expand_clause->select_item_at(1)->end()->as<odata_property_access_node>()->parent(), U("Address"));
}

TEST(parse_star_in_select)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)?$select=*"));
	auto parsed_uri = parser.parse_uri(uri);
	auto select_expand_clause = parsed_uri->select_expand_clause();
    
    VERIFY_ARE_EQUAL(1, select_expand_clause->select_items().size());
    verify_is_property_access_node(select_expand_clause->select_item_at(0)->end(), U("*"));
}

TEST(parse_star_in_select_path)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)?$select=ID,Address/*"));
	auto parsed_uri = parser.parse_uri(uri);
	auto select_expand_clause = parsed_uri->select_expand_clause();
    
    VERIFY_ARE_EQUAL(2, select_expand_clause->select_items().size());
    verify_is_property_access_node(select_expand_clause->select_item_at(0)->end(), U("ID"));
    verify_is_property_access_node(select_expand_clause->select_item_at(1)->end(), U("*"));
    verify_is_property_access_node(select_expand_clause->select_item_at(1)->end()->as<odata_property_access_node>()->parent(), U("Address"));
}

TEST(parse_expand_single_navigation_property)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)?$expand=Mates"));
	auto parsed_uri = parser.parse_uri(uri);
	auto select_expand_clause = parsed_uri->select_expand_clause();
    
    VERIFY_ARE_EQUAL(1, select_expand_clause->expand_items().size());
    verify_is_property_access_node(select_expand_clause->expand_item_at(0)->end(), U("Mates"));
}

TEST(parse_expand_navigation_properties)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)?$expand=Mates,Friends"));
	auto parsed_uri = parser.parse_uri(uri);
	auto select_expand_clause = parsed_uri->select_expand_clause();
    
    VERIFY_ARE_EQUAL(2, select_expand_clause->expand_items().size());
    verify_is_property_access_node(select_expand_clause->expand_item_at(0)->end(), U("Mates"));
    verify_is_property_access_node(select_expand_clause->expand_item_at(1)->end(), U("Friends"));
}

TEST(parse_expand_navigation_property_path)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)?$expand=Mates,Parent/Friends"));
	auto parsed_uri = parser.parse_uri(uri);
	auto select_expand_clause = parsed_uri->select_expand_clause();
    
    VERIFY_ARE_EQUAL(2, select_expand_clause->expand_items().size());
    verify_is_property_access_node(select_expand_clause->expand_item_at(0)->end(), U("Mates"));
    verify_is_property_access_node(select_expand_clause->expand_item_at(1)->end(), U("Friends"));
    verify_is_property_access_node(select_expand_clause->expand_item_at(1)->end()->as<odata_property_access_node>()->parent(), U("Parent"));
}

TEST(parse_star_in_expand)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)?$expand=*"));
	auto parsed_uri = parser.parse_uri(uri);
	auto select_expand_clause = parsed_uri->select_expand_clause();
    
    VERIFY_ARE_EQUAL(1, select_expand_clause->expand_items().size());
    verify_is_property_access_node(select_expand_clause->expand_item_at(0)->end(), U("*"));
}

TEST(parse_star_in_expand_path)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People(1)?$expand=Mates,Parent/*"));
	auto parsed_uri = parser.parse_uri(uri);
	auto select_expand_clause = parsed_uri->select_expand_clause();
    
    VERIFY_ARE_EQUAL(2, select_expand_clause->expand_items().size());
    verify_is_property_access_node(select_expand_clause->expand_item_at(0)->end(), U("Mates"));
    verify_is_property_access_node(select_expand_clause->expand_item_at(1)->end(), U("*"));
    verify_is_property_access_node(select_expand_clause->expand_item_at(1)->end()->as<odata_property_access_node>()->parent(), U("Parent"));
}

TEST(parse_nested_expand_options)
{
    odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People?$expand=Mates($select=ID;$expand=Friends($select=Age;$filter='abc()' eq 'abc()');$filter=Age gt 10;$orderby=Age asc;$top=10;$skip=1;$count=true),Parent($select=ID)"));
	auto parsed_uri = parser.parse_uri(uri);
	auto select_expand_clause = parsed_uri->select_expand_clause();
	VERIFY_ARE_EQUAL(2, select_expand_clause->expand_items().size());
	VERIFY_ARE_EQUAL(U("Mates"), select_expand_clause->expand_item_at(0)->end()->as<odata_property_access_node>()->property_name());
	VERIFY_IS_TRUE(select_expand_clause->expand_item_at(0)->select_expand_clause() != nullptr);
	VERIFY_ARE_EQUAL(1, select_expand_clause->expand_item_at(0)->select_expand_clause()->select_items().size());
	VERIFY_ARE_EQUAL(1, select_expand_clause->expand_item_at(0)->select_expand_clause()->expand_items().size());
	VERIFY_IS_TRUE(select_expand_clause->expand_item_at(0)->filter_clause() != nullptr);
	VERIFY_ARE_EQUAL(1, select_expand_clause->expand_item_at(0)->orderby_clause()->items().size());
	VERIFY_ARE_EQUAL(10, select_expand_clause->expand_item_at(0)->top().value());
	VERIFY_ARE_EQUAL(1, select_expand_clause->expand_item_at(0)->skip().value());
	VERIFY_IS_TRUE(select_expand_clause->expand_item_at(0)->count().value());

	VERIFY_ARE_EQUAL(U("Parent"), select_expand_clause->expand_item_at(1)->end()->as<odata_property_access_node>()->property_name());
	VERIFY_IS_TRUE(select_expand_clause->expand_item_at(1)->select_expand_clause() != nullptr);
	VERIFY_ARE_EQUAL(1, select_expand_clause->expand_item_at(1)->select_expand_clause()->select_items().size());
}

TEST(parse_filter_globalization_test_1)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People?$filter=ChineseName gt 'abc张三bcd李四efg'"));
	auto parsed_uri = parser.parse_uri(uri);
	auto filter_clause = parsed_uri->filter_clause();

	verify_is_operator_gt_node(filter_clause->expression());
    auto binary_node = filter_clause->expression()->as<odata_binary_operator_node>();
    verify_is_property_access_node(binary_node->left(), U("ChineseName"));
    verify_is_string(binary_node->right(), U("abc张三bcd李四efg"));
}

TEST(parse_filter_globalization_test_2)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People?$filter=ChineseName gt 'abcにほんごbcdふりがなefg'"));
	auto parsed_uri = parser.parse_uri(uri);
	auto filter_clause = parsed_uri->filter_clause();

	verify_is_operator_gt_node(filter_clause->expression());
    auto binary_node = filter_clause->expression()->as<odata_binary_operator_node>();
    verify_is_property_access_node(binary_node->left(), U("ChineseName"));
    verify_is_string(binary_node->right(), U("abcにほんごbcdふりがなefg"));
}

TEST(parse_filter_globalization_test_3)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People?$filter=ChineseName gt 'abc조선글bcd골프에efg'"));
	auto parsed_uri = parser.parse_uri(uri);
	auto filter_clause = parsed_uri->filter_clause();

	verify_is_operator_gt_node(filter_clause->expression());
    auto binary_node = filter_clause->expression()->as<odata_binary_operator_node>();
    verify_is_property_access_node(binary_node->left(), U("ChineseName"));
    verify_is_string(binary_node->right(), U("abc조선글bcd골프에efg"));
}

TEST(parse_filter_globalization_test_4)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People?$filter=ChineseName gt 'Русскийязык'"));
	auto parsed_uri = parser.parse_uri(uri);
	auto filter_clause = parsed_uri->filter_clause();

	verify_is_operator_gt_node(filter_clause->expression());
    auto binary_node = filter_clause->expression()->as<odata_binary_operator_node>();
    verify_is_property_access_node(binary_node->left(), U("ChineseName"));
    verify_is_string(binary_node->right(), U("Русскийязык"));
}

TEST(parse_filter_globalization_test_5)
{
	odata_uri_parser parser(get_test_model_with_person_with_one_key());
	auto uri = ::odata::utility::uri::encode_uri(U("http://service-root/People?$filter=ChineseName gt 'débutakönnten'"));
	auto parsed_uri = parser.parse_uri(uri);
	auto filter_clause = parsed_uri->filter_clause();

	verify_is_operator_gt_node(filter_clause->expression());
    auto binary_node = filter_clause->expression()->as<odata_binary_operator_node>();
    verify_is_property_access_node(binary_node->left(), U("ChineseName"));
    verify_is_string(binary_node->right(), U("débutakönnten"));
}

// --END-- odata_query_option_parser

// --BEGIN-- odata_path_segment_visitor

class print_visitor : public odata_path_segment_visitor<::odata::utility::string_t>
{
public:
	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_metadata_segment> segment)
	{
		return U("Metadata");
	}

	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_batch_segment> segment)
	{
		return U("Batch");
	}

	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_entity_set_segment> segment)
	{
		return U("EntitySet");
	}

	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_singleton_segment> segment)
	{
		return U("Singleton");
	}

	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_key_segment> segment)
	{
		return U("Key");
	}

	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_structural_property_segment> segment)
	{
		return U("StructuralProperty");
	}

	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_navigation_property_segment> segment)
	{
		return U("NavigationProperty");
	}

	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_dynamic_property_segment> segment)
	{
		return U("DynamicProperty");
	}

	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_value_segment> segment)
	{
		return U("Value");
	}

	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_count_segment> segment)
	{
		return U("Count");
	}

	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_ref_segment> segment)
	{
		return U("Ref");
	}

	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_type_segment> segment)
	{
		return U("Type");
	}

	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_operation_segment> segment)
	{
		return U("Operation");
	}

	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_operation_import_segment> segment)
	{
		return U("OperationImport");
	}

	::odata::utility::string_t visit_any(std::shared_ptr<::odata::core::odata_path_segment> segment)
	{
		return U("Any");
	}
};

TEST(visit_odata_path)
{
	auto model = get_test_model_with_person_with_one_key();

	std::vector<std::shared_ptr<odata_path_segment>> segments;
	segments.push_back(odata_path_segment::create_metadata_segment());
	segments.push_back(odata_path_segment::create_batch_segment());
	segments.push_back(odata_path_segment::create_entity_set_segment(model->find_container()->find_entity_set(U("People"))));
	segments.push_back(odata_path_segment::create_singleton_segment(model->find_container()->find_singleton(U("CEO"))));
	segments.push_back(odata_path_segment::create_key_segment(nullptr, nullptr, std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>>()));
	segments.push_back(odata_path_segment::create_structural_property_segment(nullptr, nullptr));
	segments.push_back(odata_path_segment::create_navigation_property_segment(nullptr, nullptr, nullptr));
	segments.push_back(odata_path_segment::create_dynamic_property_segment(U("DynamicProperty")));
	segments.push_back(odata_path_segment::create_value_segment());
	segments.push_back(odata_path_segment::create_count_segment());
	segments.push_back(odata_path_segment::create_ref_segment());
	segments.push_back(odata_path_segment::create_type_segment(nullptr));
	segments.push_back(odata_path_segment::create_operation_segment(nullptr, std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>>()));
	segments.push_back(odata_path_segment::create_operation_import_segment(nullptr, std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>>()));
	segments.push_back(std::make_shared<odata_path_segment>());

	odata_path path(segments);
	auto visitor = std::make_shared<print_visitor>();
	auto result = path.visit_with(std::static_pointer_cast<odata_path_segment_visitor<::odata::utility::string_t>>(visitor));

	VERIFY_ARE_EQUAL(U("Metadata"), result[0]);
	VERIFY_ARE_EQUAL(U("Batch"), result[1]);
	VERIFY_ARE_EQUAL(U("EntitySet"), result[2]);
	VERIFY_ARE_EQUAL(U("Singleton"), result[3]);
	VERIFY_ARE_EQUAL(U("Key"), result[4]);
	VERIFY_ARE_EQUAL(U("StructuralProperty"), result[5]);
	VERIFY_ARE_EQUAL(U("NavigationProperty"), result[6]);
	VERIFY_ARE_EQUAL(U("DynamicProperty"), result[7]);
	VERIFY_ARE_EQUAL(U("Value"), result[8]);
	VERIFY_ARE_EQUAL(U("Count"), result[9]);
	VERIFY_ARE_EQUAL(U("Ref"), result[10]);
	VERIFY_ARE_EQUAL(U("Type"), result[11]);
	VERIFY_ARE_EQUAL(U("Operation"), result[12]);
	VERIFY_ARE_EQUAL(U("OperationImport"), result[13]);
	VERIFY_ARE_EQUAL(U("Any"), result[14]);
}

// --END-- odata_path_segment_visitor

// --BEGIN-- odata_query_node_visitor

class to_string_visitor : public odata_query_node_visitor<::odata::utility::string_t>
{
public:
	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_constant_node> node)
    {
        return U("Constant");
    }
    
	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_binary_operator_node> node)
    {
        return U("BinaryOperator");
    }
    
	::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_unary_operator_node> node)
    {
        return U("UnaryOperator");
    }
    
    ::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_parameter_alias_node> node)
    {
        return U("ParameterAlias");
    }
    
    ::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_property_access_node> node)
    {
        return U("PropertyAccess");
    }
        
    ::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_type_cast_node> node)
    {
        return U("TypeCast");
    }

    ::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_lambda_node> node)
    {
        return U("Lambda");
    }
    
    ::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_range_variable_node> node)
    {
        return U("RangeVariable");
    }
    
    ::odata::utility::string_t visit(std::shared_ptr<::odata::core::odata_function_call_node> node)
    {
        return U("FunctionCall");
    }
    
	::odata::utility::string_t visit_any(std::shared_ptr<::odata::core::odata_query_node> node)
    {
        return U("Any");
    }
};

TEST(visit_query_nodes)
{
    std::shared_ptr<odata_query_node_visitor<::odata::utility::string_t>> visitor = std::make_shared<to_string_visitor>();
    
    auto constant = odata_query_node::create_constant_node(nullptr);
    VERIFY_ARE_EQUAL(U("Constant"), constant->accept(visitor));
    
    auto binary_operator = odata_query_node::create_operator_add_node(nullptr, nullptr);
    VERIFY_ARE_EQUAL(U("BinaryOperator"), binary_operator->accept(visitor));
    
    auto unary_operator = odata_query_node::create_operator_not_node(nullptr);
    VERIFY_ARE_EQUAL(U("UnaryOperator"), unary_operator->accept(visitor));
    
    auto parameter_alias = odata_query_node::create_parameter_alias_node(U("@word"));
    VERIFY_ARE_EQUAL(U("ParameterAlias"), parameter_alias->accept(visitor));

    auto property_access = odata_query_node::create_property_access_node(U("a"), nullptr);
    VERIFY_ARE_EQUAL(U("PropertyAccess"), property_access->accept(visitor));

	auto type_cast = odata_query_node::create_type_cast_node(U("b.b"), nullptr);
    VERIFY_ARE_EQUAL(U("TypeCast"), type_cast->accept(visitor));

    auto lambda = odata_query_node::create_lambda_node(false, nullptr, U("d"), nullptr);
    VERIFY_ARE_EQUAL(U("Lambda"), lambda->accept(visitor));
    
    auto range_variable = odata_query_node::create_range_variable_node(U("d"));
    VERIFY_ARE_EQUAL(U("RangeVariable"), range_variable->accept(visitor));
    
    auto function_call = odata_query_node::create_function_call_node(U("foo"),
        std::vector<std::pair< ::odata::utility::string_t, std::shared_ptr<odata_query_node>>>());
    VERIFY_ARE_EQUAL(U("FunctionCall"), function_call->accept(visitor));
    
    auto node = std::make_shared<odata_query_node>();
    VERIFY_ARE_EQUAL(U("Any"), node->accept(visitor));
}

// --END-- odata_query_node_visitor

}

}}}