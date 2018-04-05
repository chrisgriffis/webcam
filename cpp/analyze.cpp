#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <type_traits>
using namespace std;

template<class T
	//,typename = std::enable_if_t<read_count_defined<T>::value>
	//,typename = decltype(declval<T>().operator<<(declval<const std::vector<T::value_type>&>()),void())
>
void Read(istream& ifs, T& val)
{
	constexpr auto read_count = T::read_count;
	using value_type = typename T::value_type;
	std::vector<value_type> packet(T::read_count);
	for(auto& e : packet) ifs >> e;
	val << packet;
}


template<template <class, class> class C, class T, class A>
void Read(istream& ifs, C<T,A>& collection)
{
	//const initializer trick
	const size_t N = [&ifs]
	{
		size_t val;
		ifs >> val;
		return val;
	}();
	collection.resize(N);
	for( auto& e : collection)
		Read(ifs,e);
}

template<typename T=int,
	typename = std::enable_if_t<
		std::is_integral<T>::value
	>
>
struct B //Box POD of 4 integral values
{
	using value_type = typename std::remove_reference<T>::type;
	constexpr static std::size_t read_count = 4;
	//c++ does a fine job making default (copy,assign,move,etc) for this kind of struct so not touching
	//explicit constructor below deletes deletes default constructor unless brought back
	B(value_type x=0,value_type y=0, value_type w=0, value_type h=0) :
		_x(x),_y(y),_w(w),_h(h) {}
	value_type _x,_y,_w,_h;
	
	B& operator<<(const std::vector<value_type>& packet)
	{
		if(packet.size() != read_count) throw;
		_x=packet[0];
		_y=packet[1];
		_w=packet[2];
		_h=packet[3];
		return *this;
	}
	friend ostream& operator<<(ostream& s, B& b)
	{
		s <<
			"x: " << b._x << "; " <<
			"y: " << b._y << "; " <<
			"w: " << b._w << "; " <<
			"h: " << b._h << "; \n";
		return s;
	}
};

//for debugging
template<template <class,class> class C, class T, class A>
ostream& operator<<(ostream& s, C<T,A>& v)
{
	s << "\t{\n";
	for (T& e : v) s << e;
	s << "\t}\n";
	return s;
}


int main(int argc, char *argv[])
{
	//2 264 125 139 139, 97 112 141 141
	B<int> b1{264, 125, 139, 139};
	B<int> b2;
	vector<int> v{97, 112, 141, 141};
	b2 << v;
	
	vector<B<int>> foo{ b1,b2};

	//vector<B<int>> vbar;
	//ifstream i(argv[1]);
	//Read(i, vbar);

	//cout << vbar;
	cin >> b1._x;
	return 0;
}

