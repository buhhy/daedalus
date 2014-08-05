#pragma once

#include <Utilities/Integers.h>

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
}
