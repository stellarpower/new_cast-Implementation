#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <list>
#include <wtypes.h>
#include <excpt.h>


namespace One {

	using namespace std;

	std::string Demangle(const char *symbol) {
		return symbol;
	}


	ptrdiff_t Offset(void *base, void *offset) {
		intptr_t baseI = (intptr_t)base, offsetI = (intptr_t)offset;
		if (offsetI > baseI) {
			intptr_t res = offsetI - baseI;
			return res;
		}
		else {
			intptr_t res = baseI - offsetI;
			return -res;
		}

	}

	std::string OutputOffset(void *base, void *offset) {
		std::ostringstream Result;
		Result.width(10);
		if (!base)
			Result << "b NULL";
		else if (!offset)
			Result << "os NULL";
		else
			Result << Offset(base, offset);
		return Result.str();
	}


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

	static inline const vtable_ptr *get_vtable(void *obj)
	{
		return *(const vtable_ptr **)obj;
	}




	//compute the this pointer for a base class of a given type 
	static inline void *get_this_pointer(const this_ptr_offsets *off, void *object)
	{
		if (!object) return NULL;

		if (off->vbase_descr >= 0)
		{
			int *offset_ptr;

			// move this ptr to vbase descriptor 
			object = (char *)object + off->vbase_descr;
			// and fetch additional offset from vbase descriptor 
			offset_ptr = (int *)(*(char **)object + off->vbase_offset);
			object = (char *)object + *offset_ptr;
		}

		object = (char *)object + off->this_offset;
		return object;
	}


#ifdef _WIN64


	struct cxx_type_info64
	{
		UINT flags;
		unsigned int type_info;
		this_ptr_offsets offsets;
		unsigned int size;
		unsigned int copy_ctor;
	};



	struct cxx_type_info_table64
	{
		UINT count;
		unsigned int info[3];
	};

	struct rtti_object_hierarchy64
	{
		unsigned int signature;
		unsigned int attributes;
		int array_len; /* Size of the array pointed to by 'base_classes' */
		unsigned int base_classes;
	};

	typedef struct rtti_object_hierarchy64 rtti_object_hierarchy64;



	struct rtti_object_locator64
	{
		unsigned int signature;
		int base_class_offset;
		unsigned int flags;
		unsigned int type_descriptor;
		unsigned int type_hierarchy;
		unsigned int object_locator;
	};

	typedef struct rtti_object_locator64 rtti_object_locator64;

	static inline const rtti_object_locator64 *get_obj_locator64(void *cppobj)
	{
		const vtable_ptr *vtable = get_vtable(cppobj);
		return (const rtti_object_locator64 *)vtable[-1];
	}

	struct rtti_base_descriptor64
	{
		unsigned int type_descriptor;
		int num_base_classes;
		this_ptr_offsets offsets;    /* offsets for computing the this pointer */
		unsigned int attributes;
	};

	struct rtti_base_array64
	{
		unsigned int bases[10]; /* First element is the class itself */
	};


