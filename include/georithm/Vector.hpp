//          Copyright Dominic Koepke 2017 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef GEORITHM_VECTOR_HPP
#define GEORITHM_VECTOR_HPP

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <concepts>
#include <numeric>

#include "ArithmeticOperators.hpp"
#include "Concepts.hpp"
#include "RangeAlgorithms.hpp"

namespace georithm
{
	template <ValueType T, DimensionDescriptor_t TDim>
	requires Cardinality<TDim>
	class Vector :
		private Arithmetic
	{
	public:
		using ValueType = T;
		using DimensionDescriptorType = DimensionDescriptor_t;
		constexpr static DimensionDescriptorType dimensions{ TDim };

		constexpr Vector() noexcept = default;
		/*ToDo: c++20
		constexpr */
		~Vector() noexcept = default;

		constexpr Vector(const Vector&) noexcept = default;
		constexpr Vector& operator =(const Vector&) noexcept = default;

		constexpr Vector(Vector&&) noexcept = default;
		constexpr Vector& operator =(Vector&&) noexcept = default;

		template <class... TArgs>
		requires (sizeof...(TArgs) == TDim) && std::is_same_v<std::common_type_t<TArgs...>, T>
		constexpr Vector(TArgs&&... args) noexcept :
			m_Values{ std::forward<TArgs>(args)... }
		{
		}

		template <class T2>
		requires (!std::is_same_v<T2, T> && std::convertible_to<T2, T>)
		explicit constexpr Vector(const Vector<T2, TDim>& other) noexcept
		{
			std::transform(std::begin(other),
							std::end(other),
							std::begin(m_Values),
							[](T2 value) -> T { return static_cast<T>(value); }
						);
		}

		[[nodiscard]] constexpr static Vector zero() noexcept
		{
			return make(T(0));
		}

		[[nodiscard]] constexpr const ValueType& operator [](DimensionDescriptorType index) const noexcept
		{
			return m_Values[index];
		}

		[[nodiscard]] constexpr ValueType& operator [](DimensionDescriptorType index) noexcept
		{
			return m_Values[index];
		}

		template <DimensionDescriptor_t TDim2 = TDim>
		requires (1 <= TDim2) && (TDim2 == TDim)
		[[nodiscard]] constexpr const ValueType& x() const noexcept
		{
			return m_Values[0];
		}

		template <DimensionDescriptor_t TDim2 = TDim>
		requires (1 <= TDim2) && (TDim2 == TDim)
		[[nodiscard]] constexpr ValueType& x() noexcept
		{
			return m_Values[0];
		}

		template <DimensionDescriptor_t TDim2 = TDim>
		requires (2 <= TDim2) && (TDim2 == TDim)
		[[nodiscard]] constexpr const ValueType& y() const noexcept
		{
			return m_Values[1];
		}

		template <DimensionDescriptor_t TDim2 = TDim>
		requires (2 <= TDim2)
		[[nodiscard]] constexpr ValueType& y() noexcept
		{
			return m_Values[1];
		}

		template <DimensionDescriptor_t TDim2 = TDim>
		requires (3 <= TDim2) && (TDim2 == TDim)
		[[nodiscard]] constexpr const ValueType& z() const noexcept
		{
			return m_Values[2];
		}

		template <DimensionDescriptor_t TDim2 = TDim>
		requires (3 <= TDim2) && (TDim2 == TDim)
		[[nodiscard]] constexpr ValueType& z() noexcept
		{
			return m_Values[2];
		}

		[[nodiscard]] constexpr bool operator ==(const Vector&) const noexcept = default;

		template <class T2>
		requires Addable<T, T2>
		constexpr Vector& operator +=(const Vector<T2, TDim>& other) noexcept
		{
			zip_elements(std::begin(m_Values),
						std::end(m_Values),
						std::begin(other.m_Values),
						[](auto lhs, const auto& rhs) { return lhs += static_cast<T>(rhs); }
						);
			return *this;
		}

		template <class T2>
		requires Subtractable<T, T2>
		constexpr Vector& operator -=(const Vector<T2, TDim>& other) noexcept
		{
			zip_elements(std::begin(m_Values),
						std::end(m_Values),
						std::begin(other.m_Values),
						[](auto lhs, const auto& rhs) { return lhs -= static_cast<T>(rhs); }
						);
			return *this;
		}

		template <class T2>
		requires Addable<T, T2>
		constexpr Vector& operator +=(const T2& other) noexcept
		{
			std::for_each(std::begin(m_Values),
						std::end(m_Values),
						[&other](auto& lhs) { return lhs += static_cast<T>(other); }
						);
			return *this;
		}

		template <class T2>
		requires Subtractable<T, T2>
		constexpr Vector& operator -=(const T2& other) noexcept
		{
			std::for_each(std::begin(m_Values),
						std::end(m_Values),
						[&other](auto& lhs) { return lhs -= static_cast<T>(other); }
						);
			return *this;
		}

		template <class T2>
		requires Multiplicable<T, T2>
		constexpr Vector& operator *=(const T2& other) noexcept
		{
			std::for_each(std::begin(m_Values),
						std::end(m_Values),
						[&other](auto& lhs) { return lhs *= static_cast<T>(other); }
						);
			return *this;
		}

		template <class T2>
		requires Multiplicable<T2, T>
		friend Vector operator *(const T2& lhs, Vector rhs) noexcept
		{
			std::for_each(std::begin(rhs),
						std::end(rhs),
						[&lhs](auto& el) { return el *= static_cast<T>(lhs); }
						);
			return rhs;
		}

		template <class T2>
		requires Divisable<T, T2>
		constexpr Vector& operator /=(const T2& other) noexcept
		{
			assert(other != T2(0));
			std::for_each(std::begin(m_Values),
						std::end(m_Values),
						[&other](auto& lhs) { return lhs /= static_cast<T>(other); }
						);
			return *this;
		}

		template <class T2>
		requires Moduloable<T, T2>
		constexpr Vector& operator %=(const T2& other) noexcept
		{
			assert(other != T2(0));
			std::for_each(std::begin(m_Values),
						std::end(m_Values),
						[&other](auto& lhs) { return lhs %= other; }
						);
			return *this;
		}

		[[nodiscard]] constexpr auto begin() noexcept
		{
			return std::begin(m_Values);
		}

		[[nodiscard]] constexpr auto begin() const noexcept
		{
			return std::begin(m_Values);
		}

		[[nodiscard]] constexpr auto cbegin() const noexcept
		{
			return std::cbegin(m_Values);
		}

		constexpr auto end() noexcept
		{
			return std::end(m_Values);
		}

		[[nodiscard]] constexpr auto end() const noexcept
		{
			return std::end(m_Values);
		}

		[[nodiscard]] constexpr auto cend() const noexcept
		{
			return std::cend(m_Values);
		}

		constexpr auto rbegin() noexcept
		{
			return std::rbegin(m_Values);
		}

		[[nodiscard]] constexpr auto rbegin() const noexcept
		{
			return std::rbegin(m_Values);
		}

		[[nodiscard]] constexpr auto crbegin() const noexcept
		{
			return std::crbegin(m_Values);
		}

		constexpr auto rend() noexcept
		{
			return std::rend(m_Values);
		}

		[[nodiscard]] constexpr auto rend() const noexcept
		{
			return std::rend(m_Values);
		}

		[[nodiscard]] constexpr auto crend() const noexcept
		{
			return std::crend(m_Values);
		}

	private:
		std::array<T, dimensions> m_Values{};

		[[nodiscard]] constexpr static Vector make(const T& value) noexcept
		{
			Vector tmp;
			tmp.m_Values.fill(value);
			return tmp;
		}
	};

