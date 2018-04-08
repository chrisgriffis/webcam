#include <type_traits>
#include <iostream>

namespace my {

		
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
		friend std::ostream& operator<<(std::ostream& s, B& b)
		{
			s <<
				"x: " << b._x << "; " <<
				"y: " << b._y << "; " <<
				"w: " << b._w << "; " <<
				"h: " << b._h << "; \n";
			return s;
		}
	};
	
}