#include <cstring>
#include <excpt.h>
#include <stdexcept>
#include <sstream>
#include <typeinfo>
#include <iostream>

#include <tuple>

#include "GlobalFunctions.h"
#include "NewCast.h"

#ifdef _WIN32
	#include "Definitions32.h"
#else ifdef _WIN64
	#include "Definitions32.h"
#endif

const void *new_cast_msvc(const type_info& source_static_type, const void *object, const type_info& destination_type);
const void *new_cast_msvc(const void *object, const type_info& destination_type);

const void *new_cast(const void *source_ptr, const std::type_info &destination_type)
{
	return new_cast_msvc(source_ptr, destination_type);
}

const void *new_cast(const std::type_info &source_type, const void *source_ptr, const std::type_info &destination_type)
{
	return new_cast_msvc(source_type, source_ptr, destination_type);
}




typedef void(*vtable_ptr)(void);

	/*struct __type_info
	{
		const vtable_ptr *vtable;
		char              *name;        // Unmangled name, allocated lazily 
		char               mangled[32]; // Variable length, but we declare it large enough for static RTTI 
	};

	//FIXME use inbuilt one and get_vtable


	struct this_ptr_offsets
	{
		int         this_offset;   // offset of base class this pointer from start of object *
		int         vbase_descr;   // offset of virtual base class descriptor 
		int         vbase_offset;  // offset of this pointer offset in virtual base class descriptor 
	};
	*/




	std::string demangle(const char *symbol) {
		return symbol;
	}

