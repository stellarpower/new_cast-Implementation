//          Copyright Benjamin Southall 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#include <iostream>
#include <memory>
#include <type_traits>

#include "Object.h"
#include "Classes.h"
#include "NewCast.h"

const void* ObjectIF::newCast(const type_info &destinationType) const
{
    return new_cast(pointer(), destinationType); //fixme!
}

const void* ObjectIF::newCast3Arg(const type_info &destinationType) const
{
    return new_cast(staticType(), pointer(), destinationType); //fixme!
}



template< template<class> class Original, template<class> class Forwarder > struct Bind1{
    template<class T> using type = Original<Forwarder<T>>;
};

template<typename T> using DePointer = typename std::pointer_traits<T>::element_type;

