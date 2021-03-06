#pragma once

#if CPLEX_FOUND

#include "generalizedassignmentsolver/solution.hpp"

namespace generalizedassignmentsolver
{

struct BranchAndCutCplexOptionalParameters
{
    Info info = Info();

    const Solution* initial_solution = NULL;
    bool only_linear_relaxation = false;
};

struct BranchAndCutCplexOutput: Output
{
    BranchAndCutCplexOutput(const Instance& instance, Info& info): Output(instance, info) { }
    BranchAndCutCplexOutput& algorithm_end(Info& info);

    std::vector<std::vector<double>> x;
};

BranchAndCutCplexOutput branchandcut_cplex(const Instance& instance,
        BranchAndCutCplexOptionalParameters parameters = {});

}

#endif

