
//Definitions for 64-bit structures

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
