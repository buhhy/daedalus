#pragma once

#include <Utilities/Algebra/Algebra2D.h>

namespace utils {
	// TODO: split into component 2D, 3D, 4D generator classes
	/*
	 * This is a clean, fast, modern and free Perlin noise class in C++.
	 * Being a stand-alone class with no external dependencies, it is
	 * highly reusable without source code modifications.
	 *
	 * @author Stefan Gustavson
	 */
	class PerlinNoise2D {
	private:
		Vector2D<Int64> Offset;
		Uint64 Seed;
		
		double GradientAt(int hash, double x, double y) const;

	public:
		PerlinNoise2D(const Uint64 seed) : Offset(0, 0), Seed(seed) {}
		PerlinNoise2D(const Vector2D<Int64> & offset, const Uint64 seed) :
			Offset(offset), Seed(seed)
		{}

		~PerlinNoise2D() {}

		double Generate(double x, double y) const;
		double GeneratePeriodic(double x, double y, int px, int py) const;

		/**
		 * Generates 2D fractal Perlin noise.
		 * @param numOctaves Number of octaves of noise to add.
		 * @param persistence Higher persistence results in higher representation of lower
		 *                    frequencies, lower persistence results in higher representation of
		 *                    higher frequencies.
		 */
		double GenerateFractal(
			const double x, const double y,
			const Uint8 numOctaves, const double persistence) const;

	};

	class PerlinNoise {
	private:
		double GradientAt(int hash, double x) const;
		double GradientAt(int hash, double x, double y , double z) const;
		double GradientAt(int hash, double x, double y, double z, double t) const;
		
	public:
		PerlinNoise() {}
		~PerlinNoise() {}

		/**
		 * 1D, 2D, 3D and 4D double Perlin noise, SL "noise()"
		 */
		double Generate(double x) const;
		double Generate(double x, double y, double z) const;
		double Generate(double x, double y, double z, double w) const;
		/*double GenerateFractal(
			const double x, const double y, const double z,
			const Uint8 numOctaves, const double persistence) const;*/

		/**
		 * 1D, 2D, 3D and 4D double Perlin periodic noise, SL "pnoise()"
		 */
		double GeneratePeriodic(double x, int px) const;
		double GeneratePeriodic(double x, double y, double z, int px, int py, int pz) const;
		double GeneratePeriodic(
			double x, double y, double z, double w, int px, int py, int pz, int pw) const;
	};

}
