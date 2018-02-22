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


#ifndef NEWCASTITANIUM_H
#define NEWCASTITANIUM_H

#include <typeinfo>
#include <string>

#include "NewCast.h"
#include <cxxabi.h>

std::string demangle(const char *symbol);

bool isPolymorphic(const void *sourcePtr);
std::pair<const std::type_info*, const void*> polymorphicProperties(const void *sourcePtr);
std::pair<const abi::__class_type_info*, const void*> polymorphicPropertiesItanium(const void *sourcePtr);

const void *new_cast_itanium(const void *source_ptr, const std::type_info &destination_type); //synonym
const void *new_cast_itanium(const std::type_info& source_type, const void *source_ptr, const std::type_info &destination_type); //synonym

const void *new_cast(const void *source_ptr, const std::type_info &destination_type);
const void *new_cast(const std::type_info &source_type, const void *source_ptr, const std::type_info &destination_type);

const std::type_info& typeOf(const void* sourcePtr);


#endif // NEWCASTITANIUM_H
