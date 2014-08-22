#pragma once

#include <Utilities/Integers.h>

#include <vector>
#include <memory>
#include <string>
#include <new>

namespace utils {
	struct StringException: public std::exception {
	private:
		std::string Message;

	public:
		StringException(const std::string & message) : Message(message) {}
		StringException(const char * && message) : Message(message) {}
		virtual const char * what() const throw() { return Message.c_str(); }
	};

	/**
	 * Equivalent to the Scala Option construct which can be a Some or None value. Because Unreal
	 * doesn't support unrestricted unions yet, we have to use in-place new operator for now.
	 * This implementation is based on the Boost implementation.
	 */
	template <typename T>
	class Option {
	public:
		Option() : bIsInitialized(false) {}

		Option(const T &val) : bIsInitialized(true) { 
			Construct(val);
		}

		Option(const Option<T> & opt) : bIsInitialized(opt.bIsInitialized) {
			if (bIsInitialized)
				Construct(opt.Get());
		}

		Option & operator = (const Option<T> & opt) {
			Destruct();
			bIsInitialized = opt.bIsInitialized;
			if (bIsInitialized)
				Construct(opt.Get());
			return *this;
		}

		~Option() { Destruct(); }

		operator bool () const { return bIsInitialized; }
		T & operator * () { return Get(); }
		const T & operator * () const { return Get(); }
		bool IsValid() const { return bIsInitialized; }

	protected:
		void CheckValid() const {
			if (!bIsInitialized)
				throw StringException("Option::Get: Trying to get value from <None>");
		}

		void * storage() { return Data; }
		const void *storage() const { return Data; }

		T & Get() {
			CheckValid();
			return *static_cast<T *>(storage());
		}

		const T & Get() const {
			CheckValid();
			return *static_cast<const T *>(storage());
		}

		void Destruct() {
			if (bIsInitialized)
				Get().~T();
			bIsInitialized = false;
		}

		void Construct(const T & data) {
			new (storage()) T(data);
		}

		bool bIsInitialized;
		char Data[sizeof(T)];
	};

	template <typename T>
	Option<T> Some(const T & value) { return Option<T>(value); }

	template <typename T>
	Option<T> None() { return Option<T>(); }


	//template <typename T>
	//struct Option {
	//private:
	//	bool HasValue;
	//	std::unique_ptr<T> Opt;

	//public:
	//	Option() : Opt{nullptr}, HasValue{false} {}
	//	Option(const T & val) : Opt{new T(val)}, HasValue{true} {}
	//	Option(T && val) : Opt{new T(val)}, HasValue{true} {}
	//	Option(const Option<T> & other) :
	//		Opt{other.HasValue ? new T(*other.Opt) : nullptr}, HasValue{other.HasValue}
	//	{}

	//	bool IsValid() const { return HasValue; }
	//	T & operator()() {
	//		if (HasValue) return *Opt;
	//		throw StringException("Option::Get: Trying to get value from <None>");
	//	}
	//	
	//	const T & operator()() const {
	//		if (HasValue) return *Opt;
	//		throw StringException("Option::Get: Trying to get value from <None>");
	//	}
	//};
}
