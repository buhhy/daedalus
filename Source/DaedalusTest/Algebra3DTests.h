#pragma once

#include <gtest/gtest.h>
#include <Utilities/Algebra/Algebra3D.h>
#include <Utilities/Constants.h>
#include <iostream>

using namespace utils;

TEST(Vector3D, Constructors) {
	Vector3D<> v1(Vector2D<>(0.5, 0.6), 0.7);
	Vector3D<> v2(0.5, 0.6, 0.7);
	Vector3D<> v3(v2);
	ASSERT_TRUE(v1.X == 0.5 && v1.Y == 0.6 && v1.Z == 0.7);
	ASSERT_TRUE(v2.X == 0.5 && v2.Y == 0.6 && v2.Z == 0.7);
	ASSERT_TRUE(v3.X == 0.5 && v3.Y == 0.6 && v3.Z == 0.7);
}

TEST(Vector3D, PerformsEquality) {
	Vector3D<> v1(0.5, 0.6, 0.7);
	Vector3D<> v2(0.5, 0.6, 0.7);
	Vector3D<> v3(0.6, 0.5, 0.7);
	Vector3D<> v4(0.5, 0.6, 0.6);
	Vector3D<> v5(0.6, 0.6, 0.6);
	ASSERT_TRUE(v1 == v2);
	ASSERT_FALSE(v1 == v3);
	ASSERT_FALSE(v1 == v4);
	ASSERT_FALSE(v1 == v5);
}

TEST(Vector3D, PerformsAddition) {
	Vector3D<int32_t> v1(5, 6, 7);
	Vector3D<int32_t> v2(2, 7, 11);
	ASSERT_TRUE((v1 + v2) == Vector3D<int32_t>(7, 13, 18));
	ASSERT_TRUE((v1 + 4) == Vector3D<int32_t>(9, 10, 11));
}

TEST(Vector3D, PerformsSubtraction) {
	Vector3D<int32_t> v1(5, 6, 7);
	Vector3D<int32_t> v2(2, 7, 11);
	ASSERT_TRUE((v1 - v2) == Vector3D<int32_t>(3, -1, -4));
	ASSERT_TRUE((v1 - 4) == Vector3D<int32_t>(1, 2, 3));
	ASSERT_TRUE(-v1 == Vector3D<int32_t>(-5, -6, -7));
}

TEST(Vector3D, PerformsMultiplication) {
	Vector3D<int32_t> v1(5, 6, 7);
	Vector3D<int32_t> v2(2, 7, 11);
	ASSERT_TRUE((v1 * v2) == Vector3D<int32_t>(10, 42, 77));
	ASSERT_TRUE((v1 * 4) == Vector3D<int32_t>(20, 24, 28));
}

TEST(Vector3D, PerformsDivision) {
	Vector3D<int32_t> v1(6, 42, 99);
	Vector3D<int32_t> v2(2, 7, 11);
	ASSERT_TRUE((v1 / v2) == Vector3D<int32_t>(3, 6, 9));
	ASSERT_TRUE((v1 / 3) == Vector3D<int32_t>(2, 14, 33));
}

TEST(Vector3D, Length) {
	Vector3D<int32_t> v1(3, 4, 5);
	Vector3D<int32_t> v2(10, 0, 0);
	ASSERT_TRUE(DoubleEquals(v1.Length(), std::sqrt(50)));
	ASSERT_TRUE(DoubleEquals(v2.Length(), 10.0));
}

TEST(Vector3D, Normalizes) {
	Vector3D<int32_t> v1(60, 42, 88);
	Vector3D<int32_t> v2(2, 1, -4);
	Vector3D<int32_t> v3(-75, 23, 66);
	Vector3D<int32_t> v4(39, -5, 22);
	Vector3D<int32_t> v5(-5, -5, -5);
	ASSERT_TRUE(DoubleEquals(v1.Normalize().Length(), 1.0));
	ASSERT_TRUE(DoubleEquals(v2.Normalize().Length(), 1.0));
	ASSERT_TRUE(DoubleEquals(v3.Normalize().Length(), 1.0));
	ASSERT_TRUE(DoubleEquals(v4.Normalize().Length(), 1.0));
	ASSERT_TRUE(DoubleEquals(v5.Normalize().Length(), 1.0));
}

void CompareMatrix(const Matrix4D<> & matrix1, const Matrix4D<> & matrix2) {
	for (Uint8 i = 0; i < 4; i++) {
		for (Uint8 j = 0; j < 4; j++)
			ASSERT_TRUE(EEq(matrix1[i][j], matrix2[i][j]));
	}
}

void CompareVector(const Vector3D<> & vec1, const Vector3D<> & vec2) {
	for (Uint8 i = 0; i < 3; i++)
		ASSERT_TRUE(EEq(vec1[i], vec2[i]));
}

TEST(Matrix4D, Inverts) {
	const Matrix4D<> matrix(
		32, 61, 22, -56,
		5, 0, 99, 16,
		.55, 12, 9.1, 1.05,
		29, -11, -5, 69);
	CompareMatrix(matrix.Invert(), Matrix4D<>(
		0.01828432509786019, 0.003969832294097799, -0.07908316426701552, 0.01512235088775322,
		-0.0004086235067736147, -0.007765697810795369, 0.08556363179624513, 0.00016704978561371642,
		0.0003252427179623095, 0.010250205883041182, -0.003540746019955722, -0.002059013313069799,
		-0.007726290910473099, -0.0021637214932228796, 0.04662185483191838, 0.008014410222196));
	CompareMatrix(matrix.Invert().Invert(), matrix);
}

TEST(Matrix4D, Multiply) {
	CompareMatrix(
		CreateRotation(40, AXIS_X) * CreateRotation(30, AXIS_X), CreateRotation(70, AXIS_X));
	CompareMatrix(
		CreateTranslation({ 40, 50, 60 }) * CreateTranslation({ -70, 45, -20 }),
		CreateTranslation({ -30, 95, 40 }));
}

TEST(Matrix4D, TransformRay) {
	const Point3D origin(10, 20, 30);
	const Vector3D<> direction = Vector3D<>(1, 2, 3).Normalize();
	const Ray3D ray(origin, direction);
	const auto transform = CreateTranslation({ 20, 10, -25 }) *
		CreateRotation(14, AXIS_X) * CreateRotation(60, AXIS_Y) * CreateScaling({ 2.0, 1.6, 1.7 });
	const auto transformed = transform * ray;
	ASSERT_TRUE(EEq(transformed.Direction.Length2(), 1));
	CompareVector(transformed.Origin, transform * origin);
	CompareVector(
		transformed.Direction, (GetRotationMatrixFrom(transform) * direction).Normalize());
}
