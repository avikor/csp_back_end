#pragma once

#include <string>
#include "json.hpp"

nlohmann::json get_n_queens_solutions(const std::string& strN);