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
#ifndef GraphUtilsSingle_hpp
#define GraphUtilsSingle_hpp

#include <stdio.h>
#include <vector>
#include <chrono>
#include <sstream>
#include <time.h>
#include <iostream>
#include <vector>
#include <omp.h>
#include <tr1/unordered_map>
#include <cmath>
#include <queue>
#include <chrono> // for high_resolution_clock

#include "FileUtils.hpp"
#include "../LCAModels/Exchange.hpp"
#include "../DAL/ExchangeObj.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/ExchangeCell.hpp"
#include "../Calculators/Models/AppSettings.hpp"
#include "../LCAModels/ExchangeLink.hpp"
#include "../LCAModels/Exchange.hpp"
#include "../Graph/LCAEdgeCompact.hpp"
#include "../Graph/NodeInfo.hpp"
#include "../LCAModels/EdgeShare.hpp"
#include "../DAL/LCADB.hpp"
#include "LCAUtils.hpp"
#include "../LCAModels/ContributionTReeNode.hpp"
#include "../Graph/GraphData.h"
#include "../Calculators/Models/CalculatorData.h"
#include "../Uncertainties/EntitySampler.hpp"

class GraphUtilsSingle
{
  public:
    LCADB *lcadb;
    GraphData *graph;
    AppSettings settings;

    GraphUtilsSingle(LCADB *lcadb_, GraphData *graph_, AppSettings settings_)
    {

        lcadb = lcadb_;
        graph = graph_;
        settings = settings_;
    }

    bool selectProducerCompact(ExchangesLink el)
    {

        Exchange intProd = (*lcadb).exchanges[el.producer_exchangeid];
        Exchange intCons = (*lcadb).exchanges[el.input_exchangeid];

        return (
            (intCons.defaultProviderId > 0 &&
             intCons.defaultProviderId == intProd.processId) ||
            (intCons.defaultProviderId == 0 &&
             intProd.processType == "LCI_RESULT"));
    }

    ExchangesLink find_producer(long inputExchangeId, vector<Exchange> producers)
    {

        int jj = 0;

        ExchangesLink producer(0, 0);

        long size = producers.size();
        while (jj < size && producer.input_exchangeid == 0)
        {
            ExchangesLink prod = ExchangesLink(inputExchangeId, producers[jj].exchangeId);

            if (selectProducerCompact(prod))
            {
                producer = prod;
                break;
            }

            jj = jj + 1;
        }

        return producer;
    }

    static bool isCountry(string loc)
    {

        vector<string> notCountry = {"RoW", "GLO", "RER"};
        bool exist = false;
        for (auto &&cnt : notCountry)
        {
            if (cnt == loc)
                exist = true;
        }

        return !exist;
    }

    ExchangesLink makeConnection(Exchange inputExchange)
    {
        vector<Exchange> producers;
        vector<long> v = (*lcadb).producersExchangesByFlow[inputExchange.flowId];
        ExchangesLink found_producer(0, 0);

        for (auto &&pp_id : v)
        {
            Exchange intProd = (*lcadb).exchanges[pp_id];

            if ((inputExchange.defaultProviderId > 0 &&
                 inputExchange.defaultProviderId == intProd.processId) ||
                (inputExchange.defaultProviderId == 0 &&
                 intProd.processType == "LCI_RESULT"))
            {
                return ExchangesLink(inputExchange.exchangeId, intProd.exchangeId);
            }

            producers.push_back((*lcadb).exchanges[pp_id]);
        }

        for (auto &&exch : producers)
        {
            if (GraphUtilsSingle::isCountry((*lcadb).processesLocationCodes[exch.processId]))
                return ExchangesLink(inputExchange.exchangeId, exch.exchangeId);
        }

        if (producers.size() > 0)
            return ExchangesLink(inputExchange.exchangeId, producers[0].exchangeId);

        return ExchangesLink(inputExchange.exchangeId, -1);
    }

    bool applyConnection(Exchange intProd, Exchange intCons)
    {
        EntitySampler<double> sampler;
        
        LCAEdgeCompact edgeTemp = LCAEdgeCompact(intCons.exchangeId,
                                                 intProd.exchangeId,
                                                 intCons.processId,
                                                 intProd.processId,
                                                 sampler.getCellValue(intCons),
                                                 sampler.getCellValue(intProd),
                                                 &((*graph).nodesInfoMap[intCons.processId]),
                                                 &((*graph).nodesInfoMap[intProd.processId]),
                                                 intProd.flowId);

        (*graph).edgesListCompact[intCons.exchangeId] = edgeTemp;

        bool exist = (*graph).nodesInfoMap[intProd.processId].exist;
        /*Mark old nodes as existing*/
        if (!exist)
            (*graph).nodesInfoMap[intProd.processId].exist = true;

        return exist;
    }

    int buildDirectedEdgesHorizontalSingleInPlace(vector<long> parentProcessVIDs, int level)
    {

     

        (*graph).stack[level] = parentProcessVIDs;

        /*Mark new nodes as existing*/
        for (auto &&proc : parentProcessVIDs)
        {
            (*graph).nodesInfoMap[proc].exist = true;
        }

        vector<long> inputInterFlows;
        vector<long> newprocs;

        long inputInterFlows_size = 0;

        long parentProcessVIDs_size = parentProcessVIDs.size();

#pragma omp parallel for shared(newprocs, inputInterFlows, inputInterFlows_size)
        for (int pi = 0; pi < parentProcessVIDs_size; pi++)
        {
            // cout<< "proc"<<proc<<endl;
            vector<long> input_exchanges_p = (*lcadb).inputExchangesByProcess[parentProcessVIDs[pi]];

            //cout<<"inputflows: "<<(*lcadb).inputExchangesByProcess[proc].size()<<endl;
            for (auto &&inputExchangeId : input_exchanges_p)
            {

                Exchange intCons = (*lcadb).exchanges[inputExchangeId];

                //auto strt = std::chrono::high_resolution_clock::now();
                ExchangesLink el = makeConnection(intCons);

                if (el.producer_exchangeid == -1)
                {
                    cout<<"Intermediate flow found with no producer"<<endl;
                    throw LCAException(-1, "Intermediate flow found with no producer. \n The calculation cannot continue");
                }

               #pragma omp critical
                {
                    Exchange intProd = (*lcadb).exchanges[el.producer_exchangeid];

                    bool exist = applyConnection(intProd, intCons);

                    if (!exist)
                    {
                        newprocs.push_back(intProd.processId);
                    }

                    inputInterFlows.push_back(inputExchangeId);
                    inputInterFlows_size++;
                }
            }
        }

        if (inputInterFlows_size == 0)
        {
            return 0;
        }

        //    auto finish = std::chrono::high_resolution_clock::now();
        //    std::chrono::duration<double> elapsed = finish - strt;
        //    std::cout << "layer," << elapsed.count() << std::endl;

        return buildDirectedEdgesHorizontalSingleInPlace(newprocs, level + 1);
    }
};

#endif /* GraphUtilsSingle_hpp */
