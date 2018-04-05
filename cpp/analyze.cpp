#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <type_traits>
#include <sstream>

namespace my {
	using namespace std;
	
	//the ol' SFINAE "has member type" pattern, modified for named member
	template<typename T> 
	class read_count_defined 
	{
		using yes_t = char;
		using no_t = long long;
		template <typename C> static yes_t test(decltype(&C::read_count));
		template <typename C> static no_t test(...);
	public:
		static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_t);
	};
	
	//read a stream into a value that meets certain compile time requirements
	template<class T
		//exposes a read count
		,typename = std::enable_if_t<read_count_defined<T>::value>
		//has a stream operator that accepts a vector of value types
		,typename = decltype(declval<T>().operator<<(declval<const std::vector<typename T::value_type>&>()),void())
	>
	void Read(istream& ifs, T& val)
	{
		//cout << "reading object" << endl;
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
		//cout << "reading collection" << endl;
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

	//Data type that meets certain compile time requirements for using above Read mechanisms
	template<typename T=int,
		typename = std::enable_if_t<
			std::is_integral<T>::value
		>
	>
	struct B //Box POD of 4 integral values
	{
		using value_type = typename std::remove_reference<T>::type;
		constexpr static unsigned read_count = 4;
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
		for (T& e : v) s << e;
		return s << endl;
	}

}


int main(int argc, char *argv[])
{
	using namespace my;
	//2 264 125 139 139, 97 112 141 141
	B<int> b1{264, 125, 139, 139};
	B<int> b2;
	std::vector<int> v{97, 112, 141, 141};
	b2 << v;
	
	std::vector<B<int>> foo{ b1,b2};

	vector<vector<B<int>>> vvboxes;
	ifstream ifs(argv[1]);
	for (string s; std::getline(ifs, s); ) 
	{
		vector<B<int>> vboxes;
		stringstream ss(s);
		Read(ss, vboxes);
		vvboxes.push_back(vboxes);
	}

	std::cout << foo;
	std::cout << vvboxes;
	return 0;
}