	extern "C"{

		const __type_info* CDECL MSVCRT___RTtypeid64(void *cppobj)
		{
			if (!cppobj)
				return NULL;

			__try
			{
				const rtti_object_locator64 *obj_locator = get_obj_locator64(cppobj);
				char *base;

				if (obj_locator->signature == 0)
					base = (char*)RtlPcToFileHeader((void*)obj_locator, (void**)&base);
				else
					base = (char*)obj_locator - obj_locator->object_locator;

				return (__type_info*)(base + obj_locator->type_descriptor);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return NULL;
			}

		}




		void* CDECL MSVCRT___RTDynamicCast64(void *cppobj, int unknown,
			__type_info *src, __type_info *dst,
			int do_throw)
		{
			void *ret;

			if (!cppobj) return NULL;

			//TRACE("obj: %p unknown: %d src: %p %s dst: %p %s do_throw: %d)\n",
			//	cppobj, unknown, src, dbgstr_type_info(src), dst, dbgstr_type_info(dst), do_throw);


			__try
			{
				const rtti_object_locator64 *obj_locator = get_obj_locator64(cppobj);
				const rtti_object_hierarchy64 *obj_bases;
				const rtti_base_array64 *base_array;
				char *base;

				//if (TRACE_ON(msvcrt)) dump_obj_locator(obj_locator);

				if (obj_locator->signature == 0)
					base = (char*)RtlPcToFileHeader((void*)obj_locator, (void**)&base);
				else
					base = (char*)obj_locator - obj_locator->object_locator;

				obj_bases = (const rtti_object_hierarchy64*)(base + obj_locator->type_hierarchy);
				base_array = (const rtti_base_array64*)(base + obj_bases->base_classes);

				ret = NULL;
				for (int i = 0; i < obj_bases->array_len; i++)
				{
					const rtti_base_descriptor64 *base_desc = (const rtti_base_descriptor64*)(base + base_array->bases[i]);
					const __type_info *typ = (const __type_info*)(base + base_desc->type_descriptor);

					if (!strcmp(typ->mangled, dst->mangled))
					{
						void *this_ptr = (char *)cppobj - obj_locator->base_class_offset;
						ret = get_this_pointer(&base_desc->offsets, this_ptr);
						break;
					}
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return NULL;
			}
			return ret;
		}


	} //extern "C"

	void *new_cast_implementation(const type_info& source_static_type, const void *object, const type_info& destination_type) {
		return MSVCRT___RTDynamicCast64(const_cast<void*>(object), 0, (__type_info*)(&source_static_type), (__type_info*)(&destination_type), false);
	}

	const type_info& typeOf(const void* sourcePtr) {
		if (const __type_info* ret = MSVCRT___RTtypeid64(const_cast<void*>(sourcePtr)))
			return *(const type_info*)(ret);
		throw exception(); //
						   /*
						   /
						   /
						   /
						   /
						   / FIXME - correct exception type
						   /
						   /
						   /
						   */

						   ////
	}

#else
	#ifdef _WIN32


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

	static inline const rtti_object_locator32 *get_obj_locator32(void *cppobj)
	{
		const vtable_ptr *vtable = get_vtable(cppobj);
		return (const rtti_object_locator32 *)vtable[-1];
	}




	extern "C" {


		const __type_info* __cdecl MSVCRT___RTtypeid32(void *cppobj)
		{
			if (!cppobj)
				return NULL;

			__try
			{
				const rtti_object_locator32 *obj_locator = get_obj_locator32(cppobj);
				return obj_locator->type_descriptor;
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return NULL;
				//__leave; //FIXME
			}

		}

		void* __cdecl MSVCRT___RTDynamicCast32(void *cppobj, int unknown,
			__type_info *src, __type_info *dst,
			int do_throw)
		{
			void *ret;

			if (!cppobj) return NULL;

			/*TRACE("obj: %p unknown: %d src: %p %s dst: %p %s do_throw: %d)\n",
			cppobj, unknown, src, dbgstr_type_info(src), dst, dbgstr_type_info(dst), do_throw);
			*/

			/* To cast an object at runtime:
			* 1.Find out the true type of the object from the typeinfo at vtable[-1]
			* 2.Search for the destination type in the class hierarchy
			* 3.If destination type is found, return base object address + dest offset
			*   Otherwise, fail the cast
			*
			* FIXME: the unknown parameter doesn't seem to be used for anything
			*/
			__try
			{
				const rtti_object_locator32 *obj_locator = get_obj_locator32(cppobj);
				const rtti_object_hierarchy32 *obj_bases = obj_locator->type_hierarchy;
				const rtti_base_descriptor32 * const* base_desc = obj_bases->base_classes->bases;

				ret = NULL;
				for (int i = 0; i < obj_bases->array_len; i++)
				{
					const __type_info *typ = base_desc[i]->type_descriptor;

					if (!strcmp(typ->mangled, dst->mangled))
					{
						/* compute the correct this pointer for that base class */
						void *this_ptr = (char *)cppobj - obj_locator->base_class_offset;
						ret = get_this_pointer(&base_desc[i]->offsets, this_ptr);
						break;
					}
				}
				/* VC++ sets do_throw to 1 when the result of a dynamic_cast is assigned
				* to a reference, since references cannot be NULL.
				*/
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return NULL;
			}
			return ret;
		}


	} //extern "C"

	void *new_cast_implementation(const type_info& source_static_type, const void *object, const type_info& destination_type) {
		return MSVCRT___RTDynamicCast32(const_cast<void*>(object), 0, (__type_info*)(&source_static_type), (__type_info*)(&destination_type), false);
	}

	const type_info& typeOf(const void* sourcePtr) {
		if (const __type_info* ret = MSVCRT___RTtypeid32(const_cast<void*>(sourcePtr)))
			return *(const type_info*)(ret);
		throw exception(); //
		/*
		/
			/
			/
			/
			/ FIXME - correct exception type
			/
			/
			/
			*/

		////
	}

#endif //32-bit

#endif //64-bit


void *new_cast(const type_info& source_static_type, const void *object, const type_info& destination_type) {
	return new_cast_implementation(source_static_type, object, destination_type);
}



} //namespace One

#endif // FUNCTIONS_H
