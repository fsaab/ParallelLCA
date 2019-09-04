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

#ifndef LCAEdgeCompact_hpp
#define LCAEdgeCompact_hpp

#include <stdio.h>
#include "../Calculators/Models/AppSettings.hpp"

#include "../Utilities/ResultsUtilsSingle.hpp"

#include "../LCAModels/LCAIndexes.hpp"
#include "NodeInfo.hpp"


#include <boost/mpi.hpp>
#include <unordered_map>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/string.hpp>

class LCAEdgeCompact
{
  public:

    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
    }

    long InputExchangeId;
    long ProducerExchangeId;
    long ProcessSrcId;
    long ProcessDestId;
    double InputCalculatedValue;
    double OutputCalculatedValue;
    NodeInfo* InputScalar;
    NodeInfo* OutputScalar;
    long FlowId;

    LCAEdgeCompact()
    {
    }

    LCAEdgeCompact(long InputExchangeId_,
                   long ProducerExchangeId_,
                   long ProcessSrcId_,
                   long ProcessDestId_,
                   double InputCalculatedValue_,
                   double OutputCalculatedValue_,
                    NodeInfo* InputScalar_,
                   NodeInfo* OutputScalar_,
                   
                   long FlowId_)
    {

        InputExchangeId = InputExchangeId_;
        ProducerExchangeId = ProducerExchangeId_;
        ProcessSrcId = ProcessSrcId_;
        ProcessDestId = ProcessDestId_;
        InputCalculatedValue = InputCalculatedValue_;
        OutputCalculatedValue = OutputCalculatedValue_;
        InputScalar = InputScalar_;
        OutputScalar = OutputScalar_;
        FlowId = FlowId_;
    }
};

#endif /* LCAEdgeCompact_hpp */
