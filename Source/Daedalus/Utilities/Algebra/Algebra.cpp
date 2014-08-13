#include <Daedalus.h>
#include "Algebra.h"

#include <Utilities/Algebra/Algebra3D.h>

namespace utils {
	/********************************************************************************
	 * Double epsilon comparison functions
	 ********************************************************************************/
	bool EEq(
		const double v1, const double v2,
		const double maxDiff, const double maxRelDiff
	) {
		// Differences close to 0 cause problems with relative error.
		const double diff = std::abs(v1 - v2);
		if (diff <= maxDiff)
			return true;
		const double max = std::max(v1, v2);
		return diff <= std::abs(max * maxRelDiff);
	}

	bool EGTE(
		const double v1, const double v2,
		const double maxDiff, const double maxRelDiff
	) {
		// Differences close to 0 cause problems with relative error.
		const double diff = v1 - v2;
		if (diff >= -maxDiff)
			return true;
		const double max = std::max(v1, v2);
		return diff >= -std::abs(max * maxRelDiff);
	}

	bool EGT(
		const double v1, const double v2,
		const double maxDiff, const double maxRelDiff
	) {
		// Differences close to 0 cause problems with relative error.
		const double diff = v1 - v2;
		if (diff > maxDiff)
			return true;
		const double max = std::max(v1, v2);
		return diff > std::abs(max * maxRelDiff);
	}

	bool ELTE(
		const double v1, const double v2,
		const double maxDiff, const double maxRelDiff
	) {
		// Differences close to 0 cause problems with relative error.
		const double diff = v1 - v2;
		if (diff <= maxDiff)
			return true;
		const double max = std::max(v1, v2);
		return diff <= std::abs(max * maxRelDiff);
	}

	bool ELT(
		const double v1, const double v2,
		const double maxDiff, const double maxRelDiff
	) {
		// Differences close to 0 cause problems with relative error.
		const double diff = v1 - v2;
		if (diff < -maxDiff)
			return true;
		const double max = std::max(v1, v2);
		return diff < -std::abs(max * maxRelDiff);
	}



	/********************************************************************************
	 * Vector epsilon comparison functions
	 ********************************************************************************/

	Vector3D<Int64> EFloor(const Vector3D<> & vec, const double maxDiff) {
		return Vector3D<Int64>(
			EFloor(vec.X, maxDiff), EFloor(vec.Y, maxDiff), EFloor(vec.Z, maxDiff));
	}

	Vector3D<Int64> ECeil(const Vector3D<> & vec, const double maxDiff) {
		return Vector3D<Int64>(
			ECeil(vec.X, maxDiff), ECeil(vec.Y, maxDiff), ECeil(vec.Z, maxDiff));
	}

	bool EEq(
		const Vector3D<double> v1, const Vector3D<double> v2,
		const double maxDiff, const double maxRelDiff
	) {
		bool test = true;
		for (Uint8 i = 0; i < 3; i++)
			test &= EEq(v1[i], v2[i], maxDiff, maxRelDiff);
		return test;
	}

	bool EGTE(
		const Vector3D<double> v1, const Vector3D<double> v2,
		const double maxDiff, const double maxRelDiff
	) {
		bool test = true;
		for (Uint8 i = 0; i < 3; i++)
			test &= EGTE(v1[i], v2[i], maxDiff, maxRelDiff);
		return test;
	}

	bool EGT(
		const Vector3D<double> v1, const Vector3D<double> v2,
		const double maxDiff, const double maxRelDiff
	) {
		bool test = true;
		for (Uint8 i = 0; i < 3; i++)
			test &= EGT(v1[i], v2[i], maxDiff, maxRelDiff);
		return test;
	}

	bool ELTE(
		const Vector3D<double> v1, const Vector3D<double> v2,
		const double maxDiff, const double maxRelDiff
	) {
		bool test = true;
		for (Uint8 i = 0; i < 3; i++)
			test &= ELTE(v1[i], v2[i], maxDiff, maxRelDiff);
		return test;
	}

	bool ELT(
		const Vector3D<double> v1, const Vector3D<double> v2,
		const double maxDiff, const double maxRelDiff
	) {
		bool test = true;
		for (Uint8 i = 0; i < 3; i++)
			test &= ELT(v1[i], v2[i], maxDiff, maxRelDiff);
		return test;
	}
}
