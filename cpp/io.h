#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "trait_check.h"

namespace my {
	using namespace std;

		//read a stream into a value that meets certain compile time requirements
	template<class T
		//exposes a read count
		,typename = std::enable_if_t<read_count_defined<T>::value>
		//has a stream operator that accepts a vector of value types
		,typename = decltype(declval<T>().operator<<(declval<const std::vector<typename T::value_type>&>()),void())
	>
	void Read(istream& ifs, T& val)
	{
		//std::cout << "reading object" << std::endl;
		using type = typename std::remove_reference<T>::type;
		constexpr auto read_count = type::read_count;
		using value_type = typename type::value_type;
		std::vector<value_type> packet(type::read_count);
		for(auto& e : packet) ifs >> e;
		val << packet;
	}

	//read a stream into a collection of values
	template<template <class, class> class C, class T, class A>
	void Read(istream& ifs, C<T,A>& collection)
	{
		//std::cout << "reading collection" << std::endl;
		
		//lambda used here for "const initialization trick"
		//to initialize a const value with multiple steps
		const size_t N = [&ifs]
		{
			size_t val;
			ifs >> val;
			return val;
		}();
		collection.resize(N);
		for( auto& e : collection)
			Read<typename std::remove_reference<decltype(e)>::type>(ifs,e);
			//Read(ifs,e);
	}


	//namespace debug
	//{
		//for debugging
		//can work recursively on nesting for a collection of collections of collections...
		template<
			template <class,class> class C
			, class T
			, class A
			//adding what follows for better template matching, 
			//otherwise could look like a std::pair
			, typename = decltype( 
				declval<C<T,A>>().begin(), //require iterable
				declval<C<T,A>>().end(), //require iterable
				void())
		>
		std::ostream& operator<<(std::ostream& s, C<T,A> v)
		{
			for (T& e : v) s << e;
			return s << std::endl;
		}

		//print a std::pair as a cartesian ordered pair
		template<class T1, class T2>
		std::ostream& operator<<(std::ostream& s, std::pair<T1,T2> p)
		{
			s << "(" << p.first << "," << p.second << ")";
			return s << std::endl;
		}
		
	//}
		
}