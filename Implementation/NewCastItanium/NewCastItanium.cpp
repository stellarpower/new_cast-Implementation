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


#include "NewCastItanium.h"
#include "libsupc++/tinfo.h"

#include "GlobalFunctions.h"

using namespace abi;

std::string demangle(const char *symbol){
    int status;
    char * res = abi::__cxa_demangle(symbol, NULL, NULL, &status);
    if (!status){
        std::string a(res);
        free(res);
        return a;
    }
    throw std::bad_alloc();
}

const std::type_info& typeOf(const void* sourcePtr)
{
    const void *vtable_ptr = *static_cast <const void *const *> (sourcePtr);
    const vtable_prefix *prefix = adjust_pointer <vtable_prefix> (vtable_ptr, -offsetof (vtable_prefix, origin));
    return *(prefix->whole_type);
}


const void *new_cast(const void *source_ptr, const type_info &destination_type)
{
    DEBUG_IF(2, OverloadsLogger::addOverload("C-Bas-R");)
    return new_cast_itanium(source_ptr, destination_type);
}

const void *new_cast(const std::type_info &source_type, const void *source_ptr, const std::type_info &destination_type)
{
    DEBUG_IF(2, OverloadsLogger::addOverload("C-Bas");)
    return new_cast_itanium(source_type, source_ptr, destination_type);
}


//FIXME to use non-poly types.

/*std::pair<const __class_type_info*, const void*> getVTableInfo(const void *sourcePtr){
    *//*

      This returns the true type of a polymorphic object, and a pointer to the most derived object

        | VTABLE                                 |
        |----------------------------------------|
        | ptrdiff_t whole_object                 |            Object:
        | const __class_type_info *whole_type;   |            |-------|
        | const void *origin;                    |  <-------  |VPTR   |
        |----------------------------------------|            | ...   |
    *//*

    const void *vtablePtr = *static_cast <const void *const *> (sourcePtr);
    const vtable_prefix *prefix = adjust_pointer <vtable_prefix> (vtablePtr, -offsetof (vtable_prefix, origin));
    //The special beginning area of the vtable for us to feast on.

    const void *wholePtr = adjust_pointer <void> (sourcePtr, prefix->whole_object);
    //Perform pointer adjustment to get the most derived type - effectively same as dynamic_cast<void*>

    const __class_type_info *wholeType = prefix->whole_type;
    //The type_info for the vtable's type, the most derived, i.e. true, type of the object


    const void *wholeVtablePtr = *static_cast <const void *const *> (wholePtr);
    const vtable_prefix *wholePrefix = adjust_pointer <vtable_prefix> (wholeVtablePtr, -offsetof (vtable_prefix, origin));

    if (*wholeType != *wholePrefix->whole_type)
        // Copied from __dynamic_cast in dynacast.cc, if the subobject's whole_type is different from the derived object's whole type, we're in the middle of construction or something funny, so fail now.
        return make_pair(nullptr, nullptr);

    return make_pair(wholeType, wholePtr);

}*/



