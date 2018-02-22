#include "SharedLibrarySideTests.h"

#include "Classes.h"
#include "TupleUtils.h"
#include "GlobalFunctions.h"
#include "Object.h"
#include "ObjectManager.h"
#include "TestsShared.h"


static ObjectManager manager;

template<class T> struct ListCollect
{
    static std::list<T> apply(const T& item){
        return std::list<T> {item};
    }
    static std::list<T> apply(const T& item, std::list<T> list){
        list.push_back(item);
        return list;
    }
};

template<class T> struct TypeidClass
{
    static const type_info* apply(){
        return &typeid(T);
    }
};


const std::list<ObjectIF*>& getSharedLibraryPolymorphicObjects(){
    return manager.getPolymorphicObjectIFs();
}

const std::list<const std::type_info*>& getSharedLibraryPolymorphicTypes(){
    return manager.getPolymorphicTypes();
}


template <int V1, int V2> struct AssertEquality
{
    static const char not_equal_warning = V1 + V2 + 256;
};

template <int V> struct AssertEquality<V, V>
{
    static const bool not_equal_warning = 0;
};

#define ASSERT_EQUALITY(V1, V2) static_assert( \
    AssertEquality<static_cast<int>(V1), \
                   static_cast<int>(V2)>::not_equal_warning == 0, \
    #V1 " != " #V2 );

// ...


void testDynamicAgainstNewSharedLibrary(Logger &logger)
{
    // This test ensures that dynamic_cast and new_ cast always return the same value,
    // except where a dynamic_cast is undefined. This is enabled by using enable_if
    // inside can_dynamic_cast, so dynamic_cast_if_can returns null if the cast is not well defined.
    // This, however, is abstracted into the Object class' template functions.

        logger.startTest(__FUNCTION__,
             "       This function tests that dynamic_cast and new_cast always return the same value, where such a test can be performed.\n"
             "       Casting to a class completely outside of an object's hierarchy is safe with new_cast but is undefined with dynamic_cast,\n"
             "       therefore we use a template that returns null before an illegal cast is made, and only perform valid up-, down- and cross-casts.\n"
             "       As static type information is needed, the type information cannot be passed across the library boundary.\n"
             "       These casts are performed in the shared library on objects in the shared library."
         );


   bindTuple<TestCastsEqual,
            Polymorphic::Normal::Base, Polymorphic::Normal::Middle, Polymorphic::Normal::Derived,
            Polymorphic::MI::Circle, Polymorphic::MI::Square, Polymorphic::MI::RoundedSquare,
            Polymorphic::Virtual::Base, Polymorphic::Virtual::Middle, Polymorphic::Virtual::Derived, Polymorphic::Virtual::FinDerived,
            Polymorphic::Virtual::MI::Alpha, Polymorphic::Virtual::MI::Beta, Polymorphic::Virtual::MI::Gamma, Polymorphic::Virtual::MI::Delta
    >(manager.getPolymorphicObjects(), logger);

}


bool testLibraryObjectsInLibrary(const std::list<ObjectIF *> &incomingObjects, const std::list<const type_info *> &types){


    bool ret = true;
    auto internalTypes = manager.getPolymorphicTypes();

    for (auto object : incomingObjects){
        for(
            auto internalType = internalTypes.cbegin(), externalType = types.cbegin();
            (internalType != internalTypes.cend()) && (externalType != types.cend());
            internalType++, externalType++
        ){
            cout << typeName(object->staticType()) << "[" << typeName(object->dynamicType()) << "] -> (" << typeName(*internalType) << ") - (" << typeName(*externalType) << ")\t";

            const void *internal = object->newCast(**internalType), *external = object->newCast(**externalType);

            std::string prefix;
            if (internal != external){
                prefix = std::string(red) + "Fail- Library Type: ";
                ret = false;
            }
            else {
                if (internal)
                    prefix = std::string(green) + "Pass- Library Type";
                else
                    prefix = std::string(yellow) + "Pass- Library Type";
            }

            cout << prefix << internal << " - Binary Type: " << external << normal << endl;
        }
    }

    return ret;

}

