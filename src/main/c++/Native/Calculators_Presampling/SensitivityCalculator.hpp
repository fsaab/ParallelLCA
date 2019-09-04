//
//  SensitivityCalculator.hpp
//  LCA.Kernel
//
//  Created by francois saab on 2/21/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef SensitivityCalculator_hpp
#define SensitivityCalculator_hpp

#include <stdio.h>
#include <vector>
#include <omp.h>
#include <chrono>
#include <string>

#include "../Calculators/Models/AppSettings.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/TwoDimVectorStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../libs/libEigen.hpp"
#include "../Uncertainties/EntitySampler.hpp"
#include "../Calculators/Models/IterationsResults.hpp"
#include "../Utilities/ParameterUtils.h"


#include "../Utilities/StringUtils.h"
#include "../Utilities/RankingUtils.hpp"
#include "../Calculators/Models/CorrelationResult.h"
#include "../Calculators/Models/ImpactStat.hpp"
#include "../DAL/LCADB.hpp"
#include "../Calculators/Models/SamplesRanks.hpp"
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
    vector<SamplesRanks> ranks_A;
    vector<SamplesRanks> ranks_B;

    //vector<vector<SamplesRanks>> allranks;

  public:
    SensitivityCalculator(AppSettings _settings,
                          CalculatorData *_calculatorData,

                          IterationsResults *_iterationsResults                                                           //, LCADB* _lcadb
                          ) : calculatorData(_calculatorData), settings(_settings), iterationsResults(_iterationsResults) //, lcadb(_lcadb)
    {
    }

    void init_presampled()
    {

        TwoDimVectorStore store;

        int Alength = (*calculatorData).A_unc_size; //ranks_A.reserve(Alength);
        int Blength = (*calculatorData).B_unc_size; //ranks_B.reserve(Blength);

        int num_processors = settings.Parallelism;

        /* A */
        int slotsize = Alength / num_processors;
        #pragma omp parallel for shared(Alength, num_processors, slotsize)
        for (int th = 0; th < num_processors; th++)
        {
            vector<SamplesRanks> ranks_A_th;
            int ii = th * slotsize;

            while (ii < (th * slotsize) + slotsize)
            {

                if (ii >= Alength)
                {
                    break;
                };

                if ((*calculatorData).A_unc_exchanges[ii].exch.isBackground)
                {
                    ii++;
                    continue;
                }

                long in_exchange = (*calculatorData).A_unc_exchanges[ii].exch.exchangeId;
                long procid = (*calculatorData).A_unc_exchanges[ii].exch.processId;

                double *in_data = store.getDataForAllIterations((*iterationsResults).store_A_ptr,
                                                                settings.montecarlo_iterations, ii);

                double *in_data_ranks = new double[settings.montecarlo_iterations];

                RankingUtils::spearmanrank(in_data, settings.montecarlo_iterations, in_data_ranks);

                double avg = Descriptive::all(in_data_ranks, settings.montecarlo_iterations)[0];

                SamplesRanks rank(in_exchange, procid, avg);

                for (int i = 0; i < settings.montecarlo_iterations; i++)
                {
                    rank.ranks.push_back(in_data_ranks[i]);
                }

                ranks_A_th.push_back(rank);

                delete[] in_data_ranks;
                in_data_ranks = nullptr;

                delete[] in_data;
                in_data = nullptr;

                ii++;
            }

#pragma omp critical
            {

                ranks_A.insert(ranks_A.end(), ranks_A_th.begin(), ranks_A_th.end());
            }
        }

        /* B */
        slotsize = Blength / num_processors;
#pragma omp parallel for shared(Blength, num_processors, slotsize)
        for (int th = 0; th < num_processors; th++)
        {
            vector<SamplesRanks> ranks_B_th;
            int ii = th * slotsize;

            while (ii < (th * slotsize) + slotsize)
            {

                if (ii >= Blength)
                {
                    break;
                };

                if ((*calculatorData).B_unc_exchanges[ii].exch.isBackground)
                {
                    ii++;
                    continue;
                }

                long in_exchange = (*calculatorData).B_unc_exchanges[ii].exch.exchangeId;
                long procid = (*calculatorData).B_unc_exchanges[ii].exch.processId;

                double *in_data = store.getDataForAllIterations((*iterationsResults).store_B_ptr,
                                                                settings.montecarlo_iterations, ii);

                double *in_data_ranks = new double[settings.montecarlo_iterations];

                RankingUtils::spearmanrank(in_data, settings.montecarlo_iterations, in_data_ranks);

                double avg = Descriptive::all(in_data_ranks, settings.montecarlo_iterations)[0];

                SamplesRanks rank(in_exchange, procid, avg);

                for (int i = 0; i < settings.montecarlo_iterations; i++)
                {
                    rank.ranks.push_back(in_data_ranks[i]);
                }

                ranks_B_th.push_back(rank);

                delete[] in_data_ranks;
                in_data_ranks = nullptr;

                delete[] in_data;
                in_data = nullptr;

                ii++;
            }

#pragma omp critical
            {

                ranks_B.insert(ranks_B.end(), ranks_B_th.begin(), ranks_B_th.end());
            }
        }

        //allranks.resize(num_processors);
        #pragma omp parallel for
        for (int th = 0; th < num_processors; th++)
        {

            std::stringstream ss;

            ss << settings.RootPath << "data/Ranks/" << settings.montecarlo_iterations << "/ARanks/" << th + 1 << ".txt";

            FileUtils filesUtils;

            vector<string> ranks = filesUtils.readTextFromFile(ss.str());

            for (auto &&rankline : ranks)
            {

                SamplesRanks srank;

                vector<string> exch_avg_ranks = split(rankline, ",");

                srank.EntityId = stol(exch_avg_ranks[0]);
                srank.Average = std::stod(exch_avg_ranks[1]);
                srank.ProcessId = stol(exch_avg_ranks[2]);

                for (int i = 3; i < exch_avg_ranks.size(); i++)
                {
                    if (exch_avg_ranks[i] != "")
                        srank.ranks.push_back(stol(exch_avg_ranks[i]));
                }

#pragma omp critical
                {

                    ranks_A.push_back(srank);
                }
            }
        }

#pragma omp parallel for
        for (int th = 0; th < num_processors; th++)
        {

            std::stringstream ss;

            ss << settings.RootPath << "data/Ranks/" << settings.montecarlo_iterations << "/BRanks/" << th + 1 << ".txt";

            FileUtils filesUtils;

            vector<string> ranks = filesUtils.readTextFromFile(ss.str());

            for (auto &&rankline : ranks)
            {

                SamplesRanks srank;

                vector<string> exch_avg_ranks = split(rankline, ",");

                srank.EntityId = stol(exch_avg_ranks[0]);
                srank.Average = std::stod(exch_avg_ranks[1]);
                srank.ProcessId = stol(exch_avg_ranks[2]);

                for (int i = 3; i < exch_avg_ranks.size(); i++)
                {
                    if (exch_avg_ranks[i] != "")
                        srank.ranks.push_back(stol(exch_avg_ranks[i]));
                }

#pragma omp critical
                {

                    ranks_B.push_back(srank);
                }
            }
        }
    }


    std::vector<std::string> split(std::string input, std::string sep)
    {

        vector<string> strs;
        boost::split(strs, input, boost::is_any_of(sep));

        return strs;
    }

    

    void correlateImpactCategoryToInputsFormulae_PreSampled(int output, string path)
    {

        double sumSquare = 0;

        TwoDimVectorStore store;

        double *out_data = store.getDataForAllIterations(
            (*iterationsResults).store_lcia_ptr,
            settings.montecarlo_iterations,
            (*calculatorData).lcaIndexes.ImpactCategoryIndexIndices[output]);

        double *out_data_ranks = new double[settings.montecarlo_iterations];

        double out_avg = Descriptive::all(out_data_ranks, settings.montecarlo_iterations)[0];

        RankingUtils::spearmanrank(out_data, settings.montecarlo_iterations, out_data_ranks);

        delete[] out_data;
        out_data = nullptr;

        vector<CorrelationResult> corrs;

        int Alength = (*calculatorData).A_unc_size;
        int Blength = (*calculatorData).B_unc_size;
        double sumA = 0;

        /* A */

        int num_processors = settings.Parallelism;
        int slotsize = Alength / num_processors;
        corrs.reserve(Alength + Blength);

        if (ranks_A.size() > 0)
        {
#pragma omp parallel for shared(sumA, corrs, Alength, num_processors, slotsize, output)
            for (int th = 0; th < num_processors; th++)
            {

                vector<CorrelationResult> corrs_A_th;
                int ii = th * slotsize;

                while (ii < (th * slotsize) + slotsize)
                {

                    if (ii >= Alength)
                    {
                        break;
                    };

                    long in_exchange = ranks_A[ii].EntityId;
                    char in_type = 'I';

                    SamplesRanks sRank = ranks_A[ii];

                    double cor = RankingUtils::spearman_corr_formula(&sRank.ranks[0],
                                                                     sRank.Average, out_data_ranks, out_avg, settings.montecarlo_iterations);
                    //                #pragma omp critical
                    //            {
                    //                if(!printed){
                    //                for(int ix=0;ix<settings.montecarlo_iterations;ix++){
                    //                  cout<<sRank.ranks[ix]<<",";
                    //                }
                    //
                    //                }
                    //                if(!printed){ cout <<"cor "<<cor<<endl; printed=true;}
                    //            }
                    if (!isnan(cor) && !isinf(cor))
                        corrs_A_th.push_back(CorrelationResult(in_exchange, in_type, output, cor));

                    ii++;
                }

#pragma omp critical
                {

                    for (int sumi = 0; sumi < corrs_A_th.size(); sumi++)
                    {
                        double cor = corrs_A_th[sumi].cor;

                        if (!isnan(cor) && !isinf(cor))
                        {
                            sumA += pow(cor, 2);
                        }
                    }

                    corrs.insert(corrs.end(), corrs_A_th.begin(), corrs_A_th.end());
                }
            }
        }

        /* B */

        slotsize = Blength / num_processors;

        double sumB = 0;
        if (ranks_B.size() > 0)
        {
#pragma omp parallel for shared(sumB, corrs, Blength, num_processors, slotsize, output)
            for (int th = 0; th < num_processors; th++)
            {
                vector<CorrelationResult> corrs_B_th;

                for (int ii = th * slotsize; ii < (th * slotsize) + slotsize; ii++)
                {

                    if (ii >= Blength)
                    {
                        break;
                    };

                    long in_exchange = ranks_B[ii].EntityId;
                    char in_type = 'E';

                    SamplesRanks sRank = ranks_B[ii];

                    double cor = RankingUtils::spearman_corr_formula(&sRank.ranks[0],
                                                                     sRank.Average, out_data_ranks, out_avg, settings.montecarlo_iterations);

                    if (!isnan(cor) && !isinf(cor))
                        corrs_B_th.push_back(CorrelationResult(in_exchange, in_type, output, cor));
                }

#pragma omp critical
                {
                    for (int sumi = 0; sumi < corrs_B_th.size(); sumi++)
                    {

                        double cor = corrs_B_th[sumi].cor;

                        if (!isnan(cor) && !isinf(cor))
                        {
                            sumB += pow(cor, 2);
                        }
                    }
                    corrs.insert(corrs.end(), corrs_B_th.begin(), corrs_B_th.end());
                }
            }
        }

        //
        //         #pragma omp parallel for
        //        for (int th = 0; th < num_processors; th++) {
        //            vector<CorrelationResult> corrs_th;
        //
        //            for (auto && srank : allranks[th]) {
        //
        //
        //                long in_exchange = srank.EntityId;
        //                char in_type = 'I';
        //
        //                double cor = RankingUtils::spearman_corr_formula(&srank.ranks[0],
        //                        srank.Average, out_data_ranks, out_avg, settings.montecarlo_iterations);
        //                if (!isnan(cor) && !isinf(cor))
        //                    corrs_th.push_back(CorrelationResult(in_exchange, in_type, output, cor));
        //
        //            }
        //
        //#pragma omp critical
        //            {
        //
        //                for (int sumi = 0; sumi < corrs_th.size(); sumi++) {
        //                    double cor = corrs_th[sumi].cor;
        //
        //                    if (!isnan(cor) && !isinf(cor)) {
        //                        sumA += pow(cor, 2);
        //                    }
        //                }
        //
        //                corrs.insert(corrs.end(), corrs_th.begin(), corrs_th.end());
        //            }
        //        }
        //

        sumSquare = sumSquare + sumA;
        sumSquare = sumSquare + sumB;

        delete[] out_data_ranks;
        out_data_ranks = nullptr;

        std::sort(corrs.begin(), corrs.end(), more_than_key());

        double take = corrs.size();

        if (take > 100)
        {
            take = 100;
        }

        if (take != 0)
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


    void run_preRanked()
    {

        FileUtils fileUtils;

        std::stringstream ss_sensitivityPath;

        ss_sensitivityPath << settings.RootPath << "data/calculations/" << settings.CalculationId << "/SensitivityPerImpactCat/";

        fileUtils.newfolder(ss_sensitivityPath.str());

        int cat_length = (*calculatorData).lcaIndexes.ImpactCategoryIndexLength();

        (*calculatorData).LoadUncertainCells();

        cout << "starting pre-ranking ...." << endl;

        init_presampled();

        cout << ranks_A.size() << " " << ranks_B.size() << endl;

        cout << "starting correlations ...." << endl;

        //#pragma omp parallel for
        for (int i = 0; i < cat_length; i++)
        {
            int c = (*calculatorData).lcaIndexes.ImpactCategoryIndex[i];

            std::stringstream ss;

            ss << settings.RootPath << "data/calculations/" << settings.CalculationId << "/SensitivityPerImpactCat/"
               << "SENS_lcia_" << settings.SystemId << "_" << settings.CalculationId << "_" << std::to_string(c) << ".txt";

            string path = ss.str();

            //            clock_t t;
            //            t = clock();

            auto start = std::chrono::high_resolution_clock::now();
            correlateImpactCategoryToInputsFormulae_PreSampled(c, path.c_str());
            auto finish = std::chrono::high_resolution_clock::now();
            //            t = clock() - t;
            //            float elapsed= ((float)t)/CLOCKS_PER_SEC;

            std::chrono::duration<double> elapsed = finish - start;
            std::cout << "Spearman," << settings.ProjectId << "," << elapsed.count() << std::endl;

            //string str;
            //StringUtils::join(fileUtils.readTextFromFile(path), "\n", str);
            //*file << str;

            //            std::stringstream ss_out;
            //            ss_out<< settings.RootPath<<
            //              "data/calculations/"<<
            //              settings.CalculationId<<
            //              "/SensitivityPerImpactCat/"<<
            //              "SENS_lcia_lcia_"<<
            //              settings.SystemId<<
            //              "_"<<
            //              settings.CalculationId<<
            //              "_"<<
            //              std::to_string(c)<<
            //              ".txt";
            //
            //             string path_out=ss_out.str();
            //
            //            correlateImpactCategoryToOutputs(c,path_out);
            //
            //
            //            string str_out;
            //            StringUtils::join(fileUtils.readTextFromFile( path_out),"\n",str_out);
            //            file_out<<str_out;
        }

        //file->close();
        //delete file;
        //file_out.close();
    }

   
};

#endif /* SensitivityCalculator_hpp */
