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
	template<typename T,
		typename = std::enable_if_t<
			std::is_arithmetic<T>::value
		>
	>
	std::pair<T,T> elementwise_mean(const vector<std::pair<T,T>>& vt)
	{
		using Tx = std::pair<T,T>;
		return [&vt](Tx accum) {
			//integer division ok here for now
			return make_pair(
				vt.size() ? accum.first / vt.size() : std::numeric_limits<T>::infinity(),
				vt.size() ? accum.second / vt.size() : std::numeric_limits<T>::infinity()
			); 
		}(
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
		using point_mass = pair<T,T>;
		vector<point_mass> point_unit_masses(vt.size());

		//compute point masses
		std::transform(
			vt.begin(), vt.end() //input range
			,point_unit_masses.begin() //start of output range
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
		
		//find unscaled cg
		auto unscaled_cg = std::move(
			//use above function
			my::elementwise_mean(point_unit_masses)
		);
		
		//return scaled result
		return make_pair(
				unscaled_cg.first/total_mass,
				unscaled_cg.second/total_mass
		);
		return make_pair(0,0);
	}
}