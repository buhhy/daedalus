namespace utils {
	/**
	* A standard XYZ vector that can be used as a hashmap key.
	*/
	template<typename T>
	struct Vector3 {
		T X;
		T Y;
		T Z;

		Vector3() {}
		Vector3(T x, T y, T z): X(y), Y(y), Z(z) {}

		inline void Reset(T X, T Y, T Z) {
			this->X = X;
			this->Y = Y;
			this->Z = Z;
		}

		/**
		 * Dot product.
		 */
		template <typename T1>
		inline double Dot(const Vector3<T1> other) const {
			return X * other.X + Y * other.Y + Z * other.Z;
		}

		/**
		 * Cross product.
		 */
		template <typename T1, typename T2>
		inline Vector3<T1> Cross(const Vector3<T2> other) const {
			return Vector3<T1>(
				Y * other.Z - Z * other.Y,
				Z * other.X - X * other.Z,
				X * other.Y - Y * other.X);
		}

		/** Length squared. */
		inline double Length2() const { return X * X + Y * Y + Z * Z; }
		inline double Length() const { return FMath::Sqrt(Length2()); }

		Vector3<double> Normalize() const;
	};

	template<typename T1, typename T2>
	inline Vector3<T1> operator - (const Vector3<T1> & lhs, const Vector3<T2> & rhs) {
		return Vector3<T1>(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z);
	}

	template<typename T1, typename T2>
	inline Vector3<T1> operator * (const Vector3<T1> & lhs, const Vector3<T2> & rhs) {
		return Vector3<T1>(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z);
	}

	template<typename T1, typename T2>
	inline Vector3<T1> operator + (const Vector3<T1> & lhs, const Vector3<T2> & rhs) {
		return Vector3<T1>(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z);
	}

	template<typename T1>
	inline Vector3<T1> operator + (const Vector3<T1> & lhs, const int & rhs) {
		return Vector3<T1>(lhs.X + rhs, lhs.Y + rhs, lhs.Z + rhs);
	}

	template<typename T1>
	inline Vector3<T1> operator + (const Vector3<T1> & lhs, const uint64 & rhs) {
		return Vector3<T1>(lhs.X + rhs, lhs.Y + rhs, lhs.Z + rhs);
	}

	template<typename T1>
	inline Vector3<T1> operator + (const Vector3<T1> & lhs, const int64 & rhs) {
		return Vector3<T1>(lhs.X + rhs, lhs.Y + rhs, lhs.Z + rhs);
	}

	template<typename T1>
	inline Vector3<double> operator * (const Vector3<T1> & lhs, const double & rhs) {
		return Vector3<double>(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs);
	}

	template<typename T1, typename T2>
	inline bool operator == (const Vector3<T1> & lhs, const Vector3<T2> & rhs) {
		return lhs.X == rhs.X && lhs.Y == rhs.Y && lhs.Z == rhs.Z;
	}
}

namespace std {
	template <typename T>
	void hashCombine(uint64 & seed, const T & v) {
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template <typename T>
	struct hash<utils::Vector3<T> > {
		size_t operator()(const utils::Vector3<T> & v) const {
			uint64 seed = 0;
			std::hashCombine(seed, v.X);
			return seed;
		}
	};
}
