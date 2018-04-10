#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <type_traits>
#include <sstream>
#include <numeric>
#include <algorithm>

#include "io.h"
#include "Box.h"
#include "utils.h"

//convert to collection of unit tests
//CHECK_EQUALS(), ASSERT_TRUE() type stuff, etc
int main(int argc, char *argv[])
{
	using namespace my;
	using Box = B<unsigned>;
	using V = std::vector<Box>;
	
	//expect cg at 3,3
	cout << center_of_gravity(V{Box{1,1,2,2},Box{3,3,2,2}});
	//expect 4,4
	cout << center_of_gravity(
	V{
		Box{0,0,2,2},
		Box{0,7,2,2},
		Box{3,3,2,2},
		Box{7,0,2,2},
		Box{7,7,2,2}
	});
	cout << center_of_gravity(
	V{
		Box{0,0,2,2},
		Box{0,7,2,2},
		Box{7,0,2,2},
		Box{7,7,2,2}
	});
	cout << center_of_gravity(
	V{
		Box{0,0,2,2},
		Box{0,7,2,2},
		Box{2,2,4,4},
		Box{7,0,2,2},
		Box{7,7,2,2}
	});

	return 0;
}

