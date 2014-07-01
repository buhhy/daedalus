#include "gtest/gtest.h"
#include "Algebra2DTests.h"
#include "Algebra3DTests.h"
#include "DelaunayTests.h"

int main(int argc, char ** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
