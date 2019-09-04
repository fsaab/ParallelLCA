//
//  LCIContribution.h
//  LCA.Kernel
//
//  Created by francois saab on 2/18/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef LCIAContribution_h
#define LCIAContribution_h


#include <stdio.h>

#include <string>
#include <vector>
#include "../../LCAModels/CalcImpactFactorItem.hpp"
#include "../../LCAModels/ExchangeItem.hpp"
using namespace std;
#include "AppSettings.hpp"
#include "../../LCAModels/LCAIndexes.hpp"
#include "ImpactStat.hpp"

struct LCIAContribution {
    long category;
    double impact;
    long proc;
    int unit;
    double contr;

    LCIAContribution() {

        category = -1;
        impact = 0;
        proc = -1;
        unit = -1;
        contr = 0;
    }

    LCIAContribution(long category_,
            double impact_,
            long proc_,
            int unit_,
            double contr_) {

        category = category_;
        impact = impact_;
        proc = proc_;
        unit = unit_;
        contr = contr_;

    }
};


#endif /* LCIContribution_h */
