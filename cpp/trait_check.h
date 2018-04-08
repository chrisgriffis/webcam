
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

}