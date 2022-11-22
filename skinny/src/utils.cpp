#include <algorithm>

#include "utils.h"

namespace skinny {

//--------------------------------------------------------------
boost::optional<int> sstoi(const std::string& str)
{
	try
	{
		const auto i = std::stoi(str);
		return i;
	}
	catch (...)
	{
		return boost::none;
	}
}

}