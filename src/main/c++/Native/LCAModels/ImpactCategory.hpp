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
#ifndef ImpactCategory_h
#define ImpactCategory_h


#include <stdio.h>

#include <string>
#include <vector>

using namespace std;
#include <boost/mpi.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

struct ImpactCategory {
private:


public:


    int ICId;
    string ICName;
    string ICDescription;
    string ICReferenceUnit;
    int ICImpactMethod;

    ImpactCategory() {
    }

    ImpactCategory(
            int ICId_,
            string ICName_,
            string ICDescription_,
            string ICReferenceUnit_,
            int ICImpactMethod_) {


        ICId = ICId_;
        ICName = ICName_;
        ICDescription = ICDescription_;
        ICReferenceUnit = ICReferenceUnit_;
        ICImpactMethod = ICImpactMethod_;

    }


};


#endif /* ImpactCategory_h */
