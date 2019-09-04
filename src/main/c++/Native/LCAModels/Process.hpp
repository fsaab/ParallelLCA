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
#ifndef Process_h
#define Process_h


#include <stdio.h>

#include <string>
#include <vector>

using namespace std;
#include <boost/mpi.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

struct Process {
    
private:


public:
    
    long ProcId;
    string Name;
    string Process_Type;
    string ref_id;
    bool IsBackgroundLayer;

    Process(){
    }

    Process(long ProcId_,
            string Name_,
            string Process_Type_,
            string ref_id_,
            bool IsBackgroundLayer_
            ) {  
        ProcId = ProcId_;
        Name = Name_;
        Process_Type = Process_Type_;
        ref_id = ref_id_;
        IsBackgroundLayer = IsBackgroundLayer_;
    }


};


#endif /* Process_h */
