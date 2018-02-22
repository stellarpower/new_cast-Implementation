//          Copyright Benjamin Southall 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#include "ObjectManager.h"
#include "GlobalFunctions.h"
#include <algorithm>


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


template<class tuple>std::list<const std::type_info*> getTypeinfos(){
    return tuple_for_each<tuple, TypeidClass, ListCollect>();
}

ObjectManager::ObjectManager()
    : fPolymorphicObjects(constructPolymorphicObjects()),
      fPolymorphicTypes(getTypeinfos<PolymorphicClasses>()),
      fNonPolymorphicObjects(constructNonPolymorphicObjects()),
      fNonPolymorphicTypes(getTypeinfos<NonPolymorphicClasses>()),
      fAllObjectIFs(fPolymorphicObjectIFs),
      fAllTypes(fPolymorphicTypes),
      fNonClassObjects(constructNonClassObjects()),
      fNonClassTypes(getTypeinfos<NonClass::Types>())
{
    fAllObjectIFs.insert(fAllObjectIFs.end(), fNonPolymorphicObjectIFs.begin(), fNonPolymorphicObjectIFs.end());
    fAllTypes.insert(fAllTypes.end(), fNonPolymorphicTypes.begin(), fNonPolymorphicTypes.end());

    tuple_for_each(fPolymorphicObjects, [this](auto& t){ fPolymorphicObjectIFs.push_back(&t); } );
    tuple_for_each(fNonPolymorphicObjects, [this](auto& t){ fNonPolymorphicObjectIFs.push_back(&t); } );
}



//////////////////


const std::list<ObjectIF*>& ObjectManager::getPolymorphicObjectIFs() const
{
    return fPolymorphicObjectIFs;
}

ObjectManager::PolymorphicObjectsTuple ObjectManager::getPolymorphicObjects() const
{
    return fPolymorphicObjects;
}

const std::list<const std::type_info*>& ObjectManager::getPolymorphicTypes() const
{
    return fPolymorphicTypes;
}

//////////////////////

const std::list<ObjectIF*>& ObjectManager::getNonPolymorphicObjectIFs() const
{
    return fNonPolymorphicObjectIFs;
}

ObjectManager::NonPolymorphicObjectsTuple ObjectManager::getNonPolymorphicObjects() const
{
    return fNonPolymorphicObjects;
}

const std::list<const std::type_info*>& ObjectManager::getNonPolymorphicTypes() const
{
    return fNonPolymorphicTypes;
}

//////////////////////

const std::list<ObjectIF*>& ObjectManager::getAllObjectIFs() const
{
    return fAllObjectIFs;
}

ObjectManager::AllObjectsTuple ObjectManager::getAllObjects() const
{
    return std::tuple_cat(fPolymorphicObjects, fNonPolymorphicObjects);
}

const std::list<const std::type_info*>& ObjectManager::getAllTypes() const
{
    return fAllTypes;
}

///////////////////////

ObjectManager::NonClassObjectsTuple ObjectManager::getNonClassObjects() const
{
    return fNonClassObjects;
}

const std::list<ObjectIF*>& ObjectManager::getNonClassObjectIFs() const
{
    return fNonClassObjectIFs;
}

const std::list<const std::type_info*>& ObjectManager::getNonClassTypes() const
{
    return fNonClassTypes;
}


