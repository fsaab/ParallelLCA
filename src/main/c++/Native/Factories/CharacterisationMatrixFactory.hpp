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
#ifndef CharacterisationMatrixFactory_hpp
#define CharacterisationMatrixFactory_hpp

#include <stdio.h>
#include <vector>
using namespace std;
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>
#include <tr1/unordered_map>
using Eigen::VectorXd;
using Eigen::BiCGSTAB;
using Eigen::SparseMatrix;
#include <boost/range/algorithm.hpp>
#include <tr1/unordered_map>

#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/AppSettings.hpp"
#include "../DAL/LCADBUtils.h"
#include "../Calculators/Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../libs/libEigen.hpp"
#include "../Utilities/LCAUtils.hpp"
#include "../DAL/LCADB.hpp"

class CharacterisationMatrixFactory {
public:

    CalculatorData * calculatorData;
    LCADB * lcadb;
    AppSettings settings;

    CharacterisationMatrixFactory(LCADB * lcadb_, CalculatorData * calculatorData_, AppSettings settings_) : settings(settings_) {

        calculatorData = calculatorData_;
        lcadb = lcadb_;

    }

    CharacterisationMatrixFactory() {



    }

     vector<CalcImpactFactorItem> impactFactors(AppSettings settings, long *Qi_ptr, long *QJ_ptr, vector<string> Q_ptr
            ) {

        LCADBUtils dbUtils(lcadb,settings);

        return dbUtils.loadImpactCells(Qi_ptr, QJ_ptr, Q_ptr);

    }

    vector<CalcImpactFactorItem> impactFactors() {

        vector<CalcImpactFactorItem> calc_flow_factor_items;

        try {

            boost::copy(boost::unique(boost::sort( (*lcadb).impactMethodCategories[settings.ImpactMethodId])),
                std::back_inserter( (*calculatorData).lcaIndexes.ImpactCategoryIndex));
        
            (*calculatorData).lcaIndexes.loadIndexesForImpactCategories();

            for(long f:(*calculatorData).lcaIndexes.ElementaryFlowsIndex){
               for(long c:  (*calculatorData).lcaIndexes.ImpactCategoryIndex){

                long j = (*calculatorData).lcaIndexes.ElementaryFlowsIndexIndices[f];
                long i = (*calculatorData).lcaIndexes.ImpactCategoryIndexIndices[c];

                   std::pair<long,long> key=std::make_pair(c,f);
                   if((*lcadb).impactFactors.find(key)!= (*lcadb).impactFactors.end())
                   {
                       CalcImpactFactor cif=(*lcadb).impactFactors[key];
                   
                       calc_flow_factor_items.push_back({i,j,cif});
                   }
                   
               }
            }

        } catch (std::exception& e) {
            std::cerr << "Exception catched : " << e.what() << std::endl;
        }
        return calc_flow_factor_items;

    }

    vector<CalcImpactFactorItem> impactFactorsAll() {

        vector<CalcImpactFactorItem> calc_flow_factor_items;

        try {

             boost::copy(boost::unique(boost::sort( (*lcadb).impactMethodCategories[settings.ImpactMethodId])),
                std::back_inserter( (*calculatorData).lcaIndexes.ImpactCategoryIndex));
        
            (*calculatorData).lcaIndexes.loadIndexesForImpactCategories();

            for(long f:(*calculatorData).lcaIndexes.ElementaryFlowsIndex){
               for(long c:  (*calculatorData).lcaIndexes.ImpactCategoryIndex){

                   
                long j = (*calculatorData).lcaIndexes.ElementaryFlowsIndexIndices[f];
                long i = (*calculatorData).lcaIndexes.ImpactCategoryIndexIndices[c];

                   std::pair<long,long> key=std::make_pair(c,f);
                   CalcImpactFactor cif=(*lcadb).impactFactors[key];
                   
                    calc_flow_factor_items.push_back({i,j,cif});
               }
            }

        } catch (std::exception& e) {
            std::cerr << "Exception catched : " << e.what() << std::endl;
        }
        return calc_flow_factor_items;

    }

    static double getImpactCellValue(CalcImpactFactorItem c) {

        return c.imf.amount / c.imf.conversionFactor;

    }

    static SMatrix build(
            AppSettings settings, CalculatorData* calculatorData) {

        long Qlength = ((*calculatorData).Q_cells).size();

        long * rowsQ_ptr = new long[Qlength];
        long * colsQ_ptr = new long[Qlength];
        double* dataQ_ptr = new double[Qlength];


        int i = 0;
        for (auto &&cell : (*calculatorData).Q_cells) {

            CalcImpactFactorItem ent = cell;
            rowsQ_ptr[i] = ent.i;
            colsQ_ptr[i] = ent.j;
            dataQ_ptr[i] = getImpactCellValue(ent);

            i++;

        }

        int Qcols = (*calculatorData).lcaIndexes.ElementaryFlowsIndexLength();
        int Qrows = (*calculatorData).lcaIndexes.ImpactCategoryIndexLength();

        //cout<<"Q: "<<Qrows<<" "<<Qcols<<endl;

        SMatrix Q(Qrows, Qcols);
        libEigen::fillSparseMatrix(&Q, Qlength, rowsQ_ptr, colsQ_ptr, dataQ_ptr);

        return Q;


    }


};

#endif /* CharacterisationMatrixFactory_hpp */
