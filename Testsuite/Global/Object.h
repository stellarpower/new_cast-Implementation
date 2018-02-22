#ifndef OBJECT_H
#define OBJECT_H

#include <typeinfo>
#include <tuple>
#include <utility>
#include <type_traits>
#include <list>

#include "GlobalFunctions.h"
#include "ObjectTraits.h"
#include "Any.h"


using namespace std;

template<class T> constexpr auto& dynamic_type(const T& object, typename std::enable_if<!std::is_pointer<T>::value, nullptr_t>::type = nullptr){
    return typeid(object);
}


template<class T> constexpr auto& dynamic_type(const T pointer, typename std::enable_if<std::is_pointer<T>::value, nullptr_t>::type = nullptr){
    return typeid(*pointer);
}


template<class T> constexpr auto& dynamic_type(const std::shared_ptr<T> pointer){ //, typename std::enable_if<std::is_pointer<typename DeSmart<T>::type>::value, nullptr_t>::type = nullptr){
    return typeid(*pointer.get());
}



//ObjectIF///////////////////////////////////////////////////////////////////////////////////////////////////////////

class ObjectIF{
	public:
        virtual ~ObjectIF(){}

        virtual const void *newCast(const type_info& destinationType) const;
        virtual const void *newCast3Arg(const type_info& destinationType) const;

        template<class T> inline const void *newCast() const;
        template<class T> inline const void *newCast3Arg() const;

        virtual const type_info& staticType() const = 0;
        virtual const type_info& dynamicType() const = 0;
        virtual const type_info& staticOriginalType() const = 0;

        virtual bool isPolymorphic() const = 0;

        virtual void deleteObject() = 0;
        virtual const void *pointer() const = 0;
        virtual const void *fullPointer() const = 0;

        virtual Any::any anyPointer() const = 0;
        virtual Any::any anyObject() const = 0;
};

template<class T> inline const void* ObjectIF::newCast() const{
    return newCast(typeid(T));
}

