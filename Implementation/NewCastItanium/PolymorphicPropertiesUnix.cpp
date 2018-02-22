// Copyright (C) 2000-2018 Free Software Foundation, Inc.
// Copyright (C) 2018 Benjamin Southall
//
// This file is part of the Draft Itanium Implementation of new_cast.
//
// The Draft Itanium Implementation of new_cast is free software;
// you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// The Draft Itanium Implementation of new_cast is distributed
// in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

// Original written by Nathan Sidwell, Codesourcery LLC, <nathan@codesourcery.com>
// This file written by Benjamin Southall, github.com/stellarpower


#include <malloc.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include "GlobalFunctions.h"
#include "libsupc++/tinfo.h"
#include "NewCastItanium.h"


using namespace std;
using namespace abi;

inline bool isMapped(const void *memory){

    int pageSize = sysconf(_SC_PAGESIZE);

    //unsigned char *vec = new unsigned char; //(pageSize + pageSize - 1) / pageSize ];
    unsigned char vec;

    intptr_t pageStart = (intptr_t)memory;
    pageStart -= (pageStart % pageSize);

    if (mincore((void*)pageStart, pageSize, &vec)){
        DEBUG(3, "mincore returned ", strerror(errno),  "( ", errno, ")")
        //delete vec;
        return false;
    }

    //bool res = vec[0];
    //delete vec;

    DEBUG(3, "mincore returned ", strerror(errno),  "( ", errno, ")")

    return vec;
}


template<typename T> T* guard(T* pointer)
{
    if (!isMapped(pointer))
        throw std::runtime_error("The memory address referenced has not been allocated.");
    return pointer;
}


std::pair<const __class_type_info*, const void*> polymorphicPropertiesItanium(const void *sourcePtr)
{

    try {
        DEBUG(2, "Getting vtable for object ", sourcePtr, "... \t");

        const void *objectVTablePtr = *static_cast <const void *const *> (sourcePtr);
        const vtable_prefix *objectPrefix = adjust_pointer <vtable_prefix> (objectVTablePtr, -offsetof (vtable_prefix, origin));


        const void *wholeObject = guard(adjust_pointer <void> (sourcePtr, guard(objectPrefix)->whole_object));
        DEBUG_INLINE(2, "Got whole object... \t");


        const void *wholeObjectVtablePtr = *static_cast <const void *const *> (wholeObject);
        const vtable_prefix *wholeObjectPrefix = guard(adjust_pointer <vtable_prefix> (wholeObjectVtablePtr, -offsetof (vtable_prefix, origin)));
        DEBUG_INLINE(2, "Got its vtable... \t");


        auto* wholeObjectType = guard(wholeObjectPrefix->whole_type);
        DEBUG_INLINE(2, "Got the whole object's type... \t");


        const void *typeVTablePtr = *guard(static_cast <const void *const *> ((const void*)wholeObjectType));
        const vtable_prefix *typePrefix = adjust_pointer <vtable_prefix> (typeVTablePtr, -offsetof (vtable_prefix, origin));
        DEBUG_INLINE(2, "Got the type's vtable... \t");


        auto* typeType = guard(typePrefix)->whole_type;
        DEBUG_INLINE(2, "Got type's type...")

        DEBUG_INLINE(2, "Checking type of the object is an instance of __class_type_info or a subclass\n");

        if ( (*typeType == typeid(__class_type_info)) || (*typeType == typeid(__si_class_type_info)) || (*typeType == typeid(__vmi_class_type_info)) ){

            DEBUG_INLINE(2, "It is! Object ", sourcePtr, " is polymorphic of type ", typeName(wholeObjectType), " with full address ", wholeObject, "\n\n");
            return make_pair(wholeObjectType, wholeObject);

        }

        DEBUG_INLINE(2, "It isn't! Object ", sourcePtr, " is not polymorphic\n\n");
        return make_pair(wholeObjectType, nullptr);


    } catch (std::runtime_error){
        return make_pair(nullptr, nullptr);
    }

}



std::pair<const std::type_info*, const void*> polymorphicProperties(const void *sourcePtr)
{
    auto res = polymorphicPropertiesItanium(sourcePtr);
    return std::make_pair(std::get<0>(res), std::get<1>(res));
}

bool isPolymorphic(const void *sourcePtr)
{
    return polymorphicProperties(sourcePtr).first;
}
