
#ifdef COMPILER_MSVC

//#include "Any-msvc.h"
#include <any>

namespace Any {
	typedef std::any any;
	typedef std::bad_any_cast bad_any_cast;

	template<class T, class U> auto any_cast(U u) {
		return std::any_cast<T>(u);
	}

	template<class T, class U> T new_any_cast(U) {
		throw bad_any_cast();
	}
}

#endif
#ifdef COMPILER_GNU

#include "Any-g++.h"

#endif
