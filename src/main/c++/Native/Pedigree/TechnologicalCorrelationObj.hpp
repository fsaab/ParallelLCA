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
#ifndef TechnologicalCorrelationObj_hpp
#define TechnologicalCorrelationObj_hpp

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
#include "TechnologicalCorrelation.hpp"

class TechnologicalCorrelationObj
{
public:
  TechnologicalCorrelationObj(){}
	 TechnologicalCorrelation NA =  TechnologicalCorrelation("na", "", 0.000, 1, 1);

	 TechnologicalCorrelation ONE =  TechnologicalCorrelation("1", "", 0.000, 1, 1);

	 TechnologicalCorrelation TWO =  TechnologicalCorrelation("2", "", 0.0006, 1, 1.18);
	 TechnologicalCorrelation THREE =  TechnologicalCorrelation("3", "", 0.008, 1.2, 1.65);
	 TechnologicalCorrelation FOUR =  TechnologicalCorrelation("4", "", 0.04, 1.5, 2.08);

	 TechnologicalCorrelation FIVE =  TechnologicalCorrelation("5", "", 0.12, 2, 2.8);
};
#endif /* GeographicalCorrelationObj_hpp */