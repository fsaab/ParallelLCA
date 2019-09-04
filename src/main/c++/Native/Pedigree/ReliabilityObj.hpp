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
#ifndef ReliabilityObj_hpp
#define ReliabilityObj_hpp

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
#include "Reliability.hpp"

class ReliabilityObj
{
  public:
    ReliabilityObj(){}
    Reliability NA =  Reliability("na", "", 0.000, 1, 1);

    Reliability ONE =  Reliability("1", "", 0.000, 1, 1);

    Reliability TWO =  Reliability("2", "", 0.0006, 1.05, 1.54);

    Reliability THREE =  Reliability("3", "", 0.002, 1.1, 1.61);

    Reliability FOUR =  Reliability("4", "", 0.008, 1.2, 1.69);

    Reliability FIVE =  Reliability("5", "", 0.04, 1.5, 1.69);
};

#endif /* ReliabilityObj_hpp */