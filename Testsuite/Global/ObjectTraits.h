//          Copyright Benjamin Southall 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef OBJECTTRAITS_H
#define OBJECTTRAITS_H

#include <type_traits>
#include <typeinfo>
#include <cstddef>
#include <memory>

#include "Classes.h"

namespace Traits{

    template<class Source, class Dest> constexpr bool can_cross_cast = false;

    template<> constexpr bool can_cross_cast<Polymorphic::Virtual::MI::Beta, Polymorphic::Virtual::MI::Gamma> = true;
    template<> constexpr bool can_cross_cast<Polymorphic::Virtual::MI::Gamma, Polymorphic::Virtual::MI::Beta> = true;

    template<> constexpr bool can_cross_cast<Polymorphic::MI::Square, Polymorphic::MI::Circle> = true;
    template<> constexpr bool can_cross_cast<Polymorphic::MI::Circle, Polymorphic::MI::Square> = true;

    template<> constexpr bool can_cross_cast<NonPolymorphic::MI::Square, NonPolymorphic::MI::Circle> = true;
    template<> constexpr bool can_cross_cast<NonPolymorphic::MI::Circle, NonPolymorphic::MI::Square> = true;

    template<class Source, class Dest> constexpr bool can_dynamic_cast =
           std::is_base_of<Source, Dest>::value
        || std::is_base_of<Dest, Source>::value
        || can_cross_cast<Source, Dest>;


    template<class Source, class Dest> constexpr bool can_static_cast =
           std::is_base_of<Dest, Source>::value
        || std::is_same<Source, Dest>::value;


    template<class T> using removePointerReference
        = typename std::remove_pointer<typename std::remove_reference<T>::type>::type;



    template<class T> constexpr bool isClassObject = std::is_class<T>::value;

    template<class T> constexpr bool isClassPointer //Class
        = std::is_pointer<T>::value
        & std::is_class<typename std::remove_pointer<T>::type>::value;

    template<class T> constexpr bool isClassLValueReference //Class&
        = std::is_lvalue_reference<T>::value
        & std::is_class<typename std::remove_reference<T>::type>::value;

    template<class T> constexpr bool isClassRValueReference //Class&&
        = std::is_rvalue_reference<T>::value
        & std::is_class<typename std::remove_reference<T>::type>::value;

    template<class T> constexpr bool isClassPointerLValueReference //Class*&
        = std::is_lvalue_reference<T>::value
        & std::is_pointer<typename std::remove_reference<T>::type>::value
        & std::is_class<removePointerReference<T>>::value;

    template<class T> constexpr bool isClassPointerRValueReference //Class*&&
        = std::is_rvalue_reference<T>::value
        & std::is_pointer<typename std::remove_reference<T>::type>::value
        & std::is_class<removePointerReference<T>>::value;

    template<class T> constexpr bool isPolymorphic
        = std::is_polymorphic_v<removePointerReference<T>>;

} //namespace Traits




//ObjectTraits/////////////////////////////////////////////////////////////////////////////////////////

template<
    class T,
    bool co = Traits::isClassObject<T>,
    bool cp = Traits::isClassPointer<T>,
    bool iclr = Traits::isClassLValueReference<T>,
    bool icrr = Traits::isClassRValueReference<T>,
    bool icplr = Traits::isClassPointerLValueReference<T>,
    bool icprr = Traits::isClassPointerRValueReference<T>,
    bool ip = Traits::isPolymorphic<T>
>
struct ObjectTraits{

    static const bool
        isClassObject = co,
        isClassPointer = cp,
        isClassLValueReference = iclr,
        isClassRValueReference = icrr,
        isClassPointerLValueReference = icplr,
        isClassPointerRvalueReference = icprr,
        isPolymorphic = ip
    ;

};

//_LiteralType////////////////////////////////////////////////////////////////////////////////////////


template<class T> struct _LiteralType{
    typedef T GivenType;

    //AddPointerFlexible<int&> is int*, AddPointerStrict<int&> is undefined.
    typedef typename std::add_pointer<T>::type AddPointerFlexible;
    typedef std::conditional<!std::is_reference<T>::value, T*, void> AddPointerStrict;


    //RemovePointerFlexible<int> for example is int, RemovePointerStrict<int> is undefined.
    typedef typename std::remove_pointer<T>::type RemovePointerFlexible;
    typedef typename std::conditional<std::is_pointer<T>::value, typename std::remove_pointer<T>::type, void>::type RemovePointerStrict;


    //AddReferenceFlexible<int&> is int&, AddReferenceStrict<int&> is undefined.
    typedef typename std::add_rvalue_reference<T>::type AddReferenceFlexible;
    typedef std::conditional<!std::is_reference<T>::value, T&, void> AddReferenceStrict;

    //AddRemoveReferenceFlexible<int> is int, RemoveReferenceStrict<int> is undefined.
    typedef typename std::remove_reference<T>::type RemoveReferenceFlexible;
    typedef typename std::conditional<std::is_reference<T>::value, typename std::remove_reference<T>::type, void>::type RemoveReferenceStrict;

};



//ObjectTypes////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ObjectTypes encapsulates only the work from Object that is dependent on the traits, to save repetition and specialisation of Object.
// This way, Object doesn't need specialisation and can inherit from ObjectTypes, and use the types and functions defined there.


template<class T, template<class, bool...> class TraitsTemplate, class TraitsSpecialisation = TraitsTemplate<T>> class ObjectTypes {};


