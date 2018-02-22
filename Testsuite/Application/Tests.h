//          Copyright Benjamin Southall 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)



#ifndef TESTS_H
#define TESTS_H

#include "TestsShared.h"

void testPolymorphicProperties(Logger &logger);


void testBinaryObjectsSharedLibraryClasses(Logger &logger);
void testSharedLibraryObjectsBinaryClasses(Logger &logger);

void testDynamicAgainstNewBinary(Logger &logger);


void testNewCastInterfaces(Logger &logger);


void testDynamicAgainstNewAny(Logger &logger);
void testNewAnyForNonClassTypes(Logger& logger);






#endif // TESTS_H