	template <class T, class... U>
	Vector(T, U ...) -> Vector<T, 1 + sizeof...(U)>;

	template <VectorObject TVector>
	requires ConstForwardIteratable<TVector>
	[[nodiscard]] constexpr typename TVector::ValueType lengthSq(const TVector& vector) noexcept
	{
		return std::accumulate(std::cbegin(vector),
								std::cend(vector),
								typename TVector::ValueType(0),
								[](auto value, const auto& element) { return value + element * element; }
							);
	}

	template <VectorObject TVector1, VectorObject TVector2>
	requires ConstForwardIteratable<TVector1> && ConstForwardIteratable<TVector2> &&
	(TVector1::dimensions == TVector2::dimensions) &&
	Multiplicable<typename TVector1::ValueType, typename TVector2::ValueType>
	[[nodiscard]] constexpr typename TVector1::ValueType scalarProduct(const TVector1& lhs, const TVector2& rhs) noexcept
	{
		return std::inner_product(std::cbegin(lhs), std::cend(lhs), std::cbegin(rhs), typename TVector1::ValueType(0));
	}

	template <VectorObject TVector>
	[[nodiscard]] constexpr typename TVector::ValueType length(const TVector& vector) noexcept
	{
		return static_cast<typename TVector::ValueType>(std::sqrt(lengthSq(vector)));
	}

	template <class T2, VectorObject TVector>
	[[nodiscard]] constexpr T2 length(const TVector& vector) noexcept
	{
		return static_cast<T2>(std::sqrt(lengthSq(vector)));
	}

	template <VectorObject TVector>
	requires std::floating_point<typename TVector::ValueType>
	[[nodiscard]] constexpr TVector normalize(TVector vector) noexcept
	{
		assert(vector != TVector::zero());

		auto length = georithm::length(vector);
		return vector /= length;
	}

	template <VectorObject TVector>
	requires std::floating_point<typename TVector::ValueType> || std::signed_integral<typename TVector::ValueType>
	[[nodiscard]] constexpr TVector abs(TVector vector) noexcept
	{
		for (auto& el : vector)
			el = std::abs(el);
		return vector;
	}

	template <NDimensionalVectorObject<2> TVector>
	requires std::floating_point<typename TVector::ValueType>
	[[nodiscard]] constexpr TVector rotate(const TVector& vector, typename TVector::ValueType radian) noexcept
	{
		auto sin = std::sin(radian);
		auto cos = std::cos(radian);
		return { cos * vector[0] - sin * vector[1], sin * vector[0] + cos * vector[1] };
	}

	template <NDimensionalVectorObject<2> TVector>
	requires std::signed_integral<typename TVector::ValueType>
	[[nodiscard]] constexpr TVector rotate(TVector vector, double radian) noexcept
	{
		auto rotated = rotate(static_cast<Vector<double, 2>>(vector), radian);
		zip_elements(std::begin(vector),
					std::end(vector),
					std::begin(rotated),
					[](const auto& lhs, const auto& rhs) { return static_cast<typename TVector::ValueType>(std::llround(rhs)); }
					);
		return vector;
	}
}

#endif
