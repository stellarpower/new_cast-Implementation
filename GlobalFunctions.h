//          Copyright Benjamin Southall 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <iostream>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <list>
#include <typeinfo>
#include <regex>


std::string demangle(const char *symbol);

static const auto
    red = "\033[1;31m",
    green = "\033\[1;32m",
    yellow = "\033\[1;33m",
    cyan = "\033\[1;36m",
    blue = "\033\[1;34m",
    normal = "\033[0m";

#define DEBUG(LEVEL, ...) \
    if constexpr (LEVEL <= VERBOSITY) \
        debug(LEVEL, __FILE__, __FUNCTION__, __LINE__,  __VA_ARGS__);

#define DEBUG_INLINE(LEVEL, ...) \
    if constexpr (LEVEL <= VERBOSITY) \
        debugInline(__VA_ARGS__);

#define DEBUG_IF(LEVEL, ...) \
    if constexpr (LEVEL <= VERBOSITY) \
        __VA_ARGS__


template<class... T> inline auto& debug(unsigned int level, std::string file, const char *function, unsigned int line, T... args)
{

    auto forward = file.rfind("/"), backward = file.rfind("\\");
    auto fileName = file.substr(std::max( forward != std::string::npos ? forward : 0, backward != std::string::npos ? backward : 0 ) + 1);

    std::cout << cyan << "DEBUG (" << level << " - " << fileName << ":" << line << " " << function <<  "): ";
    return ( std::cout << ... << args ) << normal;
}

template<class... T> inline auto& debugInline(T... args)
{
    std::cout << cyan;
    return ((std::cout << ... << args) << normal);
}



template<class T, bool b = false> struct PrintTemplate
{
    static constexpr void apply(){
        static_assert(b, "^^ The Template Type was... ^^");
    }
};


const std::type_info& typeOf(const void* sourcePtr);

inline auto typeName(const std::type_info& aType)
{
    return demangle(aType.name());
}

inline auto typeName(const std::type_info* aType)
{
    return demangle(aType->name());
}

template<class T> inline auto typeName()
{
    return demangle(typeid(T).name());
}

inline auto typeNameOf(const void* sourcePtr)
{
    return demangle(typeOf(sourcePtr).name());
}

template<class T> inline auto typeNameOf(const T& source)
{
    return demangle(typeid(source).name());
}



inline std::ptrdiff_t offset(void *base, void *diff)
{
    intptr_t baseI = (intptr_t)base, diffI = (intptr_t)diff;
    if (diffI > baseI){
        intptr_t res = diffI - baseI;
        return res;
    } else {
        intptr_t res = baseI - diffI;
        return -res;
    }

}

inline std::string outputOffset(void *base, void *diff)
{
    std::ostringstream Result;
    Result.width(10);
    if (!base)
        Result << "base is NULL";
    else if (!diff)
        Result << "diff is NULL";
    else
        Result << offset(base, diff);
    return Result.str();
}


inline std::ostream& operator << (std::ostream& stream, void* pointer)
{
    if (pointer)
        stream << (intptr_t)pointer; //FIXME!
    else
        stream << "NULL";
    return stream;
}


#endif // GLOBALFUNCTIONS_H

