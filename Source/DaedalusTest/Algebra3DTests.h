#pragma once

#include <gtest/gtest.h>
#include <Utilities/Algebra/Algebra3.h>
#include <Utilities/Constants.h>
#include <iostream>

using namespace utils;

TEST(Vector3D, Constructors) {
	Vector3<> v1(Vector2<>(0.5, 0.6), 0.7);
	Vector3<> v2(0.5, 0.6, 0.7);
	Vector3<> v3(v2);
	ASSERT_TRUE(v1.X == 0.5 && v1.Y == 0.6 && v1.Z == 0.7);
	ASSERT_TRUE(v2.X == 0.5 && v2.Y == 0.6 && v2.Z == 0.7);
	ASSERT_TRUE(v3.X == 0.5 && v3.Y == 0.6 && v3.Z == 0.7);
}

TEST(Vector3D, PerformsEquality) {
	Vector3<> v1(0.5, 0.6, 0.7);
	Vector3<> v2(0.5, 0.6, 0.7);
	Vector3<> v3(0.6, 0.5, 0.7);
	Vector3<> v4(0.5, 0.6, 0.6);
	Vector3<> v5(0.6, 0.6, 0.6);
	ASSERT_TRUE(v1 == v2);
	ASSERT_FALSE(v1 == v3);
	ASSERT_FALSE(v1 == v4);
	ASSERT_FALSE(v1 == v5);
}

TEST(Vector3D, PerformsAddition) {
	Vector3<int32_t> v1(5, 6, 7);
	Vector3<int32_t> v2(2, 7, 11);
	ASSERT_TRUE((v1 + v2) == Vector3<int32_t>(7, 13, 18));
	ASSERT_TRUE((v1 + 4) == Vector3<int32_t>(9, 10, 11));
}

TEST(Vector3D, PerformsSubtraction) {
	Vector3<int32_t> v1(5, 6, 7);
	Vector3<int32_t> v2(2, 7, 11);
	ASSERT_TRUE((v1 - v2) == Vector3<int32_t>(3, -1, -4));
	ASSERT_TRUE((v1 - 4) == Vector3<int32_t>(1, 2, 3));
	ASSERT_TRUE(-v1 == Vector3<int32_t>(-5, -6, -7));
}

TEST(Vector3D, PerformsMultiplication) {
	Vector3<int32_t> v1(5, 6, 7);
	Vector3<int32_t> v2(2, 7, 11);
	ASSERT_TRUE((v1 * v2) == Vector3<int32_t>(10, 42, 77));
	ASSERT_TRUE((v1 * 4) == Vector3<int32_t>(20, 24, 28));
}

TEST(Vector3D, PerformsDivision) {
	Vector3<int32_t> v1(6, 42, 99);
	Vector3<int32_t> v2(2, 7, 11);
	ASSERT_TRUE((v1 / v2) == Vector3<int32_t>(3, 6, 9));
	ASSERT_TRUE((v1 / 3) == Vector3<int32_t>(2, 14, 33));
}

TEST(Vector3D, Length) {
	Vector3<int32_t> v1(3, 4, 5);
	Vector3<int32_t> v2(10, 0, 0);
	ASSERT_TRUE(DoubleEquals(v1.Length(), std::sqrt(50)));
	ASSERT_TRUE(DoubleEquals(v2.Length(), 10.0));
}

TEST(Vector3D, Normalizes) {
	Vector3<int32_t> v1(60, 42, 88);
	Vector3<int32_t> v2(2, 1, -4);
	Vector3<int32_t> v3(-75, 23, 66);
	Vector3<int32_t> v4(39, -5, 22);
	Vector3<int32_t> v5(-5, -5, -5);
	ASSERT_TRUE(DoubleEquals(v1.Normalize().Length(), 1.0));
	ASSERT_TRUE(DoubleEquals(v2.Normalize().Length(), 1.0));
	ASSERT_TRUE(DoubleEquals(v3.Normalize().Length(), 1.0));
	ASSERT_TRUE(DoubleEquals(v4.Normalize().Length(), 1.0));
	ASSERT_TRUE(DoubleEquals(v5.Normalize().Length(), 1.0));
}
