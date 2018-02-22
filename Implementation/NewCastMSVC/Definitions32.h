// Copyright (c) 1993-2018 the Wine project authors (see the file AUTHORS
// for a complete list)
// Copyright (C) 2018 Benjamin Southall
//
// This file is part of the Draft msvc Implementation of new_cast.
//
// Draft msvc Implementation of new_cast is free software;
// you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version. 
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// A copy of the GNU Lesser General Public License is included in the
// Draft msvc Implementation of new_cast distribution 
// in the file COPYING.LIB. If you did not receive this
// copy, write to the Free Software Foundation, Inc., 51 Franklin St,
// Fifth Floor, Boston, MA 02110-1301, USA.


//Definitions for 32-bit structures

typedef void(*vtable_ptr)(void);

struct __type_info
{
	const vtable_ptr *vtable;
	char              *name;        /* Unmangled name, allocated lazily */
	char               mangled[32]; /* Variable length, but we declare it large enough for static RTTI */
};

struct this_ptr_offsets
{
	int         this_offset;   /* offset of base class this pointer from start of object */
	int         vbase_descr;   /* offset of virtual base class descriptor */
	int         vbase_offset;  /* offset of this pointer offset in virtual base class descriptor */
};


struct rtti_base_descriptor32
{
  const __type_info *type_descriptor;
  int num_base_classes;
  this_ptr_offsets offsets;    /* offsets for computing the this pointer */
  unsigned int attributes;
};

struct rtti_base_array32
{
  const rtti_base_descriptor32 *bases[10]; /* First element is the class itself */
};

struct rtti_object_hierarchy32
{
  unsigned int signature;
  unsigned int attributes;
  int array_len; /* Size of the array pointed to by 'base_classes' */
  const rtti_base_array32 *base_classes;
};

struct rtti_object_locator32
{
  unsigned int signature;
  int base_class_offset;
  unsigned int flags;
  const __type_info *type_descriptor;
  const rtti_object_hierarchy32 *type_hierarchy;
};



struct BaseTable32 {
	int zero;
	ptrdiff_t offsetOfVfptr;
};


static inline const vtable_ptr *getVTable(const void *obj)
{
	return *(const vtable_ptr **)obj;
}


inline const rtti_object_locator32 *getObjectLocatorFromVPtr32(const vtable_ptr *vPtr, ptrdiff_t& offset) //Can't return a std::pair with SEH
{
	offset = 0;

	auto locator = reinterpret_cast<const rtti_object_locator32*>(vPtr[-1]);

	if (locator->signature == /*!=*/ 0) //Signature isn't 0, quick check to help ensure safety
		return locator;
	return nullptr;

}

inline const rtti_object_locator32 *getObjectLocator32(const void *cppobj, ptrdiff_t& offset) //Can't return a std::pair with SEH
{
	return getObjectLocatorFromVPtr32(getVTable(cppobj), offset);
}

inline const auto getObjectLocatorFromVPtr(const vtable_ptr *vPtr, ptrdiff_t& offset)
{
	return getObjectLocatorFromVPtr32(vPtr, offset);
}

inline const auto getObjectLocator(const void *object, ptrdiff_t& offset)
{
	return getObjectLocator32(object, offset);
}


typedef BaseTable32 BaseTable;

static inline const BaseTable *getBaseTable(const void *obj)
{
	return *(const BaseTable **)obj;
}


