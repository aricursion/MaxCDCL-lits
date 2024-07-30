//
// Created by Jordi Coll Caballero on 26/04/2022.
//

#include "MHSIface.h"

#include "maxhs/core/MaxSolver.h"
#include "maxhs/core/Wcnf.h"
#include "maxhs/utils/Params.h"
#include <stdio.h>

MHSIface::MHSIface(){
    LB=0;
    UB=INT64_MAX;
}

void MHSIface::solve(char * iname){
    error=false;
    static MaxHS::MaxSolver* thesolver{};

    params.readOptions();
    params.verbosity=0;

    Wcnf theFormula{};
    if (!theFormula.inputDimacs(iname)){
        error=true;
    }
    else {
        MaxHS::MaxSolver S(&theFormula);
        thesolver = &S;
        S.solve();

        UB = S.UB() + theFormula.baseCost();
        LB = S.LB() + theFormula.baseCost();
        if(LB>UB)
            error=true;
        else if(UB<INT64_MAX){
	S.optFound("");
            for (size_t i = 0; i < S.external_model.size(); i++)
                sol.push_back(S.external_model[i] == l_True);
}


        fflush(stdout);
        fflush(stderr);
    }

}

void MHSIface::printSol(){
    printf("v ");
    for (bool b : sol)
        printf(b ? "1" : "0");
    printf("\n");
}
