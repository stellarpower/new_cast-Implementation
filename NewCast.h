//          Copyright Benjamin Southall 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef NEWCAST_H
#define NEWCAST_H

#include <typeinfo>
#include <string>
#include <list>

#include "GlobalFunctions.h"

const std::type_info& typeOf(const void* sourcePtr);

std::string demangle(const char *symbol);

std::pair<const std::type_info*, const void*> polymorphicProperties(const void *sourcePtr);

extern int fuzzy;

struct OverloadsLogger{

        static void addOverload(std::string overloadName, bool ignoreNext = false){
            if (!fIgnoreNext)
                fStrings.push_back(overloadName);
            fIgnoreNext = ignoreNext;

        }

        static const std::list<std::string>& get(){
            return fStrings;
        }

        static void clear(){
           fStrings.clear();
        }

    private:
        static bool fIgnoreNext;
        static std::list<std::string> fStrings;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////   Const Pointers


//Use defines not typedefs so as not to confuse autocompletion in IDEs
#define cvp const void*
#define ctir const std::type_info&
#define csp const source*
#define cdp const dest*


// Basic interface. The first version is suitable only for polymorphic types, the second could be supported by non-polymorhpic classes and built-in types, unions etc.
// The first version requires compiler support to be able to extract the runtime type from the void pointer. In practice, this usually means that the vptr is a fixed
// offset from the offset into the object where the pointer points according to the ABI.

                                           cvp    new_cast( ctir source_type, cvp source_ptr, ctir destination_type  ); //C-Bas

                                           cvp    new_cast(                   cvp source_ptr, ctir destination_type  ); //C-Bas-R



// Templates to reduce typing.

template<class source>             inline cvp    new_cast(                   csp source_ptr, ctir destination_type  ); //C-Src

template<class dest>               inline cdp    new_cast( ctir source_type, cvp source_ptr                         ); //C-Dst

template<class dest>               inline cdp    new_cast(                   cvp source_ptr                         ); //C-Dst-R

template<class dest, class source> inline  cdp    new_cast(                   csp source_ptr                         ); //C-SrcDst



template<class source> inline cvp new_cast(csp source_ptr, ctir destination_type) //C-Src
{
    DEBUG_IF(2, OverloadsLogger::addOverload("C-Src", true);)
    return new_cast(typeid(source), source_ptr, destination_type);
}

template<class dest> inline cdp new_cast(ctir source_type, cvp source_ptr)//C-Dst
{
    DEBUG_IF(2, OverloadsLogger::addOverload("C-Dst", true);)
    return reinterpret_cast<cdp>(new_cast(source_type, source_ptr, typeid(dest)));
}

template<class dest> inline cdp new_cast(cvp source_ptr)//C-Dst-R
{
    DEBUG_IF(2, OverloadsLogger::addOverload("C-Dst-R", true);)
    return reinterpret_cast<cdp>(new_cast(source_ptr, typeid(dest)));
}

template<class dest, class source> inline cdp new_cast(csp source_ptr) //C-SrcDst
{
    DEBUG_IF(2, OverloadsLogger::addOverload("C-SrcDst", true);)
    return reinterpret_cast<cdp>(new_cast(typeid(source), source_ptr, typeid(dest)));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////   Non-const Pointers

//Use defines not typedefs so as not to confuse autocompletion in IDEs
#define nvp void*
#define nsp source*
#define ndp dest*


// Basic interface. The first version is suitable only for polymorphic types, the second could be supported by non-polymorhpic classes and built-in types, unions etc.
// The first version requires compiler support to be able to extract the runtime type from the void pointer. In practice, this usually means that the vptr is a fixed
// offset from the offset into the object where the pointer points according to the ABI.

                                           nvp    new_cast( ctir source_type, nvp source_ptr, ctir destination_type  ); //N-Bas
                                           nvp    new_cast(                   nvp source_ptr, ctir destination_type  ); //N-Bas-R



// Templates to reduce typing.

template<class source>              inline nvp    new_cast(                   nsp source_ptr, ctir destination_type  ); //N-Src

template<class dest>                inline ndp    new_cast( ctir source_type, nvp source_ptr                         ); //N-Dst

template<class dest>                inline ndp    new_cast(                   nvp source_ptr                         ); //N-Dst-R

template<class dest, class source>  inline ndp    new_cast(                   nsp source_ptr                         ); //N-SrcDst



inline nvp new_cast(ctir source_type, nvp source_ptr, ctir destination_type) //N-Bas
{
    DEBUG_IF(2, OverloadsLogger::addOverload("N-Bas", true);)
    return const_cast<nvp>(new_cast(source_type, const_cast<cvp>(source_ptr), destination_type));
}

inline nvp new_cast(nvp source_ptr, ctir destination_type) //N-Bas-R
{
    DEBUG_IF(2, OverloadsLogger::addOverload("N-Bas-R", true);)
    return const_cast<nvp>(new_cast(const_cast<cvp>(source_ptr), destination_type));
}


//Templtes

template<class source> inline nvp new_cast(nsp source_ptr, ctir destination_type) //N-Src
{
    DEBUG_IF(2, OverloadsLogger::addOverload("N-Src", true);)
    return new_cast(typeid(source), source_ptr, destination_type);
}

template<class dest> inline ndp new_cast(ctir source_type, nvp source_ptr)//N-Dst
{
    DEBUG_IF(2, OverloadsLogger::addOverload("N-Dst", true);)
    return reinterpret_cast<ndp>(new_cast(source_type, source_ptr, typeid(dest)));
}

template<class dest> inline ndp new_cast(nvp source_ptr)//N-Dst-R
{
    DEBUG_IF(2, OverloadsLogger::addOverload("N-Dst-R", true);)
    return reinterpret_cast<ndp>(new_cast(source_ptr, typeid(dest)));
}

template<class dest, class source> inline ndp new_cast(nsp source_ptr) //N-SrcDst
{
    DEBUG_IF(2, OverloadsLogger::addOverload("N-SrcDst", true);)
    return reinterpret_cast<ndp>(new_cast(typeid(source), source_ptr, typeid(dest)));
}


#endif // NEWCAST_H
