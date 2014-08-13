#pragma once

#include <gtest/gtest.h>
#include <Utilities/Algebra/Algebra3D.h>
#include <iostream>

using namespace utils;

/********************************************************************************
 * Double epsilon tests
 ********************************************************************************/

TEST(DoubleAlgebra, PerformsCeil) {
	ASSERT_EQ(ECeil(99.5), 100);
	ASSERT_EQ(ECeil(4.9999999999), 5);
	ASSERT_EQ(ECeil(5.0000000001), 5);
	ASSERT_EQ(ECeil(-4.9999), -4);
	ASSERT_EQ(ECeil(5000.0001), 5001);
}

TEST(DoubleAlgebra, PerformsFloor) {
	ASSERT_EQ(EFloor(100.5), 100);
	ASSERT_EQ(EFloor(4.9999999999), 5);
	ASSERT_EQ(EFloor(5.0000000001), 5);
	ASSERT_EQ(EFloor(-4.9999), -5);
	ASSERT_EQ(EFloor(5000.0001), 5000);
}

TEST(DoubleAlgebra, PerformsEquals) {
	ASSERT_TRUE(EEq(1.0, 1.0));
	ASSERT_TRUE(EEq(1.0, 1.0000000001));
	ASSERT_TRUE(EEq(50001.0, 50001.00001));
	ASSERT_TRUE(EEq(-200.5, -200.4999999999));
	ASSERT_FALSE(EEq(0.0, -0.0001));
	ASSERT_FALSE(EEq(-5.0, 5.0));
}

TEST(DoubleAlgebra, PerformsGreaterEqual) {
	ASSERT_TRUE(EGTE(1.0, 1.0));
	ASSERT_TRUE(EGTE(1.0, 1.0000000001));
	ASSERT_TRUE(EGTE(10000.0, 10000.000001));
	ASSERT_TRUE(EGTE(-500, -550));
	ASSERT_FALSE(EGTE(-1, -0.9999));
	ASSERT_FALSE(EGTE(-10, -5));
}

TEST(DoubleAlgebra, PerformsGreater) {
	ASSERT_TRUE(EGT(-500, -550));
	ASSERT_TRUE(EGT(1000, 999.99999));
	ASSERT_FALSE(EGT(1.0, 1.0));
	ASSERT_FALSE(EGT(1.0, 1.0000000001));
	ASSERT_FALSE(EGT(10000.0, 10000.000001));
	ASSERT_FALSE(EGT(-1, -0.9999));
	ASSERT_FALSE(EGT(-10, -5));
}

TEST(DoubleAlgebra, PerformsLessEqual) {
	ASSERT_TRUE(ELTE(1.0, 1.0));
	ASSERT_TRUE(ELTE(1.0, 0.999999999));
	ASSERT_TRUE(ELTE(10000.0, 9999.999999));
	ASSERT_TRUE(ELTE(-500, -450));
	ASSERT_FALSE(ELTE(-1, -1.0001));
	ASSERT_FALSE(ELTE(-5, -10));
}

TEST(DoubleAlgebra, PerformsLess) {
	ASSERT_TRUE(ELT(-500, -450));
	ASSERT_TRUE(ELT(1000, 1000.00001));
	ASSERT_FALSE(ELT(1.0, 1.0));
	ASSERT_FALSE(ELT(1.0, 0.999999999));
	ASSERT_FALSE(ELT(10000.0, 9999.999999));
	ASSERT_FALSE(ELT(-1, -1.0001));
	ASSERT_FALSE(ELT(-10, -15));
}



/********************************************************************************
 * Vector3D epsilon tests
 ********************************************************************************/

TEST(Vector3DAlgebra, PerformsCeil) {
	ASSERT_EQ(ECeil(Point3D(99.999999999)), Vector3D<Int64>(100));
	ASSERT_EQ(ECeil(Point3D(100.000000001)), Vector3D<Int64>(100));
	ASSERT_EQ(ECeil(Point3D(100.0001)), Vector3D<Int64>(101));
}

TEST(Vector3DAlgebra, PerformsFloor) {
	ASSERT_EQ(EFloor(Point3D(99.999999999)), Vector3D<Int64>(100));
	ASSERT_EQ(EFloor(Point3D(100.000000001)), Vector3D<Int64>(100));
	ASSERT_EQ(EFloor(Point3D(99.9999)), Vector3D<Int64>(99));
}

TEST(Vector3DAlgebra, PerformsEquals) {
	ASSERT_TRUE(EEq(Point3D(99.999999999), Point3D(100)));
	ASSERT_FALSE(EEq(Point3D(99.99), Point3D(100)));
}

TEST(Vector3DAlgebra, PerformsGreaterEqual) {
	ASSERT_TRUE(EGTE(Point3D(99.999999999), Point3D(100)));
	ASSERT_TRUE(EGTE(Point3D(101), Point3D(100)));
	ASSERT_FALSE(EGTE(Point3D(99.99), Point3D(100)));
	ASSERT_FALSE(EGTE(Point3D(50), Point3D(100)));
}

TEST(Vector3DAlgebra, PerformsGreater) {
	ASSERT_TRUE(EGT(Point3D(101), Point3D(100)));
	ASSERT_FALSE(EGT(Point3D(99.999999999), Point3D(100)));
	ASSERT_FALSE(EGT(Point3D(99.99), Point3D(100)));
	ASSERT_FALSE(EGT(Point3D(50), Point3D(100)));
}

TEST(Vector3DAlgebra, PerformsLessEqual) {
	ASSERT_TRUE(ELTE(Point3D(100.000000001), Point3D(100)));
	ASSERT_TRUE(ELTE(Point3D(99), Point3D(100)));
	ASSERT_FALSE(ELTE(Point3D(100.001), Point3D(100)));
	ASSERT_FALSE(ELTE(Point3D(150), Point3D(100)));
}

TEST(Vector3DAlgebra, PerformsLess) {
	ASSERT_TRUE(ELT(Point3D(99), Point3D(100)));
	ASSERT_FALSE(ELT(Point3D(100.000000001), Point3D(100)));
	ASSERT_FALSE(ELT(Point3D(100.001), Point3D(100)));
	ASSERT_FALSE(ELT(Point3D(150), Point3D(100)));
}
