//---------------------------------------------------------------------
// <copyright file="nullable.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include <type_traits>

namespace odata { namespace common
{

#ifndef null_value
#ifdef WIN32
#define null_value nullptr
#else
#define null_value ((std::nullptr_t)nullptr)
#endif
#endif

template <typename T>
class nullable
{
public:
	static_assert(std::is_pod<T>::value, "nullable<T> requires T to be POD type");

	nullable()
		: m_has_value(false), m_value() {}

	nullable(std::nullptr_t)
		: m_has_value(false), m_value() {}

	nullable(T y)
		: m_has_value(true), m_value(y) {}

	nullable(const nullable<T> &y)
		: m_has_value(y.has_value()), m_value(y.value()) {}

	~nullable() {}

	bool has_value() const { return m_has_value; }

	T value() const { return m_value; }

	bool operator==(const nullable<T> &y) const
	{
		return (!has_value() && !y.has_value())
			|| (has_value() && y.has_value() && value() == y.value());
	}

	template <typename X>
	bool operator==(const nullable<X> &y) const
	{ return !has_value() && !y.has_value(); }

	bool operator==(std::nullptr_t) const
	{ return !has_value(); }

	bool operator==(T y) const
	{ return has_value() && value() == y; }

	template <typename X>
	bool operator!=(const nullable<X> &y) const
	{ return !(*this == y); }

	bool operator!=(std::nullptr_t) const
	{ return !(*this == null_value); }

	bool operator!=(T y) const
	{ return !(*this == y); }

	nullable<T> &operator=(const nullable<T> &y)
	{
		m_has_value = y.has_value();
		m_value = y.value();

		return *this;
	}

	nullable<T> &operator=(std::nullptr_t)
	{
		m_has_value = false;

		return *this;
	}

	nullable<T> &operator=(T y)
	{
		m_has_value = true;
		m_value = y;

		return *this;
	}

private:
	bool m_has_value;
	T m_value;
};

template <typename T>
bool operator==(T x, const nullable<T> &y) { return y == x; }

template <typename T>
bool operator!=(T x, const nullable<T> &y) { return y != x; }

template <typename T>
bool operator==(std::nullptr_t, const nullable<T> &y) { return y == null_value; }

template <typename T>
bool operator!=(std::nullptr_t, const nullable<T> &y) { return y != null_value; }

}}