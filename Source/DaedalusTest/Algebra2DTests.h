#pragma once

#include <gtest/gtest.h>
#include <Utilities/Algebra/Algebra2D.h>
#include <Utilities/Constants.h>
#include <iostream>

using namespace utils;

inline bool DoubleEquals(const double d1, const double d2 = 0) {
	return std::abs(d1 - d2) < FLOAT_ERROR;
}

TEST(Vector2D, Constructors) {
	Vector2D<> v1(0.5, 0.6);
	Vector2D<> v2(v1);
	ASSERT_TRUE(v1.X == 0.5 && v1.Y == 0.6);
	ASSERT_TRUE(v2.X == 0.5 && v2.Y == 0.6);
}

TEST(Vector2D, PerformsEquality) {
	Vector2D<> v1(0.5, 0.5);
	Vector2D<> v2(0.5, 0.5);
	Vector2D<> v3(0.6, 0.5);
	Vector2D<> v4(0.5, 0.6);
	Vector2D<> v5(0.6, 0.6);
	ASSERT_TRUE(v1 == v2);
	ASSERT_FALSE(v1 == v3);
	ASSERT_FALSE(v1 == v4);
	ASSERT_FALSE(v1 == v5);
}

TEST(Vector2D, PerformsLessThan) {
	Vector2D<> v1(0.5, 0.5);
	Vector2D<> v2(0.5, 0.5);
	Vector2D<> v3(0.6, 0.5);
	Vector2D<> v4(0.5, 0.6);
	Vector2D<> v5(0.6, 0.6);
	ASSERT_FALSE(v1 < v2);
	ASSERT_TRUE(v1 < v3);
	ASSERT_TRUE(v1 < v4);
	ASSERT_TRUE(v1 < v5);
}

TEST(Vector2D, PerformsAddition) {
	Vector2D<int32_t> v1(5, 6);
	Vector2D<int32_t> v2(2, 7);
	ASSERT_TRUE((v1 + v2) == Vector2D<int32_t>(7, 13));
	ASSERT_TRUE((v1 + 4) == Vector2D<int32_t>(9, 10));
}

TEST(Vector2D, PerformsSubtraction) {
	Vector2D<int32_t> v1(5, 6);
	Vector2D<int32_t> v2(2, 7);
	ASSERT_TRUE((v1 - v2) == Vector2D<int32_t>(3, -1));
	ASSERT_TRUE((v1 - 4) == Vector2D<int32_t>(1, 2));
	ASSERT_TRUE(-v1 == Vector2D<int32_t>(-5, -6));
}

TEST(Vector2D, PerformsMultiplication) {
	Vector2D<int32_t> v1(5, 6);
	Vector2D<int32_t> v2(2, 7);
	ASSERT_TRUE((v1 * v2) == Vector2D<int32_t>(10, 42));
	ASSERT_TRUE((v1 * 4) == Vector2D<int32_t>(20, 24));
}

TEST(Vector2D, PerformsDivision) {
	Vector2D<int32_t> v1(6, 42);
	Vector2D<int32_t> v2(2, 7);
	ASSERT_TRUE((v1 / v2) == Vector2D<int32_t>(3, 6));
	ASSERT_TRUE((v1 / 3) == Vector2D<int32_t>(2, 14));
}

TEST(Vector2D, Length) {
	Vector2D<int32_t> v1(3, 4);
	Vector2D<int32_t> v2(10, 0);
	ASSERT_TRUE(DoubleEquals(v1.Length(), 5.0));
	ASSERT_TRUE(DoubleEquals(v2.Length(), 10.0));
}

TEST(Vector2D, Normalizes) {
	Vector2D<int32_t> v1(60, 42);
	Vector2D<int32_t> v2(2, 1);
	Vector2D<int32_t> v3(-75, 23);
	Vector2D<int32_t> v4(39, -5);
	ASSERT_TRUE(DoubleEquals(v1.Normalize().Length(), 1.0));
	ASSERT_TRUE(DoubleEquals(v2.Normalize().Length(), 1.0));
	ASSERT_TRUE(DoubleEquals(v3.Normalize().Length(), 1.0));
	ASSERT_TRUE(DoubleEquals(v4.Normalize().Length(), 1.0));
}
