//
// Created by Jordi Coll Caballero on 26/04/2022.
//

#ifndef WMAXCDCL1_5_MHSIFACE_H
#define WMAXCDCL1_5_MHSIFACE_H

#   include <inttypes.h>
#include <vector>

class MHSIface {
public:
    MHSIface();
    int64_t LB;
    int64_t UB;
    bool error;
    std::vector<bool> sol;
    void solve(char * iname);
    void printSol();
};


#endif //WMAXCDCL1_5_MHSIFACE_H
