//          Copyright Benjamin Southall 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef TESTSSHARED_H
#define TESTSSHARED_H

#include <typeinfo>
#include <iostream>
#include <functional>
#include <deque>

#include "GlobalFunctions.h"
#include "Object.h"


class Logger
{
        unsigned fVerbosity;
        bool fLogPasses, fLogBorings;
        unsigned fTestNumber;

        typedef std::deque<std::tuple<unsigned, unsigned, unsigned>> Counts;
        Counts fCountsList;
        unsigned fCurrentPasses, fCurrentBorings, fCurrentFailures;


        struct NullBuffer : public std::streambuf
        {
            int overflow(int c) { return c; }
        };

        NullBuffer fNullBuffer;
        std::ostream &fStream, fNullStream;
        std::ostringstream fInfoStream;


    public:

        Logger(std::ostream& where, unsigned verbosity, bool logPasses, bool logBorings);

        std::ostream& pass(int number = 1); //Negative means don't output, just update by -number
        std::ostream& bore(int number = 1);
        std::ostream& fail(int number = 1);
        std::ostream& info();

        //std::ostream& debug(unsigned level);

        bool dumpTest(unsigned testNumber);
        bool dumpTests();

        void startTest(std::string function, std::string header);
        void endTest();

};


#define SUCCEEED std::get<0>(args)++; std::get<2>(args)
#define BORING std::get<3>(args)
#define FAIL std::get<4>(args)



template<class T> struct TestCastsEqual
{
    template<class ObjectClass, class Vars>
    static void apply(const ObjectClass& anObject, Vars& logger)
    {

        const void *dynamic = anObject. template dynamicCastVoid<T>(),
            *nc2 = anObject. template newCast<T>(),
            *nc3 = anObject. template newCast3Arg<T>();

        if constexpr (ObjectClass::staticIsPolymorphic){
            if ((nc2 != dynamic) || (nc3 != dynamic))

                logger.fail()
                    << typeName(anObject.staticType()) << "[" << typeName(anObject.dynamicType()) << "] -> " << typeName<T>()
                    << "\t" << "nc2:" << nc2 << " - nc3: " << nc3 << " - dynamic: " << dynamic
                    << std::endl;

            else
                if (nc3)
                    logger.bore()
                        << typeName(anObject.staticType()) << "[" << typeName(anObject.dynamicType()) << "] -> " << typeName<T>()
                        << "\t" << "nc2:" << nc2 << " - nc3: " << nc3 << " - dynamic: " << dynamic
                        << std::endl;

                  else
                    logger.pass()
                        << typeName(anObject.staticType()) << "[" << typeName(anObject.dynamicType()) << "] -> " << typeName<T>()
                        << "\t" << "nc2:" << nc2 << " - nc3: " << nc3 << " - dynamic: " << dynamic
                        << std::endl;
        } else {

            if ((nc2 != nullptr) || (nc3 != dynamic))

                logger.fail()
                    << typeName(anObject.staticType()) << "[" << typeName(anObject.dynamicType()) << "] -> " << typeName<T>()
                    << "\t" << "nc2:" << nc2 << " - nc3: " << nc3 << " - dynamic: " << dynamic
                    << std::endl;

            else
                if (nc3)
                    logger.bore()
                        << typeName(anObject.staticType()) << "[" << typeName(anObject.dynamicType()) << "] -> " << typeName<T>()
                        << "\t" << "nc2:" << nc2 << " - nc3: " << nc3 << " - dynamic: " << dynamic
                        << std::endl;

                  else
                    logger.pass()
                        << typeName(anObject.staticType()) << "[" << typeName(anObject.dynamicType()) << "] -> " << typeName<T>()
                        << "\t" << "nc2:" << nc2 << " - nc3: " << nc3 << " - dynamic: " << dynamic
                        << std::endl;

        }

    }

};


template<class T> struct TestPolymorphicPropertiesPolymorphic
{
    template<class ObjectClass, class Vars> //Vars == tuple<ObjectClass, int, int> (successes, failures)
    static void apply(const ObjectClass& anObject, Vars& logger)
    {
        auto results = polymorphicProperties(anObject.pointer());

        if ( !results.first || !results.second )

            logger.fail()
                 << "object of STATIC type " << typeName(anObject.staticType()) << "IS POLYMORPHIC but CANNOT be accessed as polymorphic" << endl
                 << std::endl;


        else if ( ( *results.first != anObject.dynamicType()) || (results.second != anObject.fullPointer()) )

            logger.fail()
                 << "object of STATIC type " << typeName(anObject.staticType()) << "IS POLYMORPHIC but polymorphicProperties returned wrong details " << endl
                 << "fullPointer(): " << anObject.fullPointer() << " res: " << results.second
                 << " dynamicType() " << demangle(anObject.dynamicType().name()) << " res: " << demangle(results.first->name()) << endl
                 << std::endl;

        else
            logger.pass()
                    << " object of STATIC type " << typeName(anObject.staticType()) << " IS POLYMORPHIC and CAN be accessed as polymorphic"
                    << endl;


    }

};

template<class T> struct TestPolymorphicPropertiesNonPolymorphic
{
    template<class ObjectClass, class Vars> //Vars == tuple<ObjectClass, int, int> (successes, failures)
    static void apply(const ObjectClass& anObject, Vars& logger)
    {
        auto results = polymorphicProperties(anObject.pointer());

        logger.info() << typeName(anObject.staticType()) << "[" << typeName(anObject.dynamicType()) << "] -> " << typeName<T>() << "\t";

        if (results.first || results.second)

            logger.fail()
                << "object of static type " << typeName(anObject.staticType()) << " IS NOT POLYMORPHIC but CAN been accessed as polymorphic"
                << endl;

        else

            logger.pass()
                << " object " << typeName(anObject.staticType()) << " IS NOT POLYMORPHIC and CANNOT be accessed as polymorphic"
                << endl;

    }

};