/////////////////////////////////////////////////////////////////////////////////////////////////// Pointers

template<class T, template<class, bool...> class TraitsTemplate, bool ip>
//														class object  class ptr  class lref  class rref  class ptr lref  class ptr rref  poly 
struct ObjectTypes<T, TraitsTemplate, TraitsTemplate<T, false,        true,      false,      false,      false,          false,          ip>/*, false*/>{

    struct StaticType{
        typedef typename std::remove_pointer<T>::type ObjectType;
        typedef ObjectType* PointerType;
        typedef ObjectType& ReferenceType;
    };

    typedef typename StaticType::PointerType StorageType; //This defines what Object should hold - for pointers, a pointer, for references, a reference.
    typedef typename std::shared_ptr<typename StaticType::ObjectType> SmartStorageType;

    template<class U> using ObjectToStorage = U*; //Used to convert a class type in dynamic_cast to a pointer or reference.

    constexpr typename StaticType::PointerType storageToPointer(SmartStorageType a) const{
        return a.get();
    }

    struct LiteralType : public _LiteralType<T>{};

    static const std::type_info& staticType()           { return typeid(StaticType::ObjectType);    }
    static const std::type_info& staticPointerType()    { return typeid(StaticType::PointerType);   }
    static const std::type_info& staticOriginalType()   { return typeid(T);                         }

};



/////////////////////////////////////////////////////////////////////////////////////////////////// Lvalue References

template<class T, template<class, bool...> class TraitsTemplate, bool ip>
//														class object  class ptr  class lref  class rref  class ptr lref  class ptr rref  poly 
struct ObjectTypes<T, TraitsTemplate, TraitsTemplate<T, false,        false,     true,       false,      false,          false,          ip>/*, false*/>{

    struct StaticType{
        typedef typename std::remove_reference<T>::type ObjectType;
        typedef ObjectType* PointerType;
        typedef ObjectType& ReferenceType; //Same as T
    };

    typedef typename StaticType::ReferenceType StorageType; //This defines what Object should hold - for pointers, a pointer, for references, a reference.
    typedef StorageType SmartStorageType;

    template<class U> using ObjectToStorage = U&;

    constexpr typename StaticType::PointerType storageToPointer(SmartStorageType a) const{
        return &a;
    }

    struct LiteralType : public _LiteralType<T>{};

    static const std::type_info& staticType()           { return typeid(StaticType::ObjectType);    }
    static const std::type_info& staticPointerType()    { return typeid(StaticType::PointerType);   }
    static const std::type_info& staticOriginalType()   { return typeid(T);                         }

};



/////////////////////////////////////////////////////////////////////////////////////////////////// LValue Pointer References

template<class T, template<class, bool...> class TraitsTemplate, bool ip>
//														class object  class ptr  class lref  class rref  class ptr lref  class ptr rref  poly 
struct ObjectTypes<T, TraitsTemplate, TraitsTemplate<T, false,        false,     false,      true,       false,          false,          ip>/*, false*/>{

    struct StaticType{
        typedef typename std::remove_pointer<typename std::remove_reference<T>::type>::type ObjectType;
        typedef ObjectType* PointerType;
        typedef ObjectType& ReferenceType;
    };

    typedef typename StaticType::PointerType StorageType;
    typedef typename std::shared_ptr<typename StaticType::ObjectType> SmartStorageType;

    template<class U> using ObjectToStorage = U*;

    constexpr typename StaticType::PointerType storageToPointer(StorageType a) const{
        return a.get();
    }

    struct LiteralType : public _LiteralType<T>{};

    static const std::type_info& staticType()           { return typeid(StaticType::ObjectType);    }
    static const std::type_info& staticPointerType()    { return typeid(StaticType::PointerType);   }
    static const std::type_info& staticOriginalType()   { return typeid(T);                         }

};


//Fixme! Add rvalue references and rvalue pointer references.


template<class Source, class Dest>
Dest dynamic_cast_if_can(Source pointerOrReference,
                         typename std::enable_if< // Dest and Source should be pointer or reference.
                                Traits::template can_dynamic_cast< Traits::removePointerReference<Source>, Traits::removePointerReference<Dest> >,
                                std::nullptr_t
                         >::type  = nullptr)
{
    return dynamic_cast<Dest>(pointerOrReference);
}



template<class Source, class Dest>
Dest dynamic_cast_if_can(Source,
                         typename std::enable_if< // Dest and Source should be pointer or reference.
                                ! Traits::template can_dynamic_cast< Traits::removePointerReference<Source>, Traits::removePointerReference<Dest> >,
                                std::nullptr_t
                         >::type  = nullptr)
{
    return nullptr;
}

template<class Source, class Dest>
Dest static_cast_if_can(Source pointerOrReference,
                         typename std::enable_if< // Dest and Source should be pointer or reference.
                                Traits::template can_static_cast< Traits::removePointerReference<Source>, Traits::removePointerReference<Dest> >,
                                std::nullptr_t
                         >::type  = nullptr)
{
    return static_cast<Dest>(pointerOrReference);
}



template<class Source, class Dest>
Dest static_cast_if_can(Source,
                         typename std::enable_if< // Dest and Source should be pointer or reference.
                                ! Traits::template can_static_cast< Traits::removePointerReference<Source>, Traits::removePointerReference<Dest> >,
                                std::nullptr_t
                         >::type  = nullptr)
{
    return nullptr;
}



#endif // OBJECTTRAITS_H


