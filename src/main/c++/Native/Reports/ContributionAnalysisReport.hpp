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
#ifndef ContributionAnalysisReport_hpp
#define ContributionAnalysisReport_hpp

#include <stdio.h>
#include <vector>
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/AppSettings.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/TwoDimStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../Calculators/SimulatorNative.hpp"
#include <omp.h>
#include "../Factories/TechnologyMatrixFactorySingle.hpp"
#include "../Factories/InterventionMatrixFactorySingle.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
#include "../libs/libDescriptive.hpp"
#include <chrono>
#include "../Utilities/GraphUtilSingle.hpp"
using namespace std;

class ContributionAnalysisReport
{
public:
    AppSettings settings;
    CalculatorData *calculatorData;
    LCADB *lcadb;

    ContributionAnalysisReport(AppSettings _settings,
                               CalculatorData *_calculatorData, LCADB *_lcadb

                               )
        : calculatorData(_calculatorData), settings(_settings), lcadb(_lcadb)
    {
    }

    string printTreeNode(long key,
                         long parentNode,
                         long node,
                         long pp,
                         long p,
                         double q,
                         double total,
                         double percent,
                         double unc,
                         int impatCategoriesUnits)
    {

        std::string ss="";
        
ss+=to_string(key);
           ss+= (",");
           ss+= to_string(parentNode);
           ss+= (",");
           ss+= to_string(node);
          ss+= (",");
           ss+= to_string(pp);
           ss+= (",");
           ss+=to_string(p);
          ss+= (",");
           ss+= to_string(q);// quantity
           ss+= (",");
           ss+=to_string(percent);
           ss+= (",");
           ss+= to_string(total);
          ss+= (",");
           ss+= to_string(impatCategoriesUnits);
          ss+= (",");
           ss+=to_string(unc);
            //.append(",")
            //.append(if (LCADataBaseFactorySingle.impactcat_Map(key) != null) LCADataBaseFactorySingle.impactcat_Map(key).ICReferenceUnit else "Unit not found")
            //.append(if (settings.montecarlo) (","+unc) else "")
            ;

        return ss;
    }

