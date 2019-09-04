//
//  LCIContribution.h
//  LCA.Kernel
//
//  Created by francois saab on 2/18/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef LCIContribution_h
#define LCIContribution_h


#include <stdio.h>

#include <string>
#include <vector>
#include "../../LCAModels/CalcImpactFactorItem.hpp"
#include "../../LCAModels/ExchangeItem.hpp"
using namespace std;
#include "AppSettings.hpp"
#include "../../LCAModels/LCAIndexes.hpp"
#include "ImpactStat.hpp"

struct LCIContribution {
    long flow;
    double q;
    long proc;
    int unit;
    double contr;
    double scalar;

    LCIContribution(long flow_,
            double q_,
            long proc_,
            int unit_,
            double contr_,
            double scalar_) {

                flow=flow_;
                q=q_;
                proc=proc_;
                unit=unit_;
                contr=contr_;
                scalar=scalar_;
    }
};


#endif /* LCIContribution_h */
