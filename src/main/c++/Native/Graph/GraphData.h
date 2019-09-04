//
//  CalculatorData.h
//  LCA.Kernel
//
//  Created by francois saab on 2/18/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef GraphData_h
#define GraphData_h
#include <stdio.h>
#include <string>
#include <vector>
#include <tr1/unordered_map>

#include <boost/mpi.hpp>
#include <unordered_map>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/string.hpp>

#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../Calculators/Models/AppSettings.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../Calculators/Models/ImpactStat.hpp"
#include "../Graph/LCAEdgeCompact.hpp"
#include "../Graph/NodeInfo.hpp"

using namespace std;

class GraphData
{

  public:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar &edgesListCompact; /*consumer exchange id,edge*/
        ar &nodesInfoMap;

        /*Stack based*/
        ar &stack;
        ar &edgesStack; /*<layerid,edge>*/

        ar &FrontLayerNodes;
        ar &FrontLayerBarrierNodes;
        ar &BackgroundLayerNodes;
        ar &BackgroundLayerBarrierNodes;

        ar &barrier_demand;
    }

    map<long, LCAEdgeCompact> edgesListCompact; /*consumer exchange id,edge*/
    std::tr1::unordered_map<long, NodeInfo> nodesInfoMap;

    /*Stack based*/
    unordered_map<int, vector<long>> stack;
    unordered_map<long, vector<LCAEdgeCompact>> edgesStack; /*<layerid,edge>*/

    vector<long> FrontLayerNodes;
    vector<long> FrontLayerBarrierNodes;
    vector<long> BackgroundLayerNodes;
    vector<long> BackgroundLayerBarrierNodes;

    unordered_map<long, vector<std::pair<long, Exchange>>> barrier_demand; /*<demandid,<(supplyid,supply)>>*/
    std::tr1::unordered_map<long, double> foregroundNodesScalars;/*<demandid,scalar>*/

    //LCAIndexes lcaIndexes;

    GraphData() {}
};

#endif /* CalculatorData_h */
