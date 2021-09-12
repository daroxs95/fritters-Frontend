#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

//this is a very bad test, need to learn a bit from sol2 and improve it
int main(int, char*[]) 
{
	sol::state lua;

	try
	{
		// open some common libraries
		lua.open_libraries(sol::lib::base, sol::lib::package);
		lua.script("print('bark bark bark!')");
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return -1;
	}

	//do the test with this
	/*
	// load string without execute
	sol::load_result script2 = lua.load("a = 'test'");
	//execute
	sol::protected_function_result script2result = script2();
	// optionally, check if it worked
	if (script2result.valid()) {
		// yay!
	}
	else {
		// aww
	}
	*/
	return 0;
}