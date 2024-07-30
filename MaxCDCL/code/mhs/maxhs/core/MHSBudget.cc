#include "maxhs/core/MHSBudget.h"
#include "minisat/utils/System.h"

bool MHSBudget::letContinue = false;
int MHSBudget::timelimit=300;
bool MHSBudget::withinBudget(){
    return letContinue || Minisat::cpuTime()<timelimit;
}
