#include <iostream>
#include <utility>
#include <tuple>
#include <functional>
using namespace std;

namespace static_indexing
{
    template<template <class> class Op, class Tuple, size_t... Is>
    constexpr auto tuple_apply_op_impl(Tuple t1, Tuple t2, index_sequence<Is...>)
    {
        return make_tuple(Op<tuple_element_t<Is,Tuple>>{}(get<Is>(t1), get<Is>(t2))...);
    }

    template<template <class> class Op, class Tuple>
    constexpr auto tuple_apply_op(Tuple t1, Tuple t2)
    {
        return tuple_apply_op_impl<Op>(t1,t2,make_index_sequence<tuple_size_v<Tuple>>{});
    }
}

/*
int main()
{
    auto t = static_indexing::tuple_apply_op<std::plus>(make_tuple(1, 1), make_tuple(-1, -1));
    cout << "(" << get<0>(t) << "," << get<1>(t) << ")" << endl;

    t = static_indexing::tuple_apply_op<std::multiplies>(make_tuple(1, 1), make_tuple(-1, -1));
    cout << "(" << get<0>(t) << "," << get<1>(t) << ")" << endl;

    int sdfgwert;
	cin >> sdfgwert;
    return 0;
}
*/