    void printLCIA(GraphData *graph, SMatrix ASparse, double *lcia, Eigen::MatrixXd QBAinv, vector<ContributionTreeNode> tree)
    {

        /*cache local variables*/
        long Acols = (*calculatorData).lcaIndexes.ProcessesIndexLength();
        long Qrows = (*calculatorData).lcaIndexes.ImpactCategoryIndexLength();
        vector<long> ImpactCategoryIndex = (*calculatorData).lcaIndexes.ImpactCategoryIndex;
        unordered_map<long, long> ImpactCategoryIndexIndices = (*calculatorData).lcaIndexes.ImpactCategoryIndexIndices;
        unordered_map<long, long> ProcessesIndexIndices = (*calculatorData).lcaIndexes.ProcessesIndexIndices;
        int length = (*calculatorData).lcaIndexes.ImpactCategoryIndexLength();
        vector<ImpactStat> impactStats = (*calculatorData).impactStats;
        std::tr1::unordered_map<long, long> impatCategoriesUnits = (*lcadb).impatCategoriesUnits;

        exception ex;
        bool isexception = false;
        LCAException lcaex(-1, "");

        

#pragma omp parallel for shared(impactStats, impatCategoriesUnits)  num_threads(settings.Parallelism)
        for (int ii = 0; ii < length; ii++)
        {

            try
            {

                auto strtshares = std::chrono::high_resolution_clock::now();

                int cat = ImpactCategoryIndex[ii];

                string ss="";
                Eigen::MatrixXd cat_row = QBAinv.row(ImpactCategoryIndexIndices[cat]);

                for (auto &&node : tree)
                {
                   // auto strtupstrcalc = std::chrono::high_resolution_clock::now();

                    long p_i = ProcessesIndexIndices[node.ProcessId];
                    double scaled = ASparse.coeff(p_i, p_i) * (*graph).nodesInfoMap[node.ProcessId].scalar;
                    double merged = cat_row.coeff(p_i) * scaled * node.Share;
                    double mrged_unc = -1;

                    if (settings.montecarlo)
                    {
                        int catindex = ImpactCategoryIndexIndices[cat];

                        if (catindex < 0 || impactStats.size() == 0)
                        {
                            isexception = true;
                            throw LCAException(12, "(catindex < 0 || impactStats == 0) is false");
                        }
                        else
                        {
                            ImpactStat stat = impactStats[catindex];
                            mrged_unc = pow(scaled * node.Share, 2) * pow(stat.std, 2);
                        }
                    }

                    double mergedFlowQuantity = abs(merged);
                    double total = lcia[ImpactCategoryIndexIndices[cat]];


                // auto finishupstrcalc = std::chrono::high_resolution_clock::now();
                // auto elapsedupstrcalc = std::chrono::duration_cast<std::chrono::microseconds>(finishupstrcalc - strtupstrcalc);
                // std::cout << "upstrcalc/c++," << settings.ProjectId << "," << elapsedupstrcalc.count() << std::endl;

                    // ### akka Total Quantity of flow
                    //                    if (settings.montecarlo) {
                    //                    printTreeNode(cat,
                    //                            node.ParentNodeId,
                    //                            node.NodeId,
                    //                            node.ParentProcessId,
                    //                            node.ProcessId,
                    //                            mergedFlowQuantity,
                    //                            total,
                    //                            Math.abs(mergedFlowQuantity / total) * 100,
                    //
                    //                            merged_unc
                    //
                    //                            )
                    //                } else {

 //auto strtprnt = std::chrono::high_resolution_clock::now();
                    ss += printTreeNode(
                              cat,
                              node.ParentNodeId,
                              node.NodeId,
                              node.ParentProcessId,
                              node.ProcessId,
                              mergedFlowQuantity,
                              total,
                              abs(mergedFlowQuantity / total) * 100,
                              mrged_unc,
                              impatCategoriesUnits[cat]
                              );
                        ss += "\n";

                //         auto finishprnt = std::chrono::high_resolution_clock::now();
                // auto elapsedprnt = std::chrono::duration_cast<std::chrono::microseconds>(finishprnt - strtprnt);
                // std::cout << "prnt/c++," << settings.ProjectId << "," << elapsedprnt.count() << std::endl;


                    //}
                }

                // auto finishshares = std::chrono::high_resolution_clock::now();
                // auto elapsedshares = std::chrono::duration_cast<std::chrono::microseconds>(finishshares - strtshares);
                // std::cout << "shares/c++," << settings.ProjectId << "," << elapsedshares.count() << std::endl;

                //auto strtcsv = std::chrono::high_resolution_clock::now();

                string lciaContributionTreePath = settings.RootPath + "data/calculations/" + settings.CalculationId + "/lcia-tree/lcia-tree_" + to_string(cat) + "_" + settings.CalculationId + ".txt";
                FileUtils fileUtils;
                fileUtils.newfolder(settings.RootPath + "data/calculations/" + settings.CalculationId + "/lcia-tree/");
                fileUtils.writeTextToFile(ss, lciaContributionTreePath);

                // auto finishcsv = std::chrono::high_resolution_clock::now();
                // auto elapsedcsv = std::chrono::duration_cast<std::chrono::microseconds>(finishcsv - strtcsv);
                // std::cout << "csv/c++," << settings.ProjectId << "," << elapsedcsv.count() << std::endl;
            }
            catch (LCAException &e)
            {
                lcaex = e;
            }
            catch (exception &e2)
            {
                isexception = true;
                ex = e2;
            }
        }

        if (lcaex.code_ != -1)
        {
            throw lcaex;
        }

        if (isexception)
        {
            throw ex;
        }
    }
};

#endif /* ContributionAnalysisReport_hpp */
