#include "GlobalFunctions.h"

#include <utility>
#include <typeinfo>



/*
template<class Dest, class Source> void TestCast(Source *in) {
	//	Dest	*normal = dynamic_cast<Dest*>(in),
	Dest *normal = dynamic_cast<Dest*>(in),
		*abi = (Dest*)DynamicWrapper(in,
			static_cast<const __class_type_info*>(&typeid(Source)),
			static_cast<const __class_type_info*>(&typeid(Dest)),
			-1),
		*upcast = (Dest*)UpcastWrapper(in,
			static_cast<const __class_type_info*>(&typeid(Source)),
			static_cast<const __class_type_info*>(&typeid(Dest))),
		*FirstNew = (Dest*)FirstNewCast(in, typeid(Dest));

	//cout << OutputOffset(in, normal)
	//cout << in << "\t" << OutputOffset(in,normal) << "\t\t" << OutputOffset(in, abi) << endl;
	std::ostringstream Output;
	Output << setw(12) << in << "," << setw(12) << FirstNew << "," << setw(12) << normal << "," << setw(12) << abi << "," << setw(12) << upcast;
	cout << Output.str() << endl;
}

*/




