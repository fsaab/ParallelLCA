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
#ifndef CompletenessObj_hpp
#define CompletenessObj_hpp

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
#include "Completeness.hpp"


/**
 * Created by francoissaab on 3/10/17.
 */

/*[0.000, 0.0001, 0.0006, 0.002, 0.008],  # Completeness*/

/*"completeness": (1., 1.03, 1.04, 1.08, 1.08),*/
 class CompletenessObj {
public:
    CompletenessObj(){}

     Completeness NA= Completeness("na", "",0.000, 1,1);
     Completeness ONE= Completeness("1", "",0.000, 1,1);
     Completeness TWO= Completeness("2", "",0.0001, 1.02,1.03);
     Completeness THREE= Completeness("3", "",0.0006, 1.05,1.04);
     Completeness FOUR= Completeness("4", "",0.002, 1.10,1.08);
     Completeness FIVE= Completeness("5", "", 0.008, 1.20,1.08);
};
#endif /* Completeness_hpp */