#pragma once

#include <Utilities/Integers.h>
#include <Utilities/Algebra/Vector3D.h>
#include <Utilities/Algebra/Vector4D.h>
#include <Utilities/Algebra/Matrix4D.h>

#include <vector>
#include <memory>
#include <string>

namespace utils {
	struct None {};

	struct StringException: public std::exception {
	private:
		std::string Message;

	public:
		StringException(const std::string & message) : Message(message) {}
		StringException(const char * && message) : Message(message) {}
		virtual const char * what() const throw() { return Message.c_str(); }
	};

	// Equivalent to the Scala Option construct which can be a Some or None value
	template <typename T>
	struct Option {
	private:
		bool HasValue;
		std::unique_ptr<T> Opt;

	public:
		Option() : Opt{nullptr}, HasValue{false} {}
		Option(const T & val) : Opt{new T(val)}, HasValue{true} {}
		Option(T && val) : Opt{new T(val)}, HasValue{true} {}
		Option(const Option<T> & other) :
			Opt{other.HasValue ? new T(*other.Opt) : nullptr}, HasValue{other.HasValue}
		{}

		bool IsValid() const { return HasValue; }
		T & operator () () {
			if (HasValue) return *Opt;
			throw StringException("Option::Get: Trying to get value from <None>");
		}
		
		const T & operator () () const {
			if (HasValue) return *Opt;
			throw StringException("Option::Get: Trying to get value from <None>");
		}
	};

	// TODO: make actual colour class
	using Colour = Vector3D<Uint8>;

	struct Triangle3D {
		Vector3D<> Point1, Point2, Point3;

		Triangle3D(
			const Vector3D<> & p1,
			const Vector3D<> & p2,
			const Vector3D<> & p3
		) : Point1(p1), Point2(p2), Point3(p3) {}
	};

	// TODO: get rid of this
	struct GridCell {
		float values[8];
		Vector3D<> points[8];

		void Initialize(
			const float blf, const float tlf,
			const float blb, const float tlb,
			const float brf, const float trf,
			const float brb, const float trb
		) {
			values[0] = blf; values[1] = brf; values[2] = brb; values[3] = blb;
			values[4] = tlf; values[5] = trf; values[6] = trb; values[7] = tlb;
			points[0] = Vector3D<>(0, 0, 0);
			points[1] = Vector3D<>(1, 0, 0);
			points[2] = Vector3D<>(1, 1, 0);
			points[3] = Vector3D<>(0, 1, 0);
			points[4] = Vector3D<>(0, 0, 1);
			points[5] = Vector3D<>(1, 0, 1);
			points[6] = Vector3D<>(1, 1, 1);
			points[7] = Vector3D<>(0, 1, 1);
		}
	};
}
