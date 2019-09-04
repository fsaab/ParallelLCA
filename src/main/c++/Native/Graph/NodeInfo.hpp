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

#ifndef NodeInfo_hpp
#define NodeInfo_hpp

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

struct NodeInfo {
    long id;
    bool exist;
    bool isBackgroundLayer;
    bool isBarrier;
    double scalar;

     NodeInfo(){
         
     }

    NodeInfo(
            long id_,
            bool exist_,
            bool isBackgroundLayer_,
            bool isBarrier_,double scalar_) {

        id = id_;
        exist = exist_;
        isBackgroundLayer = isBackgroundLayer_;
        isBarrier = isBarrier_;
        scalar=scalar_;
    }


};



#endif /* EdgeShare_hpp */
