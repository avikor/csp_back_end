#pragma once

#include <string>
#include "json.hpp"

nlohmann::json get_magic_squares_solutions(int n, const std::string& strNSquared);