/*	const std::type_info& typeOf(const void* sourcePtr)
	{
		const vtable_ptr *vp = get_vtable(const_cast<void*>(sourcePtr));
		// const vtable_prefix *prefix = adjust_pointer <vtable_prefix> (vtable_ptr, -offsetof (vtable_prefix, origin));
		// return *(prefix->whole_type);
	}
	*/


	const void* new_cast_msvc(const std::type_info&, const void*, const std::type_info&);
	const void* new_cast_msvc(const void*, const std::type_info&);

	

	template<class T> inline T* pointerOffset(T* pointer, ptrdiff_t offset) {
		return reinterpret_cast<T*>((char*)pointer + offset);
	}


	//compute the this pointer for a base class of a given type 
	static inline const void *get_this_pointer(const this_ptr_offsets *off, const void *object)
	{
		if (!object) return nullptr;

		if (off->vbase_descr >= 0)
		{
			int *offsetPtr;

			// move this ptr to vbase descriptor 
			object = pointerOffset(object, off->vbase_descr);
			
			// and fetch additional offset from vbase descriptor
			offsetPtr = reinterpret_cast<int*>(*(char**)(pointerOffset(object, off->vbase_offset)));
			object = pointerOffset(object, *offsetPtr);
		}

		object = pointerOffset(object, off->this_offset);
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

/*
	struct rtti_base_descriptor32
	{
		const __type_info *type_descriptor;
		int num_base_classes;
		this_ptr_offsets offsets;    // offsets for computing the this pointer 
		unsigned int attributes;
	};

	struct rtti_base_array32
	{
		const rtti_base_descriptor32 *bases[10]; // First element is the class itself 
	};

	struct rtti_object_hierarchy32
	{
		unsigned int signature;
		unsigned int attributes;
		int array_len; // Size of the array pointed to by 'base_classes'
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

	*/

	



	extern "C" {


		const __type_info* __cdecl MSVCRT___RTtypeid32(void *cppobj)
		{
			if (!cppobj)
				return nullptr;

			__try
			{
				ptrdiff_t offset;
				auto objectLocator = getObjectLocator32(cppobj, offset);
				return objectLocator->type_descriptor;
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return nullptr;
				//__leave; //FIXME
			}

		}

		void* __cdecl MSVCRT___RTDynamicCast32(void *cppobj, int unknown,
			__type_info *src, __type_info *dst,
			int do_throw)
		{
			void *ret;

			if (!cppobj) return nullptr;

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
				ptrdiff_t offset;
				const rtti_object_locator32 *obj_locator = getObjectLocator32(cppobj, offset);
				const rtti_object_hierarchy32 *obj_bases = obj_locator->type_hierarchy;
				const rtti_base_descriptor32 * const* base_desc = obj_bases->base_classes->bases;

				ret = nullptr;
				for (int i = 0; i < obj_bases->array_len; i++)
				{
					const __type_info *typ = base_desc[i]->type_descriptor;

					if (!strcmp(typ->mangled, dst->mangled))
					{
						/* compute the correct this pointer for that base class */
						void *this_ptr = (char *)cppobj - obj_locator->base_class_offset;
						//ret = get_this_pointer(&base_desc[i]->offsets, this_ptr);
						break;
					}
				}
				/* VC++ sets do_throw to 1 when the result of a dynamic_cast is assigned
				* to a reference, since references cannot be NULL.
				*/
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return nullptr;
			}
			return ret;
		}


	} //extern "C"


	

	std::tuple<const __type_info*, const void*, const rtti_object_locator32*> polymorphicPropertiesMSVC(const void *sourcePtr)
	{

		if (!sourcePtr)
			throw std::exception();

		__try {


			// The first entry in a polymorphic class should be a vfptr, a pointer to the vtable for functions,
			// or it should be a pointer to a base class table, the second element in this table is the offset of the 
			// vtable for functions.

			//Try for vfptr

			DEBUG_INLINE(2, "Trying to extract vfptr from object", sourcePtr);

			ptrdiff_t offset;
			const rtti_object_locator32 *objectLocator = getObjectLocator(sourcePtr, offset);


			///////////////////////////////////

			auto actualVPtr = getVTable(sourcePtr);
			auto type = reinterpret_cast<const __type_info*>(MSVCRT___RTtypeid32((void*)sourcePtr));
			ptrdiff_t dummy;
			std::cout << type->mangled << std::endl;
			auto tempus = getObjectLocatorFromVPtr(type->vtable, dummy);


			///////////////////////////////////////////////////

			const void *thisPtr = pointerOffset(sourcePtr, offset);

			//Need this (February)?
			thisPtr = pointerOffset(thisPtr, -objectLocator->base_class_offset);

			if (objectLocator->type_descriptor) {
				DEBUG_INLINE(2, "Success, whole object is ", thisPtr, "with type ", ((const std::type_info*)(objectLocator->type_descriptor))->name());
		
				return std::make_tuple(objectLocator->type_descriptor, thisPtr, objectLocator);
			} else {
				DEBUG_INLINE(2, "Whole object is ", thisPtr, "but type is null!");
			}

		}

		__except (EXCEPTION_EXECUTE_HANDLER) {

			__try {
				//Try for vbptr

				DEBUG_INLINE(2, "Failed - trying to treat as pointer to base table");
				auto baseTable = getBaseTable(sourcePtr);

				const void *vfptr = pointerOffset(sourcePtr, baseTable->offsetOfVfptr);

				ptrdiff_t offset;
				const rtti_object_locator32 *objectLocator = getObjectLocator(vfptr, offset);

				const void *thisPtr = pointerOffset(vfptr, offset);
				thisPtr = pointerOffset(vfptr, -objectLocator->base_class_offset);

				if (objectLocator->type_descriptor) {
					DEBUG_INLINE(2, "Success, whole object is ", thisPtr, "with type ", ((const std::type_info*)(objectLocator->type_descriptor))->name());

					return std::make_tuple(objectLocator->type_descriptor, thisPtr, objectLocator);
				}
				else {
					DEBUG_INLINE(2, "Whole object is ", thisPtr, "but type is null!");
				}

			}
			__except (EXCEPTION_EXECUTE_HANDLER) {

			}
		}

		DEBUG_INLINE(2, "Object ", sourcePtr, " is not polymorphic\n\n");
		return std::make_tuple(nullptr, nullptr, nullptr);

	}


	//const void *new_cast_msvc(const type_info& source_static_type, const void *object, const type_info& destination_type);
	


	inline const void *performCast(const void *object, const rtti_object_locator32 * mostDerivedType, const __type_info* destinationType) {

		// Accumulates actual cast work into one function to reduce code duplication.
		// This NEEDS to be inlined as otherwise there is significant unnecessary overhead.

		const rtti_object_hierarchy32 *bases = mostDerivedType->type_hierarchy;
		const rtti_base_descriptor32 * const* baseDesc = bases->base_classes->bases;


		for (int i = 0; i < bases->array_len; i++)
		{
			const __type_info *type = baseDesc[i]->type_descriptor;

			if (!strcmp(type->mangled, destinationType->mangled))
			{
				// compute the correct this pointer for that base class
				const void *this_ptr = pointerOffset(object, -mostDerivedType->base_class_offset);
				return get_this_pointer(&baseDesc[i]->offsets, this_ptr);
			}
		}
		
		return nullptr;
	
	}

	//const void *new_cast_msvc(const void *object, const type_info& destination_type)
	//{

	//	if (!object) return nullptr;
	//	/* To cast an object at runtime:
	//	* 1.Find out the true type of the object from the typeinfo at vtable[-1]
	//	* 2.Search for the destination type in the class hierarchy
	//	* 3.If destination type is found, return base object address + dest offset
	//	*   Otherwise, fail the cast
	//	*/
	//	

	//	//Check that object is polymorphic.
	//	auto properties = polymorphicPropertiesMSVC(object);
	//		
	//	if (std::get<0>(properties) && std::get<1>(properties) && std::get<2>(properties))//and perform cast suing most derived type
	//		return performCast(std::get<0>(properties), std::get<2>(properties), (const __type_info*)(&destination_type));
	//			
	//	//Object isn't polymorphic	
	//	return nullptr;
	//	
	//}

	const void *new_cast_msvc(const type_info& source_type, const void *object, const type_info& destination_type)
	{

		if (!object) return nullptr;
		/* To cast an object at runtime:
		* 1.Find out the true type of the object from the typeinfo at vtable[-1]
		* 2.Search for the destination type in the class hierarchy
		* 3.If destination type is found, return base object address + dest offset
		*   Otherwise, fail the cast
		*/


		//Check that object is polymorphic.
		auto properties = polymorphicPropertiesMSVC(object);

		if (std::get<0>(properties) && std::get<1>(properties) && std::get<2>(properties))
			//and perform cast using most derived type
			return performCast(std::get<1>(properties), std::get<2>(properties), (const __type_info*)(&destination_type));

		auto type = reinterpret_cast<const __type_info*>(&source_type);
		ptrdiff_t dummy;
		std::cout << type->mangled << std::endl;
		auto tempus = getObjectLocatorFromVPtr(type->vtable, dummy);


		//auto locator = reinterpret_cast<const rtti_object_locator32*>((type->vtable)[-1]);




		auto locator = reinterpret_cast<const rtti_object_locator32*>(type->vtable[-1]);

		auto objectLocator = getObjectLocator32(&type->vtable, dummy); //Treat this vtable as if it were a pointer in an object and extract the object locator;

		return performCast(object, objectLocator, reinterpret_cast<const __type_info*>(&destination_type));

		//Object isn't polymorphic	
		return nullptr;

	}


	
	const void *new_cast_msvc(const void *object, const type_info& destination_type)
	{

		if (!object) return nullptr;

		
		/* To cast an object at runtime:
		* 1.Find out the true type of the object from the typeinfo at vtable[-1]
		* 2.Search for the destination type in the class hierarchy
		* 3.If destination type is found, return base object address + dest offset
		*   Otherwise, fail the cast
		*/
		__try
		{

			//Check that object is polymorphic.

			auto properties = polymorphicPropertiesMSVC(object);
			
			const rtti_object_hierarchy32 *bases = std::get<2>(properties)->type_hierarchy;
			const rtti_base_descriptor32 * const* baseDesc = bases->base_classes->bases;


			for (int i = 0; i < bases->array_len; i++)
			{
				const __type_info *type = baseDesc[i]->type_descriptor;

				if (!strcmp(type->mangled, reinterpret_cast<const __type_info*>(&destination_type)->mangled))
				{
					// compute the correct this pointer for that base class
					const void *this_ptr = pointerOffset(object, -std::get<2>(properties)->base_class_offset);
					return get_this_pointer(&baseDesc[i]->offsets, this_ptr);
				}
			}
			
		} __except (EXCEPTION_EXECUTE_HANDLER) {
		
		}

		return nullptr;
	}


	/*

	const void *new_cast_msvc(const type_info& sourceTypee, const void *object, const type_info& destination_type)
	{

		if (!object) return nullptr;

		__try
		{

			//See if object is polymorphic, if it is, ignore sourceType and perofrm polymorphic cast.

			auto properties = polymorphicPropertiesMSVC(object);

			const rtti_object_hierarchy32 *bases = std::get<2>(properties)->type_hierarchy;
			const rtti_base_descriptor32 * const* baseDesc = bases->base_classes->bases;


			for (int i = 0; i < bases->array_len; i++)
			{
				const __type_info *type = baseDesc[i]->type_descriptor;

				if (!strcmp(type->mangled, reinterpret_cast<const __type_info*>(&destination_type)->mangled))
				{
					// compute the correct this pointer for that base class
					const void *this_ptr = pointerOffset(object, -std::get<2>(properties)->base_class_offset);
					return get_this_pointer(&baseDesc[i]->offsets, this_ptr);
				}
			}

		}
		__except (EXCEPTION_EXECUTE_HANDLER) {


			//Object isn't polymorphic, use source_type


		}

		return nullptr;
	}

	*/
	std::pair<const std::type_info*, const void*> polymorphicProperties(const void *sourcePtr)
	{
		auto res = polymorphicPropertiesMSVC(sourcePtr);
		return std::make_pair(reinterpret_cast<const std::type_info*>(std::get<0>(res)), std::get<1>(res));
	}



	const std::type_info& typeOf(const void *object)
	{
		if (!object)
			throw std::exception();

		auto res = polymorphicProperties(object);
		if (res.first)
			return *res.first;
		
		std::ostringstream os;
		os << "Object at address " << object << " is not polymorphic. Runtime type cannot be obtained";
		throw std::exception(os.str().c_str());
	}


//	const void *new_cast_msvc(const type_info& source_static_type, const void *object, const type_info& destination_type) {
//		return MSVCRT___RTDynamicCast32(const_cast<void*>(object), 0, (__type_info*)(&source_static_type), (__type_info*)(&destination_type), false);
//	}


	/*const void *new_cast_msvc(const void *object, const type_info& destination_type) {
		return MSVCRT___RTDynamicCast32(const_cast<void*>(object), 0, (__type_info*)(MSVCRT___RTtypeid32(const_cast<void*>(object))), (__type_info*)(&destination_type), false);
	}*/



#endif //32-bit

#endif //64-bit


