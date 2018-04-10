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
	
	auto cg = center_of_gravity(std::move(flatten(std::move(vvboxes))));
	ofs << "center of gravity (closer faces are heavier): \n" << cg;
	
	return 0;
}

