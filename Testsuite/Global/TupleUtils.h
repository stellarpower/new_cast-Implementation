//          Copyright Benjamin Southall 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef TUPLEUTILS_H
#define TUPLEUTILS_H

#include <tuple>
#include "GlobalFunctions.h"


template< template<class...> class Template, class T, class... Rest>
struct make_tuple_apply_class
{
    inline static auto make_tuple_apply(const T& arg1, Rest... arg_rest)
    {
        return tuple_cat(make_tuple(Template<T>::apply(arg1)), make_tuple_apply_class<Template, Rest...>::make_tuple_apply(arg_rest...));
    }
};

template< template<class...> class Template, class T>
struct make_tuple_apply_class<Template, T>
{
    static auto make_tuple_apply(const T& arg1)
    {
        return make_tuple(Template<T>::apply(arg1));
    }
};

template<template<class...> class Template, class... Rest>
inline auto make_tuple_apply(Rest... args)
{
    return make_tuple_apply_class<Template, Rest...>::make_tuple_apply(args...);
}


template<std::size_t Length, class... Tuple> struct BindTuple
{

    template<template<class, class...> class Function, class Vars, class... Typelist>
    constexpr static void apply(std::tuple<Tuple...> tuple, Vars &args)
    {
        BindTuple<Length - 1, Tuple...>:: template apply<Function, Vars, Typelist...>(tuple, args);
        applyTypelistToTuple<Function, Vars, Typelist...>(tuple, args);
    }


    template<template<class, class...> class Function, class Vars, class First, class Second, class... Typelist>
    constexpr static void applyTypelistToTuple(std::tuple<Tuple...> tuple, Vars &args)
    {
      //for each in typelist
        applyTypelistToTuple<Function, Vars, First>(tuple, args);
        applyTypelistToTuple<Function, Vars, Second, Typelist...>(tuple, args);
    }

    template<template<class, class...> class Function, class Vars, class First>
    constexpr static void applyTypelistToTuple(std::tuple<Tuple...> tuple, Vars &args)
    {
        Function<First>::apply(std::get<Length>(tuple), args);
    }

};

template<class... Tuple> struct BindTuple<0, Tuple...>
{
    template<template<class, class...> class Function, class Vars, class... Typelist>
    constexpr static void apply(std::tuple<Tuple...> tuple, Vars& args)
    {
        applyTypelistToTuple<Function, Vars, Typelist...>(tuple, args);
    }


    template<template<class, class...> class Function, class Vars, class First, class Second, class... Typelist>
    constexpr static void applyTypelistToTuple(std::tuple<Tuple...> tuple, Vars& args)
    {
      //for each in typelist
        apply<Function, Vars, First>(tuple, args);
        apply<Function, Vars, Second, Typelist...>(tuple, args);
    }

    template<template<class, class...> class Function, class Vars, class First>
    constexpr static void applyTypelistToTuple(std::tuple<Tuple...> tuple, Vars& args)
    {
        Function<First>::apply(std::get<0>(tuple), args);
    }

};

template<class... TypeList> struct TupleDeduce
{
    template<template<class, class...> class Function, class Vars, class... Tuple>
    constexpr static void apply(std::tuple<Tuple...> tuple, Vars& args)
    {
        typedef BindTuple<std::tuple_size< std::tuple<Tuple...> >::value - 1, Tuple...> Bound;
        Bound::template apply<Function, Vars, TypeList...>(tuple, args);
    }
};

template<template<class...> class Function, class... TypeList, class Tuple, class Vars> constexpr void bindTuple(Tuple tuple, Vars& args){
    TupleDeduce<TypeList...>::template apply<Function, Vars>(tuple, args);
}

// https://stackoverflow.com/questions/1198260/iterate-over-tuple#6894436

template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
  tuple_for_each(std::tuple<Tp...> &, FuncT) // Unused arguments are given no names.
  { }

template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I < sizeof...(Tp), void>::type
  tuple_for_each(std::tuple<Tp...>& t, FuncT f)
  {
    f(std::get<I>(t));
    tuple_for_each<I + 1, FuncT, Tp...>(t, f);
  }



template<template<typename> typename FuncTemplate, std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
  tuple_for_each(std::tuple<Tp...> &) // Unused arguments are given no names.
{}

template<template<typename> typename FuncTemplate, std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I < sizeof...(Tp), void>::type
  tuple_for_each(std::tuple<Tp...>& t)
  {
    DEBUG(3,  typeName<typename tuple_element<I, std::tuple<Tp...>>::type>(), "\n");
    FuncTemplate<typename std::tuple_element<I, typename std::tuple<Tp...>>::type>::apply(std::get<I>(t));
    tuple_for_each<FuncTemplate, I + 1, Tp...>(t);
  }


template<class Tuple, template<typename> typename FuncTemplate, template<class> class Collect, std::size_t I = 0>
inline auto
  tuple_for_each(typename std::enable_if<I == std::tuple_size<Tuple>::value - 1, nullptr_t>::type = nullptr) // Unused arguments are given no names.
{
    typedef FuncTemplate<typename std::tuple_element< /*0*/ I, Tuple>::type> Class;

#ifdef NDEBUG
    cout << "Class is` " << red << typeName<Class>() << normal << endl;
#endif

    return Collect< decltype(Class::apply()) >::apply(
        Class::apply()
    );
}

template<class Tuple, template<typename> typename FuncTemplate, template<class> class Collect, std::size_t I = 0>
inline auto
  tuple_for_each(typename std::enable_if<I < std::tuple_size<Tuple>::value - 1, nullptr_t>::type = nullptr)
  {
    typedef FuncTemplate<typename std::tuple_element< /*0*/ I, Tuple>::type> Class;
#ifdef NDEBUG
    cout << "Class is` " << red << typeName<Class>() << normal << endl;
#endif

     return Collect< decltype(Class::apply()) >::apply(
         Class::apply(),
         tuple_for_each<Tuple, FuncTemplate, Collect, I + 1>()
     );
  }


template <typename T, typename Tuple>
struct has_type;

template <typename T>
struct has_type<T, std::tuple<>> : std::false_type {};

template <typename T, typename U, typename... Ts>
struct has_type<T, std::tuple<U, Ts...>> : has_type<T, std::tuple<Ts...>> {};

template <typename T, typename... Ts>
struct has_type<T, std::tuple<T, Ts...>> : std::true_type {};

template <typename T, typename Tuple>
using tuple_contains_type = typename has_type<T, Tuple>::type;


#endif // TUPLEUTILS_H
