#pragma once

#include <string>
#include <boost/optional.hpp>

namespace skinny {

//--------------------------------------------------------------
// poor man's clamp, need c++17 >:(
template<typename T>
T clamp(T v, T lo, T hi)
{
	return std::min(std::max(v, lo), hi);
}

//--------------------------------------------------------------
boost::optional<int> sstoi(const std::string& str);

}