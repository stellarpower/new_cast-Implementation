#ifndef SHAREDLIBRARYSIDETESTS_H
#define SHAREDLIBRARYSIDETESTS_H

#include "TestsShared.h"

const std::list<ObjectIF*>& getSharedLibraryPolymorphicObjects();
const std::list<const std::type_info *>& getSharedLibraryPolymorphicTypes();

void testDynamicAgainstNewSharedLibrary(Logger &logger);

#endif // SHAREDLIBRARYSIDETESTS_H
