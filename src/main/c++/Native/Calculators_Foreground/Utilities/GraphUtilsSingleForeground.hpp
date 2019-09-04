//
//  ResultsUtilsSingle.hpp
//  LCA.Kernel
//
//  Created by francois saab on 2/17/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef GraphUtilsSingleForeground_hpp
#define GraphUtilsSingleForeground_hpp

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

#include "../../Utilities/FileUtils.hpp"
#include "../../LCAModels/Exchange.hpp"
#include "../../DAL/ExchangeObj.hpp"
#include "../../LCAModels/ExchangeItem.hpp"
#include "../../LCAModels/ExchangeCell.hpp"
#include "../../Calculators/Models/AppSettings.hpp"
#include "../../LCAModels/ExchangeLink.hpp"
#include "../../LCAModels/Exchange.hpp"
#include "../../Graph/LCAEdgeCompact.hpp"
#include "../../Graph/NodeInfo.hpp"
#include "../../LCAModels/EdgeShare.hpp"
#include "../../DAL/LCADB.hpp"
#include "../../LCAModels/ContributionTReeNode.hpp"
#include "../../Graph/GraphData.h"
#include "../../Calculators/Models/CalculatorData.h"
#include "../../Uncertainties/EntitySampler.hpp"

class GraphUtilsSingleForeground
{
  public:
    LCADB *lcadb;
    GraphData *graph;
    AppSettings settings;

    GraphUtilsSingleForeground(LCADB *lcadb_, GraphData *graph_, AppSettings settings_)
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

    bool applyConnection(Exchange intProd, Exchange intCons, long level)
    {

        bool exist = (*graph).nodesInfoMap[intProd.processId].exist;

        /*Mark old nodes as existing*/
        if (!exist)
            (*graph).nodesInfoMap[intProd.processId].exist = true;

        EntitySampler<double> sampler;

        /*for root node*/
        if (intCons.processId == settings.RootProcessId)
        {
            if (intCons.isBackground)
            {
                (*graph).BackgroundLayerNodes.push_back(intCons.processId); // will contain duplicates
                (*graph).BackgroundLayerBarrierNodes.push_back(settings.RootProcessId);
            }
            else
            {
                (*graph).FrontLayerNodes.push_back(intCons.processId);
                //cout << "Root is front layer" << endl;
            }
        }

        if (!intProd.isBackground)
            (*graph).FrontLayerNodes.push_back(intProd.processId); // will contain duplicates
        else
            (*graph).BackgroundLayerNodes.push_back(intProd.processId); // will contain duplicates

        if (!intCons.isBackground && intProd.isBackground) //barrier
        {
            (*graph).FrontLayerBarrierNodes.push_back(intCons.processId);
            (*graph).BackgroundLayerBarrierNodes.push_back(intProd.processId);
            (*graph).barrier_demand[intCons.processId].push_back(std::make_pair(intProd.processId, intCons));
        }

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

        // if (!intProd.isBackground)
        // {
        (*graph).edgesStack[level].push_back(edgeTemp);
       // (*graph).demandEdges[intProd.processId].push_back((*lcadb).exchanges[intCons.exchangeId]);
       // (*graph).supplyEdges[intProd.processId].push_back((*lcadb).exchanges[intProd.exchangeId]);
        // }

        return exist;
    }

    int buildDirectedEdgesHorizontalSingleInPlace(vector<long> parentProcessVIDs, int level)
    {
        //cout<<"level "<< level<<", ";
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

                // if (el.producer_exchangeid == -1)
                // {
                //     throw LCAException(-1, "Intermediate flow found with no producer. The calculation cannot continue");
                // }

#pragma omp critical
                {
                    Exchange intProd = (*lcadb).exchanges[el.producer_exchangeid];

                    bool exist = applyConnection(intProd, intCons, level);

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

#endif /* GraphUtilsSingleForeground_hpp */
