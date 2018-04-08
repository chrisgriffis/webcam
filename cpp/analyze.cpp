#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <type_traits>
#include <sstream>
#include <numeric>
#include <algorithm>

#include "io.h"
#include "Box.h"
#include "utils.h"


int main(int argc, char *argv[])
{
	using namespace my;
	//using namespace my::debug;
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

