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
#ifndef SampleSizeObj_hpp
#define SampleSizeObj_hpp

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
#include "SampleSize.hpp"

class SampleSizeObj {
public:
     SampleSizeObj(){}

     
     SampleSize NA= SampleSize("na", "",0, 1,1);
     SampleSize ONE= SampleSize("1", "",0, 1,1);
     SampleSize TWO= SampleSize("2", "",0, 1.02,1);
     SampleSize THREE= SampleSize("3", "",0, 1.05,1);
     SampleSize FOUR= SampleSize("4", "",0, 1.1,1);
     SampleSize FIVE= SampleSize("5", "",0, 1.2,1);


};
#endif /* SampleSizeObj_hpp */
