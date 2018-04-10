#include <vector>
#include <type_traits>
#include <numeric>
#include <algorithm>

namespace my {
	using namespace std;
	
	//flatten by removing one dimension of nested containers
	template<typename T>
	vector<T> flatten(vector<vector< T >>&& vvt)
	{
		vector<T> flattened;
		for(auto&& vwt : vvt ) 
			for(auto&& wt : vwt ) 
				flattened.push_back(std::move(wt));
		return flattened;
	}

	//compute the center of gravity of unit mass boxes
	template<typename T,
		typename = std::enable_if_t<
			std::is_arithmetic<T>::value
		>
	>
	pair<T,T> center_of_gravity(vector< B<T>> vt)
	{
		using Box = B<T>;
		using point_mass = pair<T,T>;
		vector<point_mass> point_masses(vt.size());

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
		T total_mass =
			std::accumulate(
				std::next(vt.begin()), vt.end()
				,vt[0]._w * vt[0]._h //init val
				,[](T a, Box b) { 
					return a + b._w * b._h; 
				} //need to detect overflow...
			);
		
		return [total_mass](point_mass accum) {
			//integer division ok here for now
			return make_pair(
				total_mass ? accum.first / total_mass : std::numeric_limits<T>::infinity(),
				total_mass ? accum.second / total_mass : std::numeric_limits<T>::infinity()
			); 
		}(
			std::accumulate(
				std::next(point_masses.begin()), point_masses.end()
				,point_masses[0] //init val
				,[](point_mass a, point_mass b) { 
					return point_mass{
						a.first + b.first,
						a.second + b.second
					};
				} //need to detect overflow...
			)
		);
	}
}