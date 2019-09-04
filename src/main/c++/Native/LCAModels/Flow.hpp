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
#ifndef Flow_h
#define Flow_h


#include <stdio.h>

#include <string>
#include <vector>

using namespace std;
#include <boost/mpi.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

struct Flow {
private:

    
public:

    long flowId = 0;
    string flowUid = "";
    int flowType = -1;

    Flow() {
    }

    Flow(
            long _flowId,

            int _flowType,
            string _flowUid) {

        flowId = _flowId;
        flowType = _flowType;
        flowUid = _flowUid;

        
    }


};


#endif /* Exchange_h */
