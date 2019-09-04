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
#ifndef GeographicalCorrelationObj_hpp
#define GeographicalCorrelationObj_hpp

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
#include "GeographicalCorrelation.hpp"

class GeographicalCorrelationObj {
public:
GeographicalCorrelationObj(){}
	 GeographicalCorrelation NA= GeographicalCorrelation("na", "",0.000, 1,1);

	 GeographicalCorrelation ONE= GeographicalCorrelation("1", "",0.000, 1,1);

	 GeographicalCorrelation TWO= GeographicalCorrelation("2", "",2.5e-5, 1.01,1.04);

	 GeographicalCorrelation THREE= GeographicalCorrelation("3", "",0.0001, 1.02,1.08);

	 GeographicalCorrelation FOUR= GeographicalCorrelation("4", "",0.0006, 1,1.11);

	 GeographicalCorrelation FIVE= GeographicalCorrelation("5", "",0.002, 1.1,1.11);

};

#endif /* GeographicalCorrelationObj_hpp */