template<class T> struct TestNewAnyPointerAndDynamic
{
    template<class ObjectClass, class Vars> //Vars == tuple<ObjectClass, int, int> (successes, failures)
    static void apply(const ObjectClass& anObject, Vars& logger)
    {
        DEBUG(3, "ITERATION: ", typeName<T>(), " on ", typeName<ObjectClass>(), "\n");


        Any::any anyPointer = anObject.anyPointer();

        logger.info() << " anyPointer: " << typeName(anyPointer.type()) << endl;

        const void *pointer = nullptr;
        auto *dynamic = anObject. template dynamicCast<T>();

        try {
            pointer = Any::new_any_cast<T*>(anyPointer);
        } catch(Any::bad_any_cast){}


        logger.info() << typeName(anObject.staticType()) << "[" << typeName(anObject.dynamicType()) << "] -> " << typeName<T>() << "\t";

        if ((pointer == dynamic) && (!pointer) ) // && (objectHashCode == 0xdeadbeef)){

            logger.bore()
                << "ptr: " << pointer << " - dynamic: " << dynamic
                << std::endl;

        else if (pointer && dynamic)

            logger.pass()
                << "ptr: " << pointer << " - dynamic: " << dynamic
                << std::endl;

        else

            logger.fail()
                << "ptr: " << pointer << " - dynamic: " << dynamic
                << std::endl;


        try{
            Any::new_any_cast<T>(anyPointer);
            logger.fail() << "|-> cast from pointer type to object type succeeded when it should have failed" << endl;
        } catch(const Any::bad_any_cast& ){
            logger.pass(-1);
        }

        try{
            Any::new_any_cast<T**>(anyPointer);
            logger.fail() << "|-> cast from pointer type to pointer-to-pointer type succeeded when it should have failed" << endl;
        } catch(Any::bad_any_cast){
            logger.pass(-1);
        }

    }

};


template<class T> struct TestNewAnyObjectAndDynamic
{
    template<class ObjectClass, class Vars> //Vars == tuple<ObjectClass, int, int> (successes, failures)
    static void apply(const ObjectClass& anObject, Vars& logger)
    {
        DEBUG(3, "ITERATION: ", typeName<T>(), " on ", typeName<ObjectClass>(), "\n");

        Any::any anyObject = anObject.anyObject();

        DEBUG(2, "anyObject: ", typeName(anyObject.type()), "\n")

        auto *dynamic = anObject. template dynamicCast<T>();
        int objectHashCode = 0xdeadbeef;

        try {
            objectHashCode = Any::new_any_cast<T>(anyObject).hash;
        } catch(Any::bad_any_cast){}

        logger.info() << typeName(anObject.staticType()) << "[" << typeName(anObject.dynamicType()) << "] -> " << typeName<T>() << "\n";

        if ( (!dynamic) && (objectHashCode == 0xdeadbeef) ) // && (objectHashCode == 0xdeadbeef)){

            logger.bore()
                << "object any cast successfully failed"
                << std::endl;

        else if ( (dynamic)  && (objectHashCode = dynamic->hash))

            logger.pass()
                << "hash: " << objectHashCode << " - dynamic: " << dynamic
                << std::endl;

        else

            logger.fail()
                << "hash: " << objectHashCode << " - dynamic: " << dynamic
                << std::endl;


        try{
            Any::new_any_cast<T*>(anyObject);
            logger.fail() << "|-> cast from object type to pointer type succeeded when it should have failed" << endl;
        } catch(const Any::bad_any_cast& ){
            logger.pass(-1);
        }

        try{
            Any::new_any_cast<T**>(anyObject);
            logger.fail() << "|-> cast from object type to pointer-to-pointer type succeeded when it should have failed" << endl;
        } catch(Any::bad_any_cast){
            logger.pass(-1);
        }

    }

};

template<class T> struct TestNewAnyAgainstCurrentAny
{
    template<class Vars> //Vars == tuple<ObjectClass, int, int> (successes, failures)
    static void apply(const Any::any &any, Vars& logger)
    {
        DEBUG(3, "ITERATION: ", typeName<T>(), " on ", typeName(any.type()), "\n");

        bool anySucceeded = true, newAnySucceeded = true;

        try {
            Any::any_cast<T>(any);
        } catch(Any::bad_any_cast){
            anySucceeded = false;
        }

        try {
            Any::new_any_cast<T>(any);
        } catch(Any::bad_any_cast){
            newAnySucceeded = false;
        }

        if ( anySucceeded == newAnySucceeded ) // && (objectHashCode == 0xdeadbeef)){

            logger.bore()
                << "any_cast and new_any_cast agree from " << typeName(any.type()) << " to " << typeName<T>()
                << std::endl;

        else

            logger.fail()
                << "any_cast and new_any_cast DON'T agree from " << typeName(any.type()) << " to " << typeName<T>()
                << "\t any succeeded?: " << boolalpha <<  anySucceeded << " - new_any succeeded?: " << newAnySucceeded
                << std::endl;

    }

};

void testObjectsAgainstTwoTypes(const std::list<ObjectIF*>& objects,
        const std::list<const std::type_info*>& types1,
        const std::list<const std::type_info*>& types2,
        std::function<void(const char *, const void*, const void*)> outputFunction,
        Logger &logger
);


#endif // TESTSSHARED_H
