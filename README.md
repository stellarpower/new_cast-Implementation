# Draft Implementation of new_cast

### This repository contains a sample implementation for new_cast (later to be renamed), a feature proposal to the C++ Standards Committee.

##### This repository is part of the larger [new_cast repository](https://github.com/stellarpower/new_cast). You can find out more about the proposal there.

### What is new_cast?

new_cast is a cast similar to `dynamic_cast`, except that it uses `std::type_info` references instead of static type information to fix the source and destination types, and it operates on a void pointer  rather than a typed pointer. Otherwise it more or less works the same as `dynamic_cast`.

One important use case of `new_cast` is to provide heterogeneous storage - that is, containers that may store objects of any type at runtime, and that can provide type-safe access to those objects later. This implementation includes a modification of the `std::any` class to improve its handling of objects - when accessing the stored object, if the stored type does not match the desired type exactly, `any` will use `new_cast`to perform an upcast or downcast on the object and return an appropriate subobject, pointer or reference.


### What is provided in this implementation?
This repository provides the full sample implementation of new_cast for the Itanium ABI on various platforms, and the Microsoft (msvc) ABI for Microsoft Windows. The Itanium implementation supports the full features detailed in the proposal document, however at this time, the Microsoft implementation does not permit casting of non-polymorphic objects, due to limitations in the type information emitted by the compiler for these objects.

Also provided are modifications of the `std::any` class to support the improvements offerd by `new_cast`. A version for libstdc++, the g++ implementation of the standard library, is provided currently and the versions for libc++ and msvcrt will be added in the near future.

In addition to these, a comprehensive testsuite is provided, containing in the region of 5000 automatically generated tests that provide a thorough testing of the implementtaion of `new_cast` and the improved `any`. 

### What Licence is the code released under?
The code in this project is best considered as several free-standing units that are linked together. Due to the fact that the project modifies and draws upon code from different sources, the source code must be released under different licences.

The Itanium implementation of `new_cast` uses source from g++'s libsupc++, the C++ runtime support library, which is released under version 3 of the GPL with a runtime exception. Therefore, the Itanium implementation is also released under the terms of this licence. The case is the same for the g++ implementation of `any`.

The Windows implementation of `new_cast` is based upon the implementation of `__RTDynamicCast` provided by the Wine project, and so this section of the project is released under the terms of the LGPL v2.1 (or later).

Finally, all other code is released under the Boost License, as this is a more flexible option to allow ease of adoption.

This is summarised below:

File | Licence
------------ | -------------
/Any-g++.h | GPLv3 with runtime exception
/Implementation/Itanium/* | GPLv3 with runtime exception
/Implementation/MSVC/* | LGPLv2.1 or later version
All other files | Boost

### What is Required to Build it?
#### The build system is based on CMake
You should only need CMake and a suitable *recent* compiler, as the code makes use of C++17 features. So far all code has been tested on g++-7 and clang-5 on Linux and 32-bit msvc 2017 on Windows, however it is hoped that more platforms and compilers can be added to this list over time.

The build is divided into separate folders, for example, in order just to compile the implementation, and not the testsuite, run cmake in the Implementation folder.

