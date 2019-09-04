//
//  ResultsUtilsSingle.hpp
//  LCA.Kernel
//
//  Created by francois saab on 2/17/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef LinearGraphUtils_hpp
#define LinearGraphUtils_hpp

#include <stdio.h>
#include <vector>
#include "../Utilities/FileUtils.hpp"
#include "../LCAModels/Exchange.hpp"
#include "../DAL/ExchangeObj.hpp"

#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/ExchangeCell.hpp"
#include <chrono>
#include <sstream>
#include <time.h>
#include <iostream>

#include "../Calculators/Models/AppSettings.hpp"
#include <vector>
#include "../LCAModels/ExchangeLink.hpp"
#include "../LCAModels/Exchange.hpp"

#include "../Graph/LCAEdgeCompact.hpp"
#include <omp.h>
#include <tr1/unordered_map>
#include "../LCAModels/EdgeShare.hpp"
#include "../DAL/LCADB.hpp"
#include "../Utilities/LCAUtils.hpp"
#include <cmath>
#include <queue>
#include "../LCAModels/ContributionTReeNode.hpp"
#include "../Graph/GraphData.h"
#include "../Calculators/Models/CalculatorData.h"
#include <chrono>  // for high_resolution_clock
#include "../Uncertainties/EntitySampler.hpp"

class LinearGraphUtils {
private:
    map<long, vector<EdgeShare> > edgesShareMap;
public:

    LCADB* lcadb;
    GraphData* graph;
    AppSettings settings;

    LinearGraphUtils(LCADB* lcadb_, GraphData* graph_, AppSettings settings_) {

        lcadb = lcadb_;
        graph = graph_;
        settings = settings_;
    }

    EdgeShare transformEdgesWithContribtutionShare(LCAEdgeCompact ed, CalculatorData* calculatorData) {

        Exchange in_exchange = (*lcadb).exchanges[ed.InputExchangeId];

        double inval = in_exchange.amount *
                (*graph).nodesInfoMap[ed.ProcessSrcId].scalar *
                in_exchange.conversionFactor; // parent/in

        LCAUtils lcautils(lcadb, calculatorData);

        vector<long> v_procs;

        v_procs.push_back(ed.ProcessDestId);


        vector<ExchangeItem> v_outexch = lcautils.getOutputIntermediateFlowsArray(v_procs);


        Exchange outFlow;

        for (auto && exch : v_outexch) {

            if (exch.exch.exchangeId == ed.ProducerExchangeId) {
                outFlow = exch.exch;
                break;
            }
        }


        double outval = outFlow.amount *
                (*graph).nodesInfoMap[ed.ProcessDestId].scalar *
                outFlow.conversionFactor; //child/out


        double share;

        if (outval != 0 && inval != 0) {
            /*BigDecimal( */
            share = abs(inval / outval);
            /*).setScale(2,BigDecimal.RoundingMode.FLOOR).toDouble*/
        } else {
            share = 0;
        }


        //("(ed.src, ed.dest, share)" +(ed.ProcessSrcId, ed.ProcessDestId, share, inval, outval))

        return EdgeShare(ed.ProcessSrcId, ed.ProcessDestId, share);
    }

    map<long, vector<EdgeShare> > initEdgesShareMap(CalculatorData* calculatorData) {

        //fn_printTreeRecursive(forwardEdges)

        map<std::pair<long, long>, double> edgesShareTmpMap;

        for (auto && edgeentry : (*graph).edgesListCompact) {

            LCAEdgeCompact edge=edgeentry.second;

            EdgeShare trans_edhshare = transformEdgesWithContribtutionShare(edge, calculatorData);

            edgesShareTmpMap[std::pair<long, long>(trans_edhshare.src, trans_edhshare.dest)] = edgesShareTmpMap[std::pair<long, long>(trans_edhshare.src, trans_edhshare.dest)] + trans_edhshare.share;
        }

        map<long, vector<EdgeShare> > EdgesShareMap_;

        for (map<std::pair<long, long>, double> ::iterator it = edgesShareTmpMap.begin(); it != edgesShareTmpMap.end(); ++it) {

            EdgesShareMap_[(*it).first.first].push_back(EdgeShare((*it).first.first, (*it).first.second, (*it).second));
        }

        return EdgesShareMap_;

    }

    vector<EdgeShare> getChildsProcessesWithoutLoops(long ProcessId) {

        vector<EdgeShare> childsProcesses = edgesShareMap[ProcessId];

        vector<EdgeShare> childsProcesses_noloops;

        for (auto && edgeshare : childsProcesses) {

            if (edgeshare.dest != ProcessId) {
                childsProcesses_noloops.push_back(edgeshare);
            }
        }

        return childsProcesses_noloops;

    }

    vector<ContributionTreeNode> createChildrenNodes(ContributionTreeNode parentNode, long length) {

        vector<EdgeShare> childsWithoutLoops = getChildsProcessesWithoutLoops(parentNode.ProcessId);

        long i = length;

        vector<ContributionTreeNode> childs_without_loops_nodes;

        for (auto && child : childsWithoutLoops) {

            i = i + 1;
            childs_without_loops_nodes.push_back(
                    ContributionTreeNode(i, //Node Id
                    parentNode.NodeId, //ParentNode
                    parentNode.ProcessId, // parent process id
                    child.dest, //child process
                    child.share * parentNode.Share //share
                    )
                    );
        }

        return childs_without_loops_nodes;

    }

    vector<ContributionTreeNode> buildLinearTree(long root, CalculatorData * calculatorData) {

        edgesShareMap = initEdgesShareMap(calculatorData);

        std::queue<ContributionTreeNode, std::deque < ContributionTreeNode>> tmpNodesStack;

        map<long, bool> handled;

        vector<ContributionTreeNode> nodesList;

        vector<ContributionTreeNode> leafNodes;

        ///var edgesList=List[(Long,Long)]()

        ContributionTreeNode rootNode(
                nodesList.size() + 1, //Node Id
                -1, //ParentNode
                -1, //proc id
                root, //process
                1 //share
                );

        nodesList.push_back(rootNode);

        tmpNodesStack.push(rootNode);

        handled[root] = true;

        while (!tmpNodesStack.empty()) {

            //cout<< "tmpNodesStack size"<<tmpNodesStack.size()<<endl;

            ContributionTreeNode current = tmpNodesStack.front();
            tmpNodesStack.pop();
            vector<ContributionTreeNode> childs = createChildrenNodes(current, nodesList.size());

            //cout<< "childs size"<< childs.size()<<endl;


            if (childs.empty()) {

                leafNodes.push_back(current);
            } else {


                for (auto && ch : childs) {


                    if (!handled[ch.ProcessId]) {

                        nodesList.push_back(ch);

                        tmpNodesStack.push(ch);

                        handled[ch.ProcessId] = true;


                    } else {

                        nodesList.push_back(ContributionTreeNode(ch.NodeId, ch.ParentNodeId, ch.ParentProcessId, ch.ProcessId, ch.Share));
                    }

                }


            }



        }

        return nodesList; //, leafNodes)

    }

};

#endif /* LinearGraphUtils_hpp */
