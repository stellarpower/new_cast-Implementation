//          Copyright Benjamin Southall 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#include "GlobalFunctions.h"
#include "Tests.h"
#include "SharedLibrarySideTests.h"

using namespace std;



int main()
{

    std::srand(std::time(0));

    Logger logger(std::cout, VERBOSITY, false, false);

    std::deque<std::tuple<unsigned, unsigned, unsigned>> f;
	

    testPolymorphicProperties(logger);

    testBinaryObjectsSharedLibraryClasses(logger);
    testSharedLibraryObjectsBinaryClasses(logger);
    testDynamicAgainstNewSharedLibrary(logger);

    testDynamicAgainstNewBinary(logger);

    testNewCastInterfaces(logger);

    testDynamicAgainstNewAny(logger);
    testNewAnyForNonClassTypes(logger);


    cout << "\n\n\n\n";

    return !logger.dumpTests();


}