template<class T> inline const void* ObjectIF::newCast3Arg() const{
    return newCast3Arg(typeid(T));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


template<class T, template<class, bool... > class TraitsClass> void staticTraitsCheck()
{
    static_assert(TraitsClass<T>::isClassObject, "not class");
    static_assert(TraitsClass<T>::isClassPointer, "not *");
    static_assert(TraitsClass<T>::isClassLValueReference, "not l&");
    static_assert(TraitsClass<T>::isClassRValueReference, "not r&");
    static_assert(TraitsClass<T>::isClassPointerLValueReference, "not l*&");
    static_assert(TraitsClass<T>::isClassPointerRvalueReference, "not r*&");

}



//Object/////////////////////////////////////////////////////////////////////////////////////////////////////////////


template<class T> struct DeSmart
{
    typedef T type;
    inline static T* ptr(const T* aPtr)
    {
        return aPtr;
    }
};

template<class T> struct DeSmart<std::shared_ptr<T>>
{
    typedef T* type;
    inline static T* ptr(const std::shared_ptr<T>& smartPtr)
    {
        return smartPtr.get();
    }
};


template<class T> using DS = typename DeSmart<T>::type;
template<class T> using OT = ObjectTypes<typename DeSmart<T>::type, ObjectTraits>;


typedef DS<int*> pink;
typedef ObjectTraits<int*, false, false, false, false, false, false, false > maroon;
typedef OT<int*> Aqua;



template<
        class T,
        template<class, bool...> class TraitsTemplate = ObjectTraits,
        class TraitsClass = TraitsTemplate< typename DeSmart<T>::type >
>
struct Object : public ObjectIF, public ObjectTypes< typename DeSmart<T>::type, TraitsTemplate>
{

    typedef typename DeSmart<T>::type Type;

    typedef typename ObjectTypes<Type, TraitsTemplate>::StaticType StaticType; //Depend on templates, so redefine here.
    typedef typename ObjectTypes<Type, TraitsTemplate>::LiteralType LiteralType;
    typedef typename ObjectTypes<Type, TraitsTemplate>::StorageType StorageType;
    typedef typename ObjectTypes<Type, TraitsTemplate>::SmartStorageType SmartStorageType;

    Object(const SmartStorageType anObject);

    virtual const void *pointer() const;
    virtual const void *fullPointer() const;

    virtual void deleteObject();

    virtual const type_info& staticType() const;
    virtual const type_info& dynamicType() const;
    virtual const type_info& staticOriginalType() const;

    static constexpr bool staticIsPolymorphic = TraitsClass::isPolymorphic;
    virtual bool isPolymorphic() const;

    template<class DestinationObjectType> const void *dynamicCastVoid() const;
    template<class DestinationObjectType> typename ObjectTypes<Type, TraitsTemplate>::template ObjectToStorage<DestinationObjectType> dynamicCast() const;
    template<class DestinationObjectType> typename StaticType::PointerType dynamicCastPointer() const;

    virtual Any::any anyPointer() const;
    virtual Any::any anyObject() const;

private:
    const SmartStorageType fObject;
    Any::any fAnyPointer, fAnyObject;

};




template<class T, template<class, bool...> class TraitsTemplate, class TraitsClass>
Object<T, TraitsTemplate, TraitsClass>::Object(const typename Object::SmartStorageType anObject)

    : fObject(anObject),
      fAnyPointer(DeSmart<SmartStorageType>::ptr(fObject)),
      fAnyObject(*DeSmart<SmartStorageType>::ptr(fObject))
    {}



template<class T, template<class, bool...> class TraitsTemplate, class TraitsClass>
const type_info& Object<T, TraitsTemplate, TraitsClass>::staticType() const
{
    return typeid(typename StaticType::ObjectType);
}

template<class T, template<class, bool...> class TraitsTemplate, class TraitsClass>
const type_info& Object<T, TraitsTemplate, TraitsClass>::dynamicType() const
{
    return dynamic_type(fObject);
}


template<class T, template<class, bool...> class TraitsTemplate, class TraitsClass>
const type_info& Object<T, TraitsTemplate, TraitsClass>::staticOriginalType() const
{
    return typeid(typename LiteralType::GivenType);
}


template<class T, template<class, bool...> class TraitsTemplate, class TraitsClass>
bool Object<T, TraitsTemplate, TraitsClass>::isPolymorphic() const
{
    return staticIsPolymorphic;
}

template<class T, template<class, bool...> class TraitsTemplate, class TraitsClass>
const void* Object<T, TraitsTemplate, TraitsClass>::pointer() const
{
    return this->storageToPointer(fObject);
}

template<class T, template<class, bool...> class TraitsTemplate, class TraitsClass>
const void* Object<T, TraitsTemplate, TraitsClass>::fullPointer() const
{
    if constexpr (TraitsClass::isPolymorphic)
        return dynamic_cast<const void*>(this->storageToPointer(fObject));
    else
        return this->storageToPointer(fObject);
}



template<class T, template<class, bool...> class TraitsTemplate, class TraitsClass>
void Object<T, TraitsTemplate, TraitsClass>::deleteObject()
{
    delete (this->storageToPointer(fObject));
}



template<class T, template<class, bool...> class TraitsTemplate, class TraitsClass>
template<class DestinationObjectType>
const void *Object<T, TraitsTemplate, TraitsClass>::dynamicCastVoid() const
{
    if constexpr (TraitsClass::isPolymorphic)
        return dynamic_cast_if_can<typename StaticType::PointerType, DestinationObjectType*>(this->storageToPointer(fObject));
    else
        return static_cast_if_can<typename StaticType::PointerType, DestinationObjectType*>(this->storageToPointer(fObject));
}


template<class T, template<class, bool...> class TraitsTemplate, class TraitsClass>
template<class DestinationObjectType>

typename ObjectTypes<typename Object<T, TraitsTemplate, TraitsClass>::Type, TraitsTemplate>::template ObjectToStorage<DestinationObjectType>
    Object<T, TraitsTemplate, TraitsClass>
    ::dynamicCast() const
{
    typedef typename ObjectTypes<Type, TraitsTemplate>::template ObjectToStorage<DestinationObjectType> Destination;

    if constexpr (TraitsClass::isPolymorphic)
        return dynamic_cast_if_can<StorageType, Destination>(DeSmart<SmartStorageType>::ptr(fObject));
    else
        return static_cast_if_can<StorageType, Destination>(DeSmart<SmartStorageType>::ptr(fObject));

}


template<class T, template<class, bool...> class TraitsTemplate, class TraitsClass>
template<class DestinationObjectType>
typename Object<T, TraitsTemplate, TraitsClass>::StaticType::PointerType Object<T, TraitsTemplate, TraitsClass>::dynamicCastPointer() const
{
    if constexpr (TraitsClass::isPolymorphic)
        return dynamic_cast_if_can<typename StaticType::PointerType, DestinationObjectType*>(this->storageToPointer(DeSmart<SmartStorageType>::ptr(fObject)));
    else
        return static_cast_if_can<typename StaticType::PointerType, DestinationObjectType*>(this->storageToPointer(DeSmart<SmartStorageType>::ptr(fObject)));

}

template<class T, template<class, bool...> class TraitsTemplate, class TraitsClass>
Any::any Object<T, TraitsTemplate, TraitsClass>::anyPointer() const
{
    return fAnyPointer;
}

template<class T, template<class, bool...> class TraitsTemplate, class TraitsClass>
Any::any Object<T, TraitsTemplate, TraitsClass>::anyObject() const
{
    return fAnyObject;
}


template<class T> struct MakeObject
{
    static constexpr auto apply(T arg)
    {
        return Object<T>(arg);
    }
};

template<class T> inline auto makeObject(T arg){
    return Object<T>(arg);
}

template<class... Objects>
std::list<ObjectIF*> makeObjects(Objects... objects)
{
    return (std::list<ObjectIF*>() << (... << objects));
}




#endif //OBJECT_H
