#pragma once

#include "gap/lib/instance.hpp"
#include "gap/lib/solution.hpp"

namespace gap
{

Solution sopt_cplex(const Instance& ins, Info* info = NULL);

}

