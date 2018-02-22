//          Copyright Benjamin Southall 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)



#include <vector>
#include <algorithm>
#include <sstream>

#include "Tests.h"
#include "Classes.h"
#include "TupleUtils.h"
#include "SharedLibrarySideTests.h"
#include "ObjectManager.h"

#include "NewCast.h"


static ObjectManager manager;

void testPolymorphicProperties(Logger& logger){

    logger.startTest(__FUNCTION__,
         "       This function tests that the PolymorphicProperties function returns the correct information.\n"
         "       PolymorphicProperties is used to find out safely whether a void* points to a polymorphic object, in\n"
         "       a platform-specific way, and also finds the most derived type.\n"
    );

    bindTuple<TestPolymorphicPropertiesPolymorphic,
            Polymorphic::Normal::Base, Polymorphic::Normal::Middle, Polymorphic::Normal::Derived,
            Polymorphic::MI::Circle, Polymorphic::MI::Square, Polymorphic::MI::RoundedSquare,
            Polymorphic::Virtual::Base, Polymorphic::Virtual::Middle, Polymorphic::Virtual::Derived, Polymorphic::Virtual::FinDerived,
            Polymorphic::Virtual::MI::Alpha, Polymorphic::Virtual::MI::Beta, Polymorphic::Virtual::MI::Gamma, Polymorphic::Virtual::MI::Delta
    >(manager.getPolymorphicObjects(), logger);

    bindTuple<TestPolymorphicPropertiesNonPolymorphic,
            NonPolymorphic::Normal::Base, NonPolymorphic::Normal::Middle, NonPolymorphic::Normal::Derived,
            NonPolymorphic::MI::Circle, NonPolymorphic::MI::Square, NonPolymorphic::MI::RoundedSquare
    >(manager.getNonPolymorphicObjects(), logger);

    logger.endTest();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void testBinaryObjectsSharedLibraryClasses(Logger &logger)
{

    logger.startTest(__FUNCTION__,
         "       Test objects created in the Application binary, casting against classes compiled separately within a shared library.\n"
         "       The casts ae performed in the binary?"
    );

    auto output = [&logger](const char *function, const void *internal, const void *external){

        if (internal != external){
            logger.fail() << function << " - Library Type: " << internal << " - Binary Type: " << external << endl;
        } else {

            if (internal)
                logger.pass() << function << " - Library Type: " << internal << " - Binary Type: " << external << endl;
            else
                logger.bore() << function << " - Library Type: " << internal << " - Binary Type: " << external << endl;
        }


    };

    testObjectsAgainstTwoTypes( manager.getPolymorphicObjectIFs(), manager.getPolymorphicTypes(), getSharedLibraryPolymorphicTypes(), output, logger);

    logger.endTest();

}

/////////////////////////////////////////////////////////////////////////////

void testSharedLibraryObjectsBinaryClasses(Logger& logger)
{
    logger.startTest(__FUNCTION__,
         "       Test objects created in the Application binay, casting against classes compiled separately within a shared library.\n"
         "       The casts ae performed in the binary?"
    );


    auto output = [&logger](const char *function, const void *internal, const void *external){

        if (internal != external){
            logger.fail() << function << " - Library Type: " << internal << " - Binary Type: " << external << endl;
        } else {

            if (internal)
                logger.pass() << function << " - Library Type: " << internal << " - Binary Type: " << external << endl;
            else
                logger.bore() << function << " - Library Type: " << internal << " - Binary Type: " << external << endl;
        }


    };

    testObjectsAgainstTwoTypes(getSharedLibraryPolymorphicObjects(), manager.getPolymorphicTypes(), getSharedLibraryPolymorphicTypes(),  output, logger);

    logger.endTest();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void testDynamicAgainstNewBinary(Logger& logger)
{
    // This test ensures that dynamic_cast and new_ cast always return the same value,
    // except where a dynamic_cast is undefined. This is enabled by using enable_if
    // inside can_dynamic_cast, so dynamic_cast_if_can returns null if the cast is not well defined.
    // This, however, is abstracted into the Object class' template functions.

    logger.startTest(__FUNCTION__,
         "       This function tests that dynamic_cast and new_cast always return the same value, where such a test can be performed.\n"
         "       We use the 3-argument and 2-argument forms of new_cast on both polymorphic ans non-polymorphic objects.\n"
         "       The 2-argument form should fail on non-polymorphic objects and in this implementation we can return null without a crash.\n"
         "       Casting to a class completely outside of an object's hierarchy is safe with new_cast but is undefined with dynamic_cast,\n"
         "       therefore we use a template that returns null before an illegal cast is made, and only perform valid up-, down- and cross-casts.\n"
         "       As static type information is needed, the type information cannot be passed across the library boundary.\n"
         "       These casts are performed in the binary on objects in the binary."
    );


    bindTuple<TestCastsEqual,

            Polymorphic::Normal::Base, Polymorphic::Normal::Middle, Polymorphic::Normal::Derived,
            Polymorphic::MI::Circle, Polymorphic::MI::Square, Polymorphic::MI::RoundedSquare,
            Polymorphic::Virtual::Base, Polymorphic::Virtual::Middle, Polymorphic::Virtual::Derived, Polymorphic::Virtual::FinDerived,
            Polymorphic::Virtual::MI::Alpha, Polymorphic::Virtual::MI::Beta, Polymorphic::Virtual::MI::Gamma, Polymorphic::Virtual::MI::Delta,

            NonPolymorphic::Normal::Base, NonPolymorphic::Normal::Middle, NonPolymorphic::Normal::Derived,
            NonPolymorphic::MI::Circle, NonPolymorphic::MI::Square, NonPolymorphic::MI::RoundedSquare

    >(manager.getAllObjects(), logger);

    logger.endTest();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



template<class Source, class Dest, class Logger> void checkInterfaces(Source object, Logger& logger) {

	
    typedef typename std::remove_pointer<Source>::type SourceClass;
    typedef typename std::remove_pointer<Dest>::type DestClass;

	std::list<const void*> pointers;
    std::list<std::string> expectedOverloads;

    OverloadsLogger::clear();


    // With const

    const void *cVoidThree = new_cast(typeid(SourceClass), (const void*)object, typeid(DestClass));
    pointers.push_back(cVoidThree);
    expectedOverloads.push_back("C-Bas");

    if constexpr (std::is_polymorphic_v<Source>){
        const void *cVoidTwo = new_cast((void*)object, typeid(DestClass));
        pointers.push_back(cVoidTwo);
        expectedOverloads.push_back("C-Bas-R");
    }

    const void *cSourceTemplate = new_cast(const_cast<const SourceClass*>(object), typeid(DestClass));
    expectedOverloads.push_back("C-Src");
    pointers.push_back(cSourceTemplate);

    const DestClass* cDestTemplateThree = new_cast<DestClass> (typeid(SourceClass), const_cast<const SourceClass*>(object));
    expectedOverloads.push_back("C-Dst");
    pointers.push_back(cDestTemplateThree);

    if constexpr (std::is_polymorphic_v<Source>){
        const DestClass* cDestTemplateTwo = new_cast<DestClass> ((const void*)object);
        expectedOverloads.push_back("C-Dst-R");
        pointers.push_back(cDestTemplateTwo);
    }

    const DestClass* cBothTemplate = new_cast<DestClass> (const_cast<const SourceClass*>(object));
    expectedOverloads.push_back("C-SrcDst");
    pointers.push_back(cBothTemplate);



    //Without const

    const void *nVoidThree = new_cast(typeid(SourceClass), (void*)object, typeid(DestClass));
    pointers.push_back(nVoidThree);
    expectedOverloads.push_back("N-Bas");

    if constexpr (std::is_polymorphic_v<Source>){
        const void *nVoidTwo = new_cast((void*)object, typeid(DestClass));
        pointers.push_back(nVoidTwo);
        expectedOverloads.push_back("N-Bas-R");
    }

    const void *nSourceTemplate = new_cast(object, typeid(DestClass));
    expectedOverloads.push_back("N-Src");
    pointers.push_back(nSourceTemplate);

    Dest nDestTemplateThree = new_cast<DestClass> (typeid(SourceClass), object);
    expectedOverloads.push_back("N-Dst");
    pointers.push_back(nDestTemplateThree);

    if constexpr (std::is_polymorphic_v<Source>){
        Dest nDestTemplateTwo = new_cast<DestClass> ((void*)object);
        expectedOverloads.push_back("N-Dst-R");
        pointers.push_back(nDestTemplateTwo);
    }

    Dest nBothTemplate = new_cast<DestClass> (object);
    expectedOverloads.push_back("N-SrcDst");
    pointers.push_back(nBothTemplate);



    auto overloads = OverloadsLogger::get();

    auto p = pointers.cbegin();
    auto e = expectedOverloads.cbegin();
    auto o = overloads.cbegin();

    for ( ; p != pointers.cend(); p++, e++, o++ ){

        std::ostringstream errorString;

        auto correctCast = *p == static_cast<Dest>(object), correctOverload = *e == *o;

        if ( !correctCast )
            errorString << " new_cast returned incorrect address - expected " << static_cast<Dest>(object) << " but got " << *p << ".";
        if (!correctOverload)
            errorString << " Incorrect overload called, expected " << *e << " but got " << *o << ".";

        if (correctCast && correctOverload)
            logger.pass();
        else
            logger.fail() << "casting from " << typeName<Source>() << " to " << typeName<Dest>() << errorString.str() << endl;

    }

}

void testNewCastInterfaces(Logger& logger)
{
    logger.startTest(__FUNCTION__,
         "       This test ensures the correct specification of the numerous interfaces to new_cast.\n"
         "       It checks const correctness and ensures that the right overloads and templates are available.\n"
         "       The actual process of calling the different overloads is not checked - as if the correct return value\n"
         "       is received and the compiler doesn't complain, the correct interface must have been provided.\n"
    );


    unsigned passes = 0, failures = 0;

	auto* pPolyMiddle = new Polymorphic::Normal::Middle;
	const auto* cpPolyMiddle = pPolyMiddle;

	auto* pPolyRoundedSquare = new Polymorphic::MI::RoundedSquare;
	const auto* cpPolyRoundedSquare = pPolyRoundedSquare;

	auto* pNonPolyMiddle = new NonPolymorphic::Normal::Middle;
	const auto* cpNonPolyMiddle = pNonPolyMiddle;

	auto* pNonPolyRoundedSquare = new NonPolymorphic::MI::RoundedSquare;
	const auto* cpNonPolyRoundedSquare = pNonPolyRoundedSquare;


    checkInterfaces<Polymorphic::Normal::Middle*, Polymorphic::Normal::Base*>(pPolyMiddle, logger);
    checkInterfaces<Polymorphic::MI::RoundedSquare*, Polymorphic::MI::Square*>(pPolyRoundedSquare, logger);
    checkInterfaces<NonPolymorphic::Normal::Middle*, NonPolymorphic::Normal::Base*>(pNonPolyMiddle, logger);
    checkInterfaces<NonPolymorphic::MI::RoundedSquare*, NonPolymorphic::MI::Square*>(pNonPolyRoundedSquare, logger);
	

    OverloadsLogger::clear();

    if (!failures)
        logger.pass(passes) << "All interfaces return same address " << endl;
    else
        logger.fail(failures) << "Not all interfaces return same address " << endl;

	delete pPolyMiddle;
	delete pPolyRoundedSquare;
	delete pNonPolyMiddle;
	delete pNonPolyRoundedSquare;

    logger.endTest();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void testDynamicAgainstNewAny(Logger& logger)
{
    // This test ensures that dynamic_cast and new_ cast always return the same value,
    // except where a dynamic_cast is undefined. This is enabled by using enable_if
    // inside can_dynamic_cast, so dynamic_cast_if_can returns null if the cast is not well defined.
    // This, however, is abstracted into the Object class' template functions.

    logger.startTest(__FUNCTION__,
         "       This function tests that dynamic_cast and any_cast always return the same value, where such a test can be performed.\n"
         "       This test uses an any object created from an object and a pointer, and tests both against T and T*\n"
    );

    bindTuple<TestNewAnyPointerAndDynamic,
            Polymorphic::Normal::Base, Polymorphic::Normal::Middle, Polymorphic::Normal::Derived,
            Polymorphic::MI::Circle, Polymorphic::MI::Square, Polymorphic::MI::RoundedSquare,
            Polymorphic::Virtual::Base, Polymorphic::Virtual::Middle, Polymorphic::Virtual::Derived, Polymorphic::Virtual::FinDerived,
            Polymorphic::Virtual::MI::Alpha, Polymorphic::Virtual::MI::Beta, Polymorphic::Virtual::MI::Gamma, Polymorphic::Virtual::MI::Delta,

            NonPolymorphic::Normal::Base, NonPolymorphic::Normal::Middle, NonPolymorphic::Normal::Derived,
            NonPolymorphic::MI::Circle, NonPolymorphic::MI::Square, NonPolymorphic::MI::RoundedSquare

    >(manager.getPolymorphicObjects(), logger);

    bindTuple<TestNewAnyObjectAndDynamic,
            Polymorphic::Normal::Base, Polymorphic::Normal::Middle, Polymorphic::Normal::Derived,
            Polymorphic::MI::Circle, Polymorphic::MI::Square, Polymorphic::MI::RoundedSquare,
            Polymorphic::Virtual::Base, Polymorphic::Virtual::Middle, Polymorphic::Virtual::Derived, Polymorphic::Virtual::FinDerived,
            Polymorphic::Virtual::MI::Alpha, Polymorphic::Virtual::MI::Beta, Polymorphic::Virtual::MI::Gamma, Polymorphic::Virtual::MI::Delta,

            NonPolymorphic::Normal::Base, NonPolymorphic::Normal::Middle, NonPolymorphic::Normal::Derived,
            NonPolymorphic::MI::Circle, NonPolymorphic::MI::Square, NonPolymorphic::MI::RoundedSquare

    >(manager.getNonPolymorphicObjects(), logger);


     logger.endTest();

}


void testNewAnyForNonClassTypes(Logger& logger)
{
    logger.startTest(__FUNCTION__,
         "       Ensure that new_any_cast works as std::any for non-class types, and also that\n"
         "       when the any contains a non-class, it fails to cast to a class, and vice versa.\n"
    );

    bindTuple<TestNewAnyAgainstCurrentAny,
            Polymorphic::Normal::Base, Polymorphic::Normal::Middle, Polymorphic::Normal::Derived,
            Polymorphic::MI::Circle, Polymorphic::MI::Square, Polymorphic::MI::RoundedSquare,
            Polymorphic::Virtual::Base, Polymorphic::Virtual::Middle, Polymorphic::Virtual::Derived, Polymorphic::Virtual::FinDerived,
            Polymorphic::Virtual::MI::Alpha, Polymorphic::Virtual::MI::Beta, Polymorphic::Virtual::MI::Gamma, Polymorphic::Virtual::MI::Delta,

            NonPolymorphic::Normal::Base, NonPolymorphic::Normal::Middle, NonPolymorphic::Normal::Derived,
            NonPolymorphic::MI::Circle, NonPolymorphic::MI::Square, NonPolymorphic::MI::RoundedSquare,

            NonClass::Enum, NonClass::ClassEnum, NonClass::ClassEnumBase, NonClass::Union, NonClass::ClassUnion,
            int, float,
            nullptr_t, void*,

            NonClass::Enum*, NonClass::ClassEnum*, NonClass::ClassEnumBase*, NonClass::Union*, NonClass::ClassUnion*,
            int*, float*, void**

    >(manager.getNonClassObjects(), logger);


    logger.endTest();
}



