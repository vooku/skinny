#pragma once

#include <boost/optional.hpp>
#include <string>

namespace skinny {

//--------------------------------------------------------------
boost::optional<int> sstoi(const std::string& str);

} // namespace skinny