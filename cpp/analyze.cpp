#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <type_traits>
#include <sstream>
#include <numeric>
#include <algorithm>

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
	ostream& operator<<(ostream& s, C<T,A>& v)
	{
		for (T& e : v) s << e;
		return s << endl;
	}

	//print a std::pair as a cartesian ordered pair
	template<class T1, class T2>
	ostream& operator<<(ostream& s, std::pair<T1,T2>& p)
	{
		s << "(" << p.first << "," << p.second << ")";
		return s << endl;
	}

	//flatten by removing one dimension of nested containers
	template<typename T>
	vector<T> flatten(vector<vector< T >>&& vvt)
	{
		vector<T> flattened;
		for(auto&& vwt : vvt ) 
			for(auto&& wt : vwt ) 
				flattened.push_back(std::move(wt));
	}

	//compute the elementwise mean
	template<typename T>
	T elementwise_mean(const vector< T>& vt)
	{
		return [&vt](T accum) {
			return accum / vt.size(); //integer division ok here for now
		}(
			std::accumulate(
				std::next(vt.begin()), vt.end()
				,vt[0] //init val
				,std::plus<T>() //need to detect overflow...
			)
		);
	}
	
	//compute the elementwise mean
	template<typename T>
	std::pair<T,T> elementwise_mean(const vector<std::pair<T,T>>& vt)
	{
		using Tx = std::pair<T,T>;
		return [&vt](Tx accum) {
			//integer division ok here for now
			return make_pair(
				accum.first / vt.size(),
				accum.second / vt.size()
			); 
		}(make_pair(0,0)
/*
			std::accumulate(
				std::next(vt.begin()), vt.end()
				,vt[0] //init val
				,[](Tx a, Tx b) { 
					return make_pair(
						a.first + b.first,
						a.second + b.second
					);
				} //need to detect overflow...
			)
*/
		);
	}
	
	//compute the center of gravity of unit mass boxes
	template<typename T,
		typename = std::enable_if_t<
			std::is_arithmetic<T>::value
		>
	>
	pair<T,T> center_of_gravity(const vector< B<T>>& vt)
	{
		using Box = B<T>;
		vector<pair<T,T>> point_masses;
		
		//compute point masses
		std::transform(
			vt.begin(), vt.end() //input range
			,point_masses.begin() //start of output range
			,[](Box b) { //comput sum(mi*ri)
				return make_pair(
					//weighted centerpoint.x
					(b._x + b._w/2)*b._w*b._h,
					//weighted centerpoint.y
					(b._y + b._h/2)*b._w*b._h
				); 
			}
		);
		
		//compute total mass
		T total_mass = 0;
/*		
			std::accumulate(
				std::next(vt.begin()), vt.end()
				,vt[0]._w * vt[0]._h //init val
				,[](T a, Box b) { 
					return a + b._w * b._h; 
				} //need to detect overflow...
		);
*/			
		
		//find unscaled cg
		auto unscaled_cg = std::move(
			//use above function
			my::elementwise_mean(point_masses)
		);
		
		//return scaled result
		return make_pair(
				unscaled_cg.first/total_mass,
				unscaled_cg.second/total_mass
		);
		return make_pair(0,0);
	}
}


int main(int argc, char *argv[])
{
	using namespace my;
	using Box = B<unsigned>;
	vector<vector<Box>> vvboxes;
	ifstream ifs(argv[1]);
	for (string s; std::getline(ifs, s); ) 
	{
		vector<Box> vboxes;
		stringstream ss(s);
		Read(ss, vboxes);
		vvboxes.push_back(vboxes);
	}
	
	ofstream ofs(argv[2]);
	
	auto vboxes = std::move(flatten(std::move(vvboxes)));
	
	auto cg = center_of_gravity(vboxes);
	ofs << "center of gravity (closer faces are heavier): \n" << cg;
	
	return 0;
}

//convert to collection of unit tests
//CHECK_EQUALS(), ASSERT_TRUE() type stuff, etc
int test(int argc, char *argv[])
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
	std::cout << vvboxes; //recursive nesting stream operator on a collection!!
	return 0;
}

