//
// Created by Jordi Coll Caballero on 09/05/2022.
//

#ifndef WMAXCDCL1_5_MHSBUDGET_H
#define WMAXCDCL1_5_MHSBUDGET_H

#include "minisat/utils/System.h"

 class MHSBudget{
public:
    static bool letContinue;
    static int timelimit;
    static bool withinBudget();
};

#endif //WMAXCDCL1_5_MHSBUDGET_H
