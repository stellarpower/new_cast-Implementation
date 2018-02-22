#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <list>
#include "Classes.h"
#include "Object.h"
#include "TupleUtils.h"


template<class T, class... Args> std::shared_ptr<T> New(Args... args)
{
    return std::shared_ptr<T>(new T(args...));
}




inline auto constructPolymorphicObjects(){
    auto base1 = New<Polymorphic::Normal::Base>(1);
    auto middle1 = New<Polymorphic::Normal::Middle>(2, 20);
    auto derived1 = New<Polymorphic::Normal::Derived>(3, 30, 300);

    std::shared_ptr<Polymorphic::Normal::Base> baseToMiddle1 = middle1,
        baseToDerived1 = derived1;
    std::shared_ptr<Polymorphic::Normal::Middle> middleToDerived1 = derived1;

    auto circle1 = New<Polymorphic::MI::Circle>(1);
    auto square1 = New<Polymorphic::MI::Square>(2);
    auto roundedSquare1 = New<Polymorphic::MI::RoundedSquare>(3, 30, 300);

    std::shared_ptr<Polymorphic::MI::Circle> circleToRoundedSquare1 = roundedSquare1;
    std::shared_ptr<Polymorphic::MI::Square> squareToRoundedSquare1 = roundedSquare1;
	
    auto alpha1 = New<Polymorphic::Virtual::MI::Alpha>(1);
    auto beta1 = New<Polymorphic::Virtual::MI::Beta>(2, 20);
    auto gamma1 = New<Polymorphic::Virtual::MI::Gamma>(3, 30);
    auto delta1 = New<Polymorphic::Virtual::MI::Delta>(4,40,400,4000);

    std::shared_ptr<Polymorphic::Virtual::MI::Alpha> alphaToBeta1 = beta1,
        alphaToGamma1 = gamma1,
        alphaToDelta1 = delta1;

    std::shared_ptr<Polymorphic::Virtual::MI::Beta> betaToDelta1 = delta1;
    std::shared_ptr<Polymorphic::Virtual::MI::Gamma> gammaToDelta1 = delta1;
	
    auto vFinDerived1 = New<Polymorphic::Virtual::FinDerived>(4, 40, 400, 4000);

    std::shared_ptr<Polymorphic::Virtual::Derived> vDerived1 = New<Polymorphic::Virtual::Derived>(3, 30, 300),
        vDerivedToVFinDerived1 = vFinDerived1;

    std::shared_ptr<Polymorphic::Virtual::Middle> vMiddle1 = New<Polymorphic::Virtual::Middle>(2, 20),
        vMiddleToVFinDerived1 = vFinDerived1,
        vMiddleToVDerived1 = vDerived1;

    std::shared_ptr<Polymorphic::Virtual::Base> vBase1 = New<Polymorphic::Virtual::Base>(1),
        vBaseToVFinDerived1 = vFinDerived1,
        vBaseToVDerived1 = vDerived1,
        vBaseToVMiddle1 = vMiddle1;

	

		
    return make_tuple_apply<MakeObject>(
        base1, middle1, derived1,
        baseToMiddle1, baseToDerived1, middleToDerived1,

        circle1, square1, roundedSquare1,
        circleToRoundedSquare1, squareToRoundedSquare1,
		
        alpha1, beta1, gamma1, delta1,
        alphaToBeta1, alphaToGamma1, alphaToDelta1, betaToDelta1, gammaToDelta1,
		
        vBase1, vMiddle1, vDerived1, vFinDerived1,
        vBaseToVMiddle1, vBaseToVDerived1, vBaseToVFinDerived1,
        vMiddleToVDerived1, vMiddleToVFinDerived1, vDerivedToVFinDerived1
		
    );
}

