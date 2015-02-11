//---------------------------------------------------------------------
// <copyright file="common_nullable_test.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "../odata_tests.h"
#include "odata/common/nullable.h"

using namespace ::odata::common;

namespace tests { namespace functional { namespace _odata {

SUITE(common_nullable_test)
{

TEST(compare_same_nullable)
{
	nullable<int> x(1);
	nullable<int> y(1);
	nullable<int> z(2);
	nullable<int> u;
	nullable<int> v;
	VERIFY_ARE_EQUAL(x, y);
	VERIFY_ARE_NOT_EQUAL(x, z);
	VERIFY_ARE_NOT_EQUAL(x, u);
	VERIFY_ARE_EQUAL(u, v);
}

TEST(compare_different_nullable)
{
	nullable<int> x(1);
	nullable<char> y(1);
	nullable<char> z(2);
	nullable<int> u;
	nullable<char> v;
	VERIFY_ARE_NOT_EQUAL(x, y);
	VERIFY_ARE_NOT_EQUAL(x, z);
	VERIFY_ARE_EQUAL(u, v);
	VERIFY_ARE_NOT_EQUAL(x, v);
}

TEST(compare_nullable_with_T)
{
	nullable<int> x(1);
	nullable<int> y;
	VERIFY_ARE_EQUAL(x, 1);
	VERIFY_ARE_NOT_EQUAL(x, 2);
	VERIFY_ARE_EQUAL(1, x);
	VERIFY_ARE_NOT_EQUAL(2, x);
	VERIFY_ARE_NOT_EQUAL(y, 1);
	VERIFY_ARE_NOT_EQUAL(1, y);
}

TEST(assignment_same_nullable)
{
	nullable<int> x(1);
	nullable<int> y;
	VERIFY_IS_TRUE(x.has_value());
	VERIFY_IS_FALSE(y.has_value());
	y = x;
	VERIFY_IS_TRUE(y.has_value());
	VERIFY_ARE_EQUAL(x, y);
	nullable<int> z;
	y = z;
	VERIFY_IS_FALSE(y.has_value());
}

TEST(assignment_nullable_T)
{
	nullable<int> x;
	VERIFY_IS_FALSE(x.has_value());
	x = 1;
	VERIFY_IS_TRUE(x.has_value());
	VERIFY_ARE_EQUAL(x, 1);
	x = 2.5;
	VERIFY_IS_TRUE(x.has_value());
	VERIFY_ARE_EQUAL(x, 2);
}

TEST(new_from_nullptr)
{
	nullable<int> x = null_value;
	VERIFY_IS_FALSE(x.has_value());
	VERIFY_IS_TRUE(x == null_value);
	nullable<int> y(1);
	VERIFY_IS_TRUE(null_value != y);
}

TEST(assignment_nullable_nullptr)
{
	nullable<int> x(1);
	VERIFY_IS_TRUE(x.has_value());
	x = null_value;
	VERIFY_IS_FALSE(x.has_value());
	VERIFY_IS_TRUE(x == null_value);
}

}

}}}