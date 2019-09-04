//
//  ResultsUtilsSingle.hpp
//  LCA.Kernel
//
//  Created by francois saab on 2/17/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef LCAUtilsForeground_hpp
#define LCAUtilsForeground_hpp

#include <stdio.h>
#include <vector>
#include <sstream>
#include <time.h>
#include <iostream>
#include <string>

#include "../../Utilities/FileUtils.hpp"
#include "../../LCAModels/Exchange.hpp"
#include "../../DAL/ExchangeObj.hpp"
#include "../../LCAModels/CalcImpactFactor.hpp"
#include "../../DAL/CalcImpactFactorObj.hpp"
#include "../../LCAModels/ExchangeItem.hpp"
#include "../../LCAModels/ExchangeCell.hpp"
#include "../../LCAModels/CalcImpactFactorItem.hpp"
#include "../../LCAModels/LCAIndexes.hpp"
#include "../../DAL/LCADB.hpp"
#include "../../Calculators/Models/CalculatorData.h"
#include "../../Uncertainties/EntitySampler.hpp"
#include "../../Graph/GraphData.h"

class LCAUtilsForeground
{
  public:
    LCADB *lcadb;
    CalculatorData *calculatorData;

    LCAUtilsForeground(LCADB *lcadb_, CalculatorData *calculatorData_)
    {
        lcadb = lcadb_;
        calculatorData = calculatorData_;
    }

    vector<long> getElementaryFlowIdsArray(vector<long> processesIds)
    {

        std::vector<long> resV;

        for (auto &&pid : processesIds)
        {

            vector<long> v = (*lcadb).elementaryExchangesByProcess[pid];

            for (auto &&exchid : v)
            {

                Exchange e = (*lcadb).exchanges[exchid];

                resV.push_back(e.flowId);
            }
        }

        return resV;
    }

    vector<ExchangeItem> getElementaryFlowsArray(vector<long> processesIds)
    {
        std::vector<ExchangeItem> resV;

        for (auto &&pid : processesIds)
        {

            vector<long> v = (*lcadb).elementaryExchangesByProcess[pid];

            // resV.insert(resV.end(),v.begin(),v.end());

            for (auto &&exchid : v)
            {

                Exchange e = (*lcadb).exchanges[exchid];

                ExchangeItem eitem = ExchangeItem{

                    (*calculatorData).lcaIndexes.ElementaryFlowsIndexIndices[e.flowId],
                    (*calculatorData).lcaIndexes.ProcessesIndexIndices[pid],
                    (*calculatorData).lcaIndexes.ElementaryFlowsIndexIndices[e.flowId],
                    (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndexIndices[pid],
                    (*calculatorData).lcaIndexes.ElementaryFlowsIndexIndices[e.flowId],
                    (*calculatorData).lcaIndexes.BackgroundLayer_ProcessesIndexIndices[pid],

                    e};

                resV.push_back(eitem);
            }
        }

        return resV;
    }
    vector<ExchangeItem> getOutputIntermediateFlowsArray(vector<long> processesIds)
    {

        std::vector<ExchangeItem> resV;

        for (auto &&pid : processesIds)
        {

            vector<long> v = (*lcadb).outputExchangesByProcess[pid];

            // resV.insert(resV.end(),v.begin(),v.end());

            for (auto &&exchid : v)
            {

                Exchange e = (*lcadb).exchanges[exchid];

                 ExchangeItem eitem = ExchangeItem{

                    (*calculatorData).lcaIndexes.IntermediateFlowsIndexIndices[e.exchangeId],
                    (*calculatorData).lcaIndexes.ProcessesIndexIndices[pid],
                    (*calculatorData).lcaIndexes.FrontLayer_IntermediateFlowsIndexIndices[e.exchangeId],
                    (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndexIndices[pid],
                    (*calculatorData).lcaIndexes.BackgroundLayer_IntermediateFlowsIndexIndices[e.exchangeId],
                    (*calculatorData).lcaIndexes.BackgroundLayer_ProcessesIndexIndices[pid],
                    e};

                resV.push_back(eitem);
            }
        }

        return resV;
    }

    vector<long> getOutputIntermediateFlowIdsArray(vector<long> processesIds)
    {

        std::vector<long> resV;

        for (auto &&pid : processesIds)
        {
            vector<long> v = (*lcadb).outputExchangesByProcess[pid];
            resV.insert(resV.end(), v.begin(), v.end());
        }

        return resV;
    }

    vector<ExchangeItem> getIntermediateFlowsArray(vector<long> processesIds)
    {

        std::vector<ExchangeItem> resV;

        for (auto &&pid : processesIds)
        {

            long out_exch = (*lcadb).outputExchangesByProcess[pid][0];

            Exchange e = (*lcadb).exchanges[out_exch];

            ExchangeItem eitem = ExchangeItem{
                (*calculatorData).lcaIndexes.IntermediateFlowsIndexIndices[e.exchangeId],
                (*calculatorData).lcaIndexes.ProcessesIndexIndices[pid],
                e};

            resV.push_back(eitem);

            vector<long> v = (*lcadb).inputExchangesByProcess[pid];

            // resV.insert(resV.end(),v.begin(),v.end());

            for (auto &&exchid : v)
            {

                Exchange e = (*lcadb).exchanges[exchid];

                ExchangeItem eitem = ExchangeItem{

                    (*calculatorData).lcaIndexes.IntermediateFlowsIndexIndices[out_exch], // this influences nothing
                    (*calculatorData).lcaIndexes.ProcessesIndexIndices[pid],
                    e};

                resV.push_back(eitem);
            }
        }

        return resV;
    }
};

#endif /* LCAUtilsForeground_hpp */