inline auto constructNonPolymorphicObjects(){
    auto base1 = New<NonPolymorphic::Normal::Base>(1);
    auto middle1 = New<NonPolymorphic::Normal::Middle>(2, 20);
    auto derived1 = New<NonPolymorphic::Normal::Derived>(3, 30, 300);

    std::shared_ptr<NonPolymorphic::Normal::Base> baseToMiddle1 = middle1,
        baseToDerived1 = derived1;
    std::shared_ptr<NonPolymorphic::Normal::Middle> middleToDerived1 = derived1;

    auto circle1 = New<NonPolymorphic::MI::Circle>(1);
    auto square1 = New<NonPolymorphic::MI::Square>(2);
    auto roundedSquare1 = New<NonPolymorphic::MI::RoundedSquare>(3, 30, 300);

    std::shared_ptr<NonPolymorphic::MI::Circle> circleToRoundedSquare1 = roundedSquare1;
    std::shared_ptr<NonPolymorphic::MI::Square> squareToRoundedSquare1 = roundedSquare1;
	

    return make_tuple_apply<MakeObject>(
        base1, middle1, derived1,
        baseToMiddle1, baseToDerived1, middleToDerived1,

        circle1, square1, roundedSquare1,
        circleToRoundedSquare1, squareToRoundedSquare1
    
	);
}


inline auto constructNonClassObjects(){

    using namespace NonClass;

    Enum e(Dummy);
    ClassEnum ce(ClassEnum::Dummy);
    ClassEnumBase ceb(ClassEnumBase::Dummy);
    Union u;
    ClassUnion cu(1);

    int i(1);
    float f(3.14f);

    void *v = nullptr;


    auto pe = New<Enum>(Dummy);
    auto pce = New<ClassEnum>(ClassEnum::Dummy);
    auto pceb = New<ClassEnumBase>(ClassEnumBase::Dummy);
    auto pu = New<Union>();
    auto pcu = New<ClassUnion>(1);

    auto pi = New<int>(1);
    auto pf = New<float>(3.14);

    auto pn = New<nullptr_t>(nullptr);
    auto pv = New<void*>(nullptr);

    return make_tuple(
        e, ce, ceb, u, cu,
        i, f,
        nullptr,

        pe, pce, pceb, pu, pcu,
        pi, pf,
        pn, v

    );
}

class ObjectManager
{
    public:


        ObjectManager();
        virtual ~ObjectManager(){}

        typedef decltype(constructPolymorphicObjects()) PolymorphicObjectsTuple;
        typedef decltype(constructNonPolymorphicObjects()) NonPolymorphicObjectsTuple;
        typedef decltype(std::tuple_cat(
            std::declval<PolymorphicObjectsTuple>(), std::declval<NonPolymorphicObjectsTuple>()
        )) AllObjectsTuple;

        typedef decltype(constructNonClassObjects()) NonClassObjectsTuple;

        const std::list<ObjectIF*>& getPolymorphicObjectIFs() const;
        PolymorphicObjectsTuple getPolymorphicObjects() const;
        const std::list<const std::type_info*>& getPolymorphicTypes() const;

        const std::list<ObjectIF*>& getNonPolymorphicObjectIFs() const;
        NonPolymorphicObjectsTuple getNonPolymorphicObjects() const;
        const std::list<const std::type_info*>& getNonPolymorphicTypes() const;

        const std::list<ObjectIF*>& getNonClassObjectIFs() const;
        NonClassObjectsTuple getNonClassObjects() const;
        const std::list<const std::type_info*>& getNonClassTypes() const;


        const std::list<ObjectIF*>& getAllObjectIFs() const;
        AllObjectsTuple getAllObjects() const;
        const std::list<const std::type_info*>& getAllTypes() const;

    private:
        PolymorphicObjectsTuple fPolymorphicObjects;
        std::list<ObjectIF*> fPolymorphicObjectIFs;
        std::list<const std::type_info*> fPolymorphicTypes;

        NonPolymorphicObjectsTuple fNonPolymorphicObjects;
        std::list<ObjectIF*> fNonPolymorphicObjectIFs;
        std::list<const std::type_info*> fNonPolymorphicTypes;

        NonClassObjectsTuple fNonClassObjects;
        std::list<ObjectIF*> fNonClassObjectIFs;
        std::list<const std::type_info*> fNonClassTypes;

        std::list<ObjectIF*> fAllObjectIFs;
        std::list<const std::type_info*> fAllTypes;


};


#endif // OBJECTMANAGER_H
