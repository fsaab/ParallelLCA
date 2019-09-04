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
#ifndef LCAIndexes_hpp
#define LCAIndexes_hpp

#include <stdio.h>


#include <string>
#include <vector>

#include "../Calculators/Models/AppSettings.hpp"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include <map>

using namespace std;
#include <unordered_map>
#include <boost/mpi.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>

class LCAIndexes {
private:

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {

        ar & ElementaryFlowsIndex;
        ar & ElementaryFlowsIndexIndices;

        ar & IntermediateFlowsIndex;
        ar & IntermediateFlowsIndexIndices;

        ar & ProcessesIndex;
        ar & ProcessesIndexIndices;

        ar & ImpactCategoryIndex;
        ar & ImpactCategoryIndexIndices;

        ar & FrontLayer_ProcessesIndex;
        ar & FrontLayer_ProcessesIndexIndices;

        ar & FrontLayer_IntermediateFlowsIndex;
        ar & FrontLayer_IntermediateFlowsIndexIndices;

        ar & FrontLayerBarrier_ProcessesIndex;
        ar & BackgroundLayerBarrier_ProcessesIndex;
        
        ar & BackgroundLayer_ProcessesIndex;
        ar & BackgroundLayer_ProcessesIndexIndices;
        
        ar & BackgroundLayer_IntermediateFlowsIndex;
        ar & BackgroundLayer_IntermediateFlowsIndexIndices;

    }

public:


    vector<long> ElementaryFlowsIndex;
    unordered_map<long, long> ElementaryFlowsIndexIndices;

    vector<long> IntermediateFlowsIndex;
    unordered_map<long, long> IntermediateFlowsIndexIndices;

    vector<long> ProcessesIndex;
    unordered_map<long, long> ProcessesIndexIndices;

    vector<long> FrontLayer_ProcessesIndex;
    unordered_map<long, long> FrontLayer_ProcessesIndexIndices;


    vector<long> BackgroundLayer_ProcessesIndex;
    unordered_map<long, long> BackgroundLayer_ProcessesIndexIndices;


    vector<long> FrontLayer_IntermediateFlowsIndex;
    unordered_map<long, long> FrontLayer_IntermediateFlowsIndexIndices;

    vector<long> BackgroundLayer_IntermediateFlowsIndex;
    unordered_map<long, long> BackgroundLayer_IntermediateFlowsIndexIndices;

    vector<long> FrontLayerBarrier_ProcessesIndex;
    vector<long> BackgroundLayerBarrier_ProcessesIndex;


    vector<long> ImpactCategoryIndex;
    unordered_map<long, long> ImpactCategoryIndexIndices;



    //    LCAIndexes(AppSettings _settings):settings(_settings){
    //        
    //        //loadIndexes();
    //        
    //    }

    unsigned long ElementaryFlowsIndexLength() {
        return ElementaryFlowsIndex.size();
    }
    unsigned long IntermediateFlowsIndexLength() {
        return IntermediateFlowsIndex.size();
    }
    unsigned long ProcessesIndexLength() {
        return ProcessesIndex.size();
    }
    unsigned long FrontLayer_ProcessesIndexLength() {
        return FrontLayer_ProcessesIndex.size();
    }
    unsigned long FrontLayer_IntermediateFlowsIndexLength() {
        return FrontLayer_IntermediateFlowsIndex.size();
    }
    unsigned long ImpactCategoryIndexLength() {
        return ImpactCategoryIndex.size();
    }

    //AppSettings settings;

    void loadIndexesForProcesses() {


        long ip = 0, fl_ip = 0;

        for (auto&& e : ProcessesIndex) {

            ProcessesIndexIndices[e] = ip;

            ip++;


        }


    }

    void loadIndexesForFrontLayerProcesses() {


        long ip = 0;

        for (auto&& e : FrontLayer_ProcessesIndex) {

            FrontLayer_ProcessesIndexIndices[e] = ip;

            ip++;


        }


    }

    void loadIndexesForBackgroundLayerProcesses() {


        long ip = 0;

        for (auto&& e : BackgroundLayer_ProcessesIndex) {

            BackgroundLayer_ProcessesIndexIndices[e] = ip;

            ip++;


        }


    }

    void loadIndexesForItermediateFlows() {

        ResultsUtilsSingle resUtils;


        int iif = 0;

        for (long e : IntermediateFlowsIndex) {

            IntermediateFlowsIndexIndices[e] = iif;

            iif++;


        }

    }

    void loadIndexesForFrontLayerItermediateFlows() {

        ResultsUtilsSingle resUtils;


        int iif = 0;

        for (long e : FrontLayer_IntermediateFlowsIndex) {

            FrontLayer_IntermediateFlowsIndexIndices[e] = iif;

            iif++;

        }

    }

    void loadIndexesForBackgroundLayerItermediateFlows() {

        ResultsUtilsSingle resUtils;


        int iif = 0;

        for (long e : BackgroundLayer_IntermediateFlowsIndex) {

            BackgroundLayer_IntermediateFlowsIndexIndices[e] = iif;

            iif++;

        }

    }

    void loadIndexesForElementaryFlows() {


        ResultsUtilsSingle resUtils;



        //        ElementaryFlowsIndex = resUtils.readLongIndex(
        //
        //                "./data/calculations/ETS_123_v1_234364_-52234918/B_flowidSeq.txt"
        //
        //                );





        int ief = 0;

        for (int e : ElementaryFlowsIndex) {

            ElementaryFlowsIndexIndices[e] = ief;

            ief++;


        }



    }

    void loadIndexesForImpactCategories() {

        int ic = 0;

        for (int cat : ImpactCategoryIndex) {

            ImpactCategoryIndexIndices[cat] = ic;

            ic++;
        }

    }
};


#endif /* LCAIndexes_hpp */
