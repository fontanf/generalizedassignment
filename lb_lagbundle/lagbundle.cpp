#include "gap/lb_lagbundle/lagbundle.hpp"

#include "knapsack/opt_minknap/minknap.hpp"

#include <algorithm>

using namespace gap;

LagOut gap::lb_lagrangian(const SubInstance& sub,
        StateIdx it_total, StateIdx a, std::vector<Value>* mult_init, Info* info)
{
    (void)info;
    const Instance& ins = sub.instance();
    LagOut out(ins);
    std::vector<Value> multipliers_curr(ins.item_number(), 0);
    if (mult_init != NULL)
        multipliers_curr = *mult_init;
    for (ItemIdx j=0; j<ins.item_number(); ++j)
        if (sub.reduced_solution()->agent(j) >= 0)
            multipliers_curr[j] = 0;

    Value gamma = 0;
    for (AltIdx k=0; k<ins.alternative_number(); ++k)
        if (gamma < ins.alternative(k).v)
            gamma = ins.alternative(k).v;
    gamma *= 4;

    StateIdx it = 1;
    for (; it<it_total && gamma/(it/a+1) >= 1; ++it) {
        std::vector<std::vector<int>> xji(ins.item_number(),
                std::vector<int>(ins.agent_number(), 0));
        std::vector<AgentIdx> xj(ins.item_number(), 0);

        Value ub = 0;
        for (ItemIdx j=0; j<ins.item_number(); ++j)
            ub += multipliers_curr[j];

        for (AgentIdx i=0; i<ins.agent_number(); ++i) {
            knapsack::Instance ins_kp(sub.agent_alternative_number(i), sub.capacity(i));
            for (ItemIdx j=0; j<ins.item_number(); ++j) {
                AltIdx k = ins.alternative_index(j, i);
                if (sub.reduced_solution()->agent(j) == i) {
                    xj[j]++;
                    xji[j][i] = 1;
                    ub += ins.alternative(k).v;
                }
                if (sub.reduced(k) == -1)
                    continue;
                Value p = ins.alternative(k).v - multipliers_curr[j];
                if (p > 0)
                    ins_kp.add_item(ins.alternative(k).w, p);
            }
            knapsack::MinknapParams params;
            knapsack::Solution sol = knapsack::Minknap(ins_kp, params).run();
            ub += sol.profit();
            for (knapsack::ItemPos j_kp=0; j_kp<ins_kp.total_item_number(); ++j_kp) {
                //ItemIdx j_gap = ins_kp.item(j_kp).l;
                ItemIdx j_gap = 0;
                if (sol.contains(j_kp)) {
                    xji[j_gap][i] = 1;
                    xj[j_gap]++;
                }
            }
        }

        if (out.u == -1 || ub < out.u) {
            out.u    = ub;
            out.mult = multipliers_curr;
            out.xj   = xj;
            out.xji  = xji;
        }
        if (out.u < 0) {
            out.u = -1;
            return out;
        }

        // Update multipliers
        for (ItemIdx j=0; j<ins.item_number(); ++j) {
            if (xj[j] < 1) { // <=> x[j] == 0
                multipliers_curr[j] -= gamma / (it/a+1);
            } else if (xj[j] > 1) {
                multipliers_curr[j] += (gamma * (xj[j]-1)) / (it/a+1);
            }
        }
    }

    return out;
}

