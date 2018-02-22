//          Copyright Benjamin Southall 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CLASSES_H
#define CLASSES_H

#include <iostream>
#include <tuple>

inline int dRand(){
    int res;
    while ((res = std::rand()) == 0xdeadbeef)
        ;
    return res;
}


namespace NonClass{

    enum Enum{ Dummy };
    enum class ClassEnum{ Dummy };
    enum class ClassEnumBase : long{ Dummy };

    union Union{
        int i; float f;
    };

    union ClassUnion{
        int i; float f;
        ClassUnion(int i) : i(i){}
    };

    typedef std::tuple<
        Enum, ClassEnum, ClassEnumBase, Union,
        int, float, bool,
        std::nullptr_t, void*,
        decltype(dRand)
    > Types;

}

namespace Polymorphic{

    namespace Normal{

        struct Base{
            Base(int b = 1) : b(b), hash(dRand()){}
            virtual ~Base(){}
            int b, hash;
            /* NOT VIRTUAL */ void print(){ std::cout << "(B) b is " << b << std::endl; }
            virtual void name(){ std::cout << "Normal::Base[" << b << "]" << std::endl; }
        };
        struct Middle : public Base{
            Middle(int b = 2, int m = 20) : Base(b), m(m){}
            int m;
            /* NOT VIRTUAL */ void print(){ std::cout << "(M) b is " << b << " m is " << m << std::endl; }
            virtual void name(){ std::cout << "Normal::Middle[" << b << "," << m << "]" << std::endl; }
        };
        struct Derived : public Middle{
            Derived(int b = 3, int m = 30, int d = 300) : Middle(b,m), d(d){}
            int d;
            /* NOT VIRTUAL */ void print(){ std::cout << "(D) b is " << b << " m is " << m << " d is " << d << std::endl; }
            virtual void name(){ std::cout << "Normal::Derived[" <<  b << "," << m << "," << d << "]" << std::endl; }
        };

        typedef std::tuple<Base, Middle, Derived> Classes;
    }

    namespace Virtual{


        struct Base{
                Base(int b = 1) : b(b), hash(dRand()){}
                virtual ~Base(){};
                int b, hash;
                /* NOT VIRTUAL */ void print(){ std::cout << "(B) b is " << b << std::endl; }
                virtual void name(){ std::cout << "Normal::Base[" << b << "]" << std::endl; }
        };

        struct Middle : virtual public Base{
                Middle(int b = 2, int m = 20) : Base(b), m(m){}
                int m;
                /* NOT VIRTUAL */ void print(){ std::cout << "(M) b is " << b << " m is " << m << std::endl; }
                virtual void name(){ std::cout << "Normal::Middle[" << b << "," << m << "]" << std::endl; }
        };

        struct Derived : virtual  public Middle{
                Derived(int b = 3, int m = 30, int d = 300) : Base(b), Middle(b,m), d(d){}
                int d;
                /* NOT VIRTUAL */ void print(){ std::cout << "(D) b is " << b << " m is " << m << " d is " << d << std::endl; }
                virtual void name(){ std::cout << "Virtual::Derived[" <<  b << "," << m << "," << d << "]" << std::endl; }
        };

        struct FinDerived : public Derived{
                FinDerived(int b = 4, int m = 40, int d = 400, int f = 4000) : Base(b), Middle(0, m), Derived(0,0,d), f(f){}
                int f;
                /* NOT VIRTUAL */ void print(){ std::cout << "(F) b is " << b << " m is " << m << " d is " << d << " f is " << f << std::endl; }
                void name(){ std::cout << "FinDerived" << std::endl; }
        };

        typedef std::tuple<Base, Middle, Derived, FinDerived> Classes;



        namespace MI{

            struct Alpha{
                    Alpha(int a = 1) : a(a), hash(dRand()){}
                    virtual ~Alpha(){};
                    int a, hash;
    /* NOT VIRTUAL */ void print(){ std::cout << "(A) a is " << a << std::endl; }
                    virtual void name(){ std::cout << "Alpha[" << a << "]" << std::endl; }
            };

            struct Beta : virtual public Alpha{
                    Beta(int a = 2, int b = 20) : Alpha(a), b(b){}
                    int b;
                    void print(){ std::cout << "(B) a is " << a << " b is " << b << std::endl; }
                    virtual void name(){ std::cout << "Beta[" << a << "," << b << "]" << std::endl; }
            };

            struct Gamma : virtual public Alpha{
                    Gamma(int a = 3, int g = 300) : Alpha(a), g(g){}
                    int g;
                    void print(){ std::cout << "(G) a is " << a << " g is " << g << std::endl; }
                    virtual void name(){ std::cout << "Gamma[" << a  << "," << g << "]" << std::endl; }
            };