const void *new_cast_itanium(const void *source_ptr, const type_info &destination_type)
{

    DEBUG(2, "NEW CAST ITANIUM 2-ARG (Object should be polymorphic but we fail safely if not) - object: ", source_ptr, " from ? to ", typeName(destination_type), "\n")

    if (!source_ptr){
        DEBUG(3, "Object is null, return nullptr\n")
        return nullptr;
    }

    auto *destinationClass = dynamic_cast<const abi::__class_type_info*>(&destination_type);
    if (!destinationClass){
        //If not a class type, error out. If it is a class, access the ABI-specific representation for useful goodies.

        DEBUG(3, "destintion_type is not a class type, return nullptr\n")
        return nullptr; //Or do we throw an exception?
    }


    auto vTableResults = polymorphicPropertiesItanium(source_ptr); //getVTableInfo(source_ptr);

    auto wholeType = vTableResults.first;
    auto wholePtr = vTableResults.second;

    if (!wholeType || !wholePtr){
        DEBUG(2, "Object is not polymorphic, failing safely (standad does not dictate this but we can) and returning nullptr\n")
        return nullptr;
    }

    DEBUG(3, "Source type is ", typeName(wholeType), " whole pointer is ", wholePtr, "\t\t")



    if (*wholeType == destination_type){
        // Casting to the exact same type, return the input

        DEBUG(3, "Success, whole object type and dest are the same.\n")
        return wholePtr;
    }

    __class_type_info::__upcast_result res(__vmi_class_type_info::__flags_unknown_mask); //From class_typeinfo.cc, these flags must be useful only for vmi classes.
        if (wholeType->__do_upcast(destinationClass, /*sourcePtr*/ wholePtr, res) && contained_public_p(res.part2dst)){
            DEBUG(3, "Downcast or upcast succeeded!\n")
            return res.dst_ptr;
        }

    DEBUG(3, "Downcast or upcast failed, returning nullptr\n")
    return nullptr;

}

const void *new_cast_itanium(const std::type_info& source_type, const void *source_ptr, const type_info &destination_type)
{
    DEBUG(2, "NEW CAST ITANIUM 3-ARG - object: ", source_ptr, " from ", typeName(source_type), " to ", typeName(destination_type), "\n")

    if (!source_ptr){
        DEBUG(3, "Object is null, return nullptr\n")
        return nullptr;
    }

    if (source_type == destination_type){
        // Casting to the exact same type, return the input
        DEBUG(3, "Success, source_type and destination_type are the same")
        return source_ptr;
    }

    auto *sourceClass = dynamic_cast<const abi::__class_type_info*>(&source_type);
    if (!sourceClass){
        //If not a class type, error out. If it is a class, access the ABI-specific representation for useful goodies.

        DEBUG(3, "source_type is not a class type, undefined behaviour but return nullptr as we can\n")
        return nullptr; //Or do we throw an exception?
    }

    auto *destinationClass = dynamic_cast<const abi::__class_type_info*>(&destination_type);
    if (!destinationClass){
        //If not a class type, error out. If it is a class, access the ABI-specific representation for useful goodies.

        DEBUG(3, "destintion_type is not a class type, return nullptr\n")
        return nullptr; //Or do we throw an exception?
    }


    auto vTableResults = polymorphicPropertiesItanium(source_ptr); //getVTableInfo(source_ptr);

    const __class_type_info* wholeType = vTableResults.first;
    const void* wholePtr = vTableResults.second;

    if (wholePtr && wholeType){ //is Polymorphic, upcast from real type

        DEBUG(3, "Source type is ", typeName(wholeType), " whole pointer is ", wholePtr, "\t\t")

        if (*wholeType == destination_type){
            // Casting to the exact same type, return the input

            DEBUG(3, "Success, whole object type and dest are the same.\n")
            return wholePtr;
        }

        __class_type_info::__upcast_result res(__vmi_class_type_info::__flags_unknown_mask); //From class_typeinfo.cc, these flags must be useful only for vmi classes.
            if (wholeType->__do_upcast(destinationClass, /*sourcePtr*/ wholePtr, res) && contained_public_p(res.part2dst)){
                DEBUG(3, "Downcast or upcast succeeded!\n")
                return res.dst_ptr;
            }

    } else {

        DEBUG(3, "Object is not polymorphic, only upcast possible")

        __class_type_info::__upcast_result res(__vmi_class_type_info::__flags_unknown_mask); //From class_typeinfo.cc, these flags must be useful only for vmi classes.

        //Do upcast only as type is not polymorphic
        if (sourceClass->__do_upcast(destinationClass, source_ptr, res) && contained_public_p(res.part2dst)){
            DEBUG(3, "Downcast or upcast succeeded!\n")
            return res.dst_ptr;
        }
    }


    DEBUG(3, "Upcast failed, return nullptr");
    return nullptr;

}





