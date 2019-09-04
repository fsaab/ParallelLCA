/**---------------------------   
 * PROJECT: ParallelLCA
 * Auth:
 *   Francois Saab
 * Mail: saab.francois@gmail.com, francois.saab.1@ens.etsmtl.ca
 * Date: 1/1/2017
 *
 * Copyright © 2017 Francois Saab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. 
 *
 *--------------------------*/
#ifndef LCAUtils_hpp
#define LCAUtils_hpp

#include <stdio.h>
#include <vector>
#include "FileUtils.hpp"
#include "../LCAModels/Exchange.hpp"
#include "../DAL/ExchangeObj.hpp"
#include "../LCAModels/CalcImpactFactor.hpp"
#include "../DAL/CalcImpactFactorObj.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/ExchangeCell.hpp"
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include <sstream>
#include <time.h>
#include <iostream>
#include <string>
#include "../LCAModels/LCAIndexes.hpp"
#include "../DAL/LCADB.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Uncertainties/EntitySampler.hpp"
#include "../Graph/GraphData.h"

class LCAUtils
{
  public:
    LCADB *lcadb;
    CalculatorData *calculatorData;

    LCAUtils(LCADB *lcadb_, CalculatorData *calculatorData_)
    {
        lcadb = lcadb_;
        calculatorData = calculatorData_;
    }

    map<long, double> mergeExchanges(long processId, vector<long> exchangeIds,
    map<string, expression_t> &expressionsTable)
    {

        map<long, double> exchangeMapTmp;

        EntitySampler<double> sampler;
        ParameterUtils<double> paramUtils;

        for (auto &&id : exchangeIds)
        {
            Exchange exch = (*lcadb).exchanges[id]; // get exchange object
            expression_t expressionTable_perid = paramUtils.getExpressionTable(exch.processId, "PROCESS", expressionsTable);
            double v = sampler.getCellValue(exch, expressionTable_perid);
            exchangeMapTmp[exch.flowId] = exchangeMapTmp[exch.flowId] + v;
        }
        
        return exchangeMapTmp;
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
                    -1,
                    -1,
                    -1,
                    -1,

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
                    -1,
                    -1,
                    -1,
                    -1,
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

#endif /* LCAUtils_hpp */
