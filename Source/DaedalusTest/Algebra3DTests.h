#pragma once

#include <gtest/gtest.h>
#include <Utilities/Algebra/Algebra3D.h>
#include <Utilities/Constants.h>
#include <iostream>

using namespace utils;

/*************************************************************
 * Vector3D Tests
 *************************************************************/

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



/*************************************************************
 * Matrix Tests
 *************************************************************/

void CompareMatrix(const Matrix4D<> & matrix1, const Matrix4D<> & matrix2) {
	for (Uint8 i = 0; i < 4; i++) {
		for (Uint8 j = 0; j < 4; j++)
			ASSERT_TRUE(EEq(matrix1[i][j], matrix2[i][j]));
	}
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
		CreateRotation(40, AXIS_X) * CreateRotation(30, AXIS_X) * CreateRotation(15, AXIS_X),
		CreateRotation(85, AXIS_X));
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
	ASSERT_TRUE(EEq(transformed.Origin, transform * origin));
	ASSERT_TRUE(EEq(
		transformed.Direction, (GetRotationMatrixFrom(transform) * direction).Normalize()));
}

TEST(Matrix4D, RotationTransformX) {
	const Point3D point(0, 1, 0);
	const auto sqrt2 = std::sqrt(0.5);
	ASSERT_TRUE(EEq(CreateRotation(45, AXIS_X) * point, { 0, sqrt2, sqrt2 }));
	ASSERT_TRUE(EEq(CreateRotation(90, AXIS_X) * point, { 0, 0, 1 }));
	ASSERT_TRUE(EEq(CreateRotation(135, AXIS_X) * point, { 0, -sqrt2, sqrt2 }));
	ASSERT_TRUE(EEq(CreateRotation(180, AXIS_X) * point, { 0, -1, 0 }));
	ASSERT_TRUE(EEq(CreateRotation(270, AXIS_X) * point, { 0, 0, -1 }));
}

TEST(Matrix4D, RotationTransformY) {
	const Point3D point(1, 0, 0);
	const auto sqrt2 = std::sqrt(0.5);
	ASSERT_TRUE(EEq(CreateRotation(45, AXIS_Y) * point, { sqrt2, 0, -sqrt2 }));
	ASSERT_TRUE(EEq(CreateRotation(90, AXIS_Y) * point, { 0, 0, -1 }));
	ASSERT_TRUE(EEq(CreateRotation(135, AXIS_Y) * point, { -sqrt2, 0, -sqrt2 }));
	ASSERT_TRUE(EEq(CreateRotation(180, AXIS_Y) * point, { -1, 0, 0 }));
	ASSERT_TRUE(EEq(CreateRotation(270, AXIS_Y) * point, { 0, 0, 1 }));
}

TEST(Matrix4D, RotationTransformZ) {
	const Point3D point(1, 0, 0);
	const auto sqrt2 = std::sqrt(0.5);
	ASSERT_TRUE(EEq(CreateRotation(45, AXIS_Z) * point, { sqrt2, sqrt2, 0 }));
	ASSERT_TRUE(EEq(CreateRotation(90, AXIS_Z) * point, { 0, 1, 0 }));
	ASSERT_TRUE(EEq(CreateRotation(135, AXIS_Z) * point, { -sqrt2, sqrt2, 0 }));
	ASSERT_TRUE(EEq(CreateRotation(180, AXIS_Z) * point, { -1, 0, 0 }));
	ASSERT_TRUE(EEq(CreateRotation(270, AXIS_Z) * point, { 0, -1, 0 }));
}



/*************************************************************
 * Bounding Box Tests
 *************************************************************/

TEST(OrientedBoundingBox3D, GetsCorrectExtents) {
	OrientedBoundingBox3D bb(
		{ 5, 2, 3 }, { 8, 4, 9 },
		CreateRotation(24, AXIS_X) * CreateRotation(52, AXIS_Y) *
		CreateTranslation({ 24, 30, 15 }));
	ASSERT_TRUE(EEq(bb.GetExtents(), { 1.5, 1.0, 3.0 }));
	ASSERT_TRUE(EEq(bb.GetCentre(), bb.Transform * Vector3D<>(6.5, 3, 6)));
}

TEST(OrientedBoundingBox3D, GetsEnclosingBoundingBox1) {
	OrientedBoundingBox3D bb({ 0, 0, 0 }, { 1, 1, 1 }, CreateRotation(45, AXIS_Z));
	const auto box = bb.GetEnclosingBoundingBox();
	const auto sqrt2 = std::sqrt(2);
	ASSERT_TRUE(EEq(box.MinPoint, { -sqrt2 / 2, 0, 0 }));
	ASSERT_TRUE(EEq(box.MaxPoint, { sqrt2 / 2, sqrt2, 1 }));
}

TEST(OrientedBoundingBox3D, GetsEnclosingBoundingBox2) {
	OrientedBoundingBox3D bb(
		{ 0, 0, 0 }, { 1, 1, 1 },
		CreateTranslation({ 0, 2, 1 }) * CreateRotation(45, AXIS_Z));
	const auto box = bb.GetEnclosingBoundingBox();
	const auto sqrt2 = std::sqrt(2);
	ASSERT_TRUE(EEq(box.MinPoint, { -sqrt2 / 2, 2, 1 }));
	ASSERT_TRUE(EEq(box.MaxPoint, { sqrt2 / 2, sqrt2 + 2, 2 }));
}

TEST(OrientedBoundingBox3D, GetsEnclosingBoundingBox3) {
	OrientedBoundingBox3D bb(
		{ 0, 0, 0 }, { 1, 1, 1 },
		CreateTranslation({ 0, 2, 1 }) * CreateRotation(45, AXIS_Y) * CreateRotation(45, AXIS_Z));
	const auto box = bb.GetEnclosingBoundingBox();
	const auto sqrt2 = std::sqrt(2);
	ASSERT_TRUE(EEq(box.MinPoint, { -0.5, 2, 0.5 }));
	ASSERT_TRUE(EEq(box.MaxPoint, { 0.5 + sqrt2 / 2, sqrt2 + 2, 0.5 + sqrt2 / 2 + 1 }));
}
