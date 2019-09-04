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

#ifndef SensitivityCalculator_hpp
#define SensitivityCalculator_hpp

#include <stdio.h>
#include <stdio.h>
#include <vector>
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "Models/AppSettings.hpp"
#include "Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "Models/TwoDimStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../Utilities/StringUtils.h"
#include "Models/IterationsResults.hpp"
#include "../Utilities/RankingUtils.hpp"
#include "Models/CorrelationResult.h"
#include "Models/ImpactStat.hpp"
#include "../DAL/LCADB.hpp"
#include <omp.h>
#include <chrono>
#include <string>
#include "Models/SamplesRanks.hpp"
#include "../libs/libDescriptive.hpp"

struct more_than_key
{

    inline bool operator()(const CorrelationResult &item1, const CorrelationResult &item2)
    {

        if (isnan(item1.cor))
            return false;
        if (isnan(item2.cor))
            return true;
        return (pow(item1.cor, 2) > pow(item2.cor, 2));
    }
};

class SensitivityCalculator
{
    AppSettings settings;
    CalculatorData *calculatorData;
    // LCADB * lcadb;
    IterationsResults *iterationsResults;

    //vector<vector<SamplesRanks>> allranks;

  public:
    SensitivityCalculator(AppSettings _settings,
                          CalculatorData *_calculatorData,

                          IterationsResults *_iterationsResults                                                           //, LCADB* _lcadb
                          ) : calculatorData(_calculatorData), settings(_settings), iterationsResults(_iterationsResults) //, lcadb(_lcadb)
    {
    }

    void pre_rank_entity(long length,
                         vector<vector<double>> &store,
                         vector<ExchangeItem> &items,
                         vector<SamplesRanks> &ranks)
    {

        int num_processors = settings.Parallelism;

        int slotsize = length / num_processors;
#pragma omp parallel for shared(length, num_processors, slotsize)
        for (int th = 0; th < num_processors; th++)
        {
            vector<SamplesRanks> ranks_th;
            int ii = th * slotsize;

            while (ii < (th * slotsize) + slotsize)
            {

                if (ii >= length)
                {
                    break;
                };

                Exchange exch = items[ii].exch;

                long in_exchange = exch.exchangeId;
                long procid = exch.processId;

                double *in_data = TwoDimVectorStore::getDataForAllIterations(store,
                                                                             settings.montecarlo_iterations, ii);
                double *in_data_ranks = new double[settings.montecarlo_iterations];

                RankingUtils::spearmanrank(in_data, settings.montecarlo_iterations, in_data_ranks);

                double avg = Descriptive::all(in_data_ranks, settings.montecarlo_iterations)[0];

                SamplesRanks rank(in_exchange, procid, avg);

                for (int i = 0; i < settings.montecarlo_iterations; i++)
                {
                    rank.ranks.push_back(in_data_ranks[i]);
                }

                ranks_th.push_back(rank);

                delete[] in_data_ranks;
                in_data_ranks = nullptr;

                delete[] in_data;
                in_data = nullptr;

                ii++;
            }

#pragma omp critical
            {
                ranks.insert(ranks.end(), ranks_th.begin(), ranks_th.end());
            }
        }
    }

    std::vector<std::string> split(std::string input, std::string sep)
    {

        vector<string> strs;
        boost::split(strs, input, boost::is_any_of(sep));

        return strs;
    }

    void correlateOutputToEntityRows(long length, vector<CorrelationResult> &corrs, double &totalSumSquare,
                                     double *out_data_ranks, double out_avg, int output, char in_type, vector<SamplesRanks> &ranks)
    {

        double entitySum = 0;

        int num_processors = settings.Parallelism;
        int slotsize = length / num_processors;

        //#pragma omp parallel for shared(sum,corrs,length,num_processors,slotsize,output)
        for (int th = 0; th < num_processors; th++)
        {

            vector<CorrelationResult> corrs_th;
            int ii = th * slotsize;

            while (ii < (th * slotsize) + slotsize)
            {

                if (ii >= length)
                {
                    break;
                };

                long in_exchange = ranks[ii].EntityId;

                SamplesRanks sRank = ranks[ii];

                double cor = RankingUtils::spearman_corr_formula(&sRank.ranks[0],
                                                                 sRank.Average,
                                                                 out_data_ranks,
                                                                 out_avg,
                                                                 settings.montecarlo_iterations);

                if (!isnan(cor) && !isinf(cor))
                    corrs_th.push_back(CorrelationResult(in_exchange, in_type, output, cor));

                ii++;
            }

            //#pragma omp critical
            //{

            for (int sumi = 0; sumi < corrs_th.size(); sumi++)
            {
                double cor = corrs_th[sumi].cor;

                if (!isnan(cor) && !isinf(cor))
                {
                    entitySum += pow(cor, 2);
                }
            }

            corrs.insert(corrs.end(), corrs_th.begin(), corrs_th.end());
            //}
        }

        totalSumSquare = totalSumSquare + entitySum;
    }