            struct Delta : public Beta, public Gamma{
                    Delta(int a = 4, int b = 40, int g = 400, int d = 4000) : Alpha(a), Beta(0, b), Gamma(0, g), d(d){}
                    int d;
                    void print(){ std::cout << "(D) a is " << a << " b is " << b << " g is " << g << " d is " << d << std::endl; }
                    virtual void name(){ std::cout << "Delta[" << a << "," << b << "," << g << "," << d << "]" << std::endl; }
            };

            typedef std::tuple<Alpha, Beta, Gamma, Delta> Classes;
        }
    }

    namespace MI{
        struct Circle{
                Circle(int c = 1) : c(c), hash(dRand()){}
                virtual ~Circle(){}
                int c, hash;
                /* NOT VIRTUAL */ void print(){ std::cout << "(C) c is " << c << std::endl; }
                virtual void name(){ std::cout << "Circle[" << c << "]" << std::endl; }
        };

        struct Square{
                Square(int s = 2) : s(s), hash(dRand()){}
                virtual ~Square(){}
                int s, hash;
                /* NOT VIRTUAL */ void print(){ std::cout << "(S) s is " << s << std::endl; }
                virtual void name(){ std::cout << "Square[" << s << "]" << std::endl; }
        };

        struct RoundedSquare : public Circle, public Square{
                RoundedSquare(int c = 3, int s = 30, int r = 300) : Circle(c), Square(s), r(r), hash(dRand()){}
                int r, hash;
                /* NOT VIRTUAL */ void print(){ std::cout << "(R) c is " << c << " s is " << s << " r is " << r << std::endl; }
                void name(){ std::cout << "RoundedSquare[" << c << "," << s << "," << r << "]" << std::endl; }
        };

        typedef std::tuple<Circle, Square, RoundedSquare> Classes;
    }

}


namespace NonPolymorphic{

    namespace Normal{

        struct Base{
            Base(int b = 1) : b(b), hash(dRand()){}
            int b, hash;
            void print(){ std::cout << "(B) b is " << b << std::endl; }
            void name(){ std::cout << "Normal::Base[" << b << "]" << std::endl; }
        };
        struct Middle : public Base{
            Middle(int b = 2, int m = 20) : Base(b), m(m){}
            int m;
            void print(){ std::cout << "(M) b is " << b << " m is " << m << std::endl; }
            void name(){ std::cout << "Normal::Middle[" << b << "," << m << "]" << std::endl; }
        };
        struct Derived : public Middle{
            Derived(int b = 3, int m = 30, int d = 300) : Middle(b,m), d(d){}
            int d;
            void print(){ std::cout << "(D) b is " << b << " m is " << m << " d is " << d << std::endl; }
            void name(){ std::cout << "Normal::Derived[" <<  b << "," << m << "," << d << "]" << std::endl; }
        };

        typedef std::tuple<Base, Middle, Derived> Classes;
    }

    namespace MI{
        struct Circle{
                Circle(int c = 1) : c(c), hash(dRand()){}
                int c, hash;
                void print(){ std::cout << "(C) c is " << c << std::endl; }
                void name(){ std::cout << "Circle[" << c << "]" << std::endl; }
        };

        struct Square{
                Square(int s = 2) : s(s), hash(dRand()){}
                int s, hash;
                void print(){ std::cout << "(S) s is " << s << std::endl; }
                void name(){ std::cout << "Square[" << s << "]" << std::endl; }
        };

        struct RoundedSquare : public Circle, public Square{
                RoundedSquare(int c = 3, int s = 30, int r = 300) : Circle(c), Square(s), r(r), hash(dRand()){}
                int r, hash;
                void print(){ std::cout << "(R) c is " << c << " s is " << s << " r is " << r << std::endl; }
                void name(){ std::cout << "RoundedSquare[" << c << "," << s << "," << r << "]" << std::endl; }
        };

        typedef std::tuple<Circle, Square, RoundedSquare> Classes;
    }

}


typedef decltype(std::tuple_cat(
    std::declval<Polymorphic::Normal::Classes>(), std::declval<Polymorphic::MI::Classes>(),
    std::declval<Polymorphic::Virtual::Classes>(), std::declval<Polymorphic::Virtual::MI::Classes>()
)) PolymorphicClasses;

typedef decltype(std::tuple_cat(
    std::declval<NonPolymorphic::Normal::Classes>(), std::declval<NonPolymorphic::MI::Classes>()
)) NonPolymorphicClasses;

typedef decltype(std::tuple_cat(
    std::declval<NonPolymorphicClasses>(), std::declval<PolymorphicClasses>()
)) AllClasses;

#endif // CLASSES_H
