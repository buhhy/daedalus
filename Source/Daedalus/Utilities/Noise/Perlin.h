#pragma once

#include <Utilities/DataStructures.h>

namespace utils {
	/*
	 * This is a clean, fast, modern and free Perlin noise class in C++.
	 * Being a stand-alone class with no external dependencies, it is
	 * highly reusable without source code modifications.
	 *
	 * Note:
	 * Replacing the "float" type with "double" can actually make this run faster
	 * on some platforms. A templatized version of PerlinNoise could be useful.
	 *
	 * @author Stefan Gustavson
	 */
	class PerlinNoise {
	private:
		inline double Fade(const double t) const { return t * t * t * (t * (t * 6 - 15) + 10); }

		inline uint64_t FastFloor(const double x) const {
			return x > 0 ? ((uint64_t) x) : ((uint64_t) x - 1);
		}

		inline double Lerp(const double t, const double a, const double b) const {
			return a + t * (b - a);
		}


		static unsigned char perm[];
		float grad(int hash, float x) const;
		float grad(int hash, float x, float y) const;
		float grad(int hash, float x, float y , float z) const;
		float grad(int hash, float x, float y, float z, float t) const;
		
	public:
		PerlinNoise() {}
		~PerlinNoise() {}

	/**
	 * 1D, 2D, 3D and 4D float Perlin noise, SL "noise()"
	 */
		float Generate(float x) const;
		float Generate(float x, float y) const;
		float Generate(float x, float y, float z) const;
		float Generate(float x, float y, float z, float w) const;

	/**
	 * 1D, 2D, 3D and 4D float Perlin periodic noise, SL "pnoise()"
	 */
		float GeneratePeriodic(float x, int px) const;
		float GeneratePeriodic(float x, float y, int px, int py) const;
		float GeneratePeriodic(float x, float y, float z, int px, int py, int pz) const;
		float GeneratePeriodic(
			float x, float y, float z, float w, int px, int py, int pz, int pw) const;
	};

}
