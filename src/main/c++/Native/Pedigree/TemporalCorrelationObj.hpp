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
#ifndef TemporalCorrelationObj_hpp
#define TemporalCorrelationObj_hpp

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
#include "TemporalCorrelation.hpp"
class TemporalCorrelationObj
{
public:
TemporalCorrelationObj(){}
     TemporalCorrelation NA =  TemporalCorrelation("na", "", 0.000, 1, 1);

     TemporalCorrelation ONE =  TemporalCorrelation("1", "", 0.000, 1, 1);

     TemporalCorrelation TWO =  TemporalCorrelation("2", "", 0.0002, 1.03, 1.03);

     TemporalCorrelation THREE =  TemporalCorrelation("3", "", 0.002, 1.1, 1.1);
 
     TemporalCorrelation FOUR =  TemporalCorrelation("4", "", 0.008, 1.2, 1.19);

    TemporalCorrelation FIVE= TemporalCorrelation("5", "", 0.04, 1.5, 1.29);

};

#endif /* ReliabilityObj_hpp */