    void correlateImpactCategoryToInputsFormulae(int output, string path,
                                                 vector<SamplesRanks> &ranks_A,
                                                 vector<SamplesRanks> &ranks_B,
                                                 long Alength, long Blength, vector<vector<double>> &store_out,
                                                 vector<CorrelationResult> &corrs, double &sumSquare)
    {

        TwoDimVectorStore store;

        double *out_data = store.getDataForAllIterations(store_out,
        settings.montecarlo_iterations,
         (*calculatorData).lcaIndexes.ImpactCategoryIndexIndices[output]);

        double *out_data_ranks = new double[settings.montecarlo_iterations];

        double out_avg = Descriptive::all(out_data_ranks, settings.montecarlo_iterations)[0];

        RankingUtils::spearmanrank(out_data, settings.montecarlo_iterations, out_data_ranks);

        delete[] out_data;
        out_data = nullptr;

        correlateOutputToEntityRows(Alength, corrs, sumSquare, out_data_ranks, out_avg, output, 'I', ranks_A);

        correlateOutputToEntityRows(Blength, corrs, sumSquare, out_data_ranks, out_avg, output, 'E', ranks_B);

        delete[] out_data_ranks;
        out_data_ranks = nullptr;
    }

    void run()
    {
        FileUtils fileUtils;
        std::stringstream ss_sensitivityPath;
        ss_sensitivityPath << settings.RootPath << "data/calculations/" << settings.CalculationId << "/SensitivityPerImpactCat/";
        fileUtils.newfolder(ss_sensitivityPath.str());

        auto start_rank = std::chrono::high_resolution_clock::now();

        int cat_length = (*calculatorData).lcaIndexes.ImpactCategoryIndexLength();


        vector<SamplesRanks> ranks_A;

        pre_rank_entity((*calculatorData).A_unc_size,      //length of uncertain cells
                        (*iterationsResults).store_A_ptr,  //samples 2D matrix
                        (*calculatorData).A_unc_exchanges, // vector of uncetain items
                        ranks_A                            // destination of ranks
        );

        vector<SamplesRanks> ranks_B;

        pre_rank_entity((*calculatorData).B_unc_size,
                        (*iterationsResults).store_B_ptr,
                        (*calculatorData).B_unc_exchanges,
                        ranks_B);

                        
        auto finish_rank = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_rank = finish_rank - start_rank;
        std::cout << "Ranking," << settings.ProjectId << ","<<ranks_A.size()+ranks_B.size()<<","<< elapsed_rank.count() << std::endl;
                        
        auto start_corr = std::chrono::high_resolution_clock::now();

        int Alength = (*calculatorData).A_unc_size;
        int Blength = (*calculatorData).B_unc_size;

        #pragma omp parallel for 
        for (int i = 0; i < cat_length; i++)
        {
            int c = (*calculatorData).lcaIndexes.ImpactCategoryIndex[i];
            string path = settings.RootPath + "data/calculations/" + settings.CalculationId + "/SensitivityPerImpactCat/" + "SENS_lcia_" + settings.SystemId + "_" + settings.CalculationId + "_" + std::to_string(c) + ".txt";

            vector<CorrelationResult> corrs;
            corrs.reserve(Alength + Blength);

            double sumSquare = 0;

            correlateImpactCategoryToInputsFormulae(c, path, ranks_A, ranks_B, Alength, Blength,
                                                    (*iterationsResults).store_lcia_ptr, corrs,sumSquare);

            std::sort(corrs.begin(), corrs.end(), more_than_key());

            double take = (corrs.size() > 100 ? 100 : corrs.size());

            if (take != 0) // no uncertain elements to correlate
            {
                std::stringstream ss;

                for (int i = 0; i < take; i++)
                {
                    ss << corrs[i].in_type << "," << corrs[i].in_id << "," << corrs[i].out_id << ","
                       << pow(corrs[i].cor, 2) << "," << pow(corrs[i].cor, 2) / sumSquare << "," << sumSquare << endl;
                }

                FileUtils fileUtils;
                fileUtils.writeTextToFile(ss.str(), path);
            }
        }

        auto finish_corr = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_corr = finish_corr - start_corr;
        std::cout << "Pearsons_openmp," << settings.ProjectId << ","<<(ranks_A.size()+ranks_B.size())*cat_length <<","<< elapsed_corr.count() << std::endl;
    }
};

#endif /* SensitivityCalculator_hpp */
