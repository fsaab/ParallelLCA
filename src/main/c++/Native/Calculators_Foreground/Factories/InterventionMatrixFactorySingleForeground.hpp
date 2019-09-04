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

#ifndef InterventionMatrixFactorySingleForeground_hpp
#define InterventionMatrixFactorySingleForeground_hpp

#include <stdio.h>
#include <vector>
using namespace std;

#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>
#include <algorithm>
using Eigen::BiCGSTAB;
using Eigen::SparseMatrix;
using Eigen::VectorXd;
#include <tr1/unordered_map>
typedef Eigen::Triplet<double> Triplet;
typedef Eigen::SparseMatrix<double> SMatrix;
#include "../../LCAModels/LCAIndexes.hpp"
#include <boost/range/algorithm.hpp>

#include "../../Calculators/Models/CalculatorData.h"
#include "../../Utilities/ResultsUtilsSingle.hpp"
#include "../../LCAModels/LCAIndexes.hpp"
#include "../../libs/libEigen.hpp"
#include "../../Uncertainties/EntitySampler.hpp"
#include "../../DAL/LCADBUtils.h"
#include "../Utilities/LCAUtilsForeground.hpp"
#include "../../LCAModels/ExchangeItem.hpp"
#include "../../Utilities/ResultsUtilsSingle.hpp"
#include "../../Calculators/Models/AppSettings.hpp"

class InterventionMatrixFactorySingleForeground
{
  public:
    CalculatorData *calculatorData;
    LCADB *lcadb;

    InterventionMatrixFactorySingleForeground() {}

    InterventionMatrixFactorySingleForeground(LCADB *lcadb_, CalculatorData *calculatorData_)
    {

        calculatorData = calculatorData_;
        lcadb = lcadb_;
    }

    vector<ExchangeItem> BExchangeItems()
    {

        LCAUtilsForeground lcaUtils(lcadb, calculatorData);

        std::vector<long> v = lcaUtils.getElementaryFlowIdsArray((*calculatorData).lcaIndexes.ProcessesIndex);
        boost::copy(boost::unique(boost::sort(v)), std::back_inserter((*calculatorData).lcaIndexes.ElementaryFlowsIndex));
        (*calculatorData).lcaIndexes.loadIndexesForElementaryFlows();

        vector<ExchangeItem> flows;
        vector<ExchangeItem> elemflows = lcaUtils.getElementaryFlowsArray((*calculatorData).lcaIndexes.ProcessesIndex);
        flows.insert(flows.end(), elemflows.begin(), elemflows.end());

        return flows;
    }

    vector<ExchangeItem> exchanges(AppSettings settings, long *Bi_ptr, long *Bj_ptr, vector<string> B_ptr)
    {

        LCADBUtils dbUtils(lcadb, settings);

        return dbUtils.loadExchangeItems(Bi_ptr, Bj_ptr, B_ptr);
    }
};

#endif /* InterventionMatrixFactorySingle_hpp */
