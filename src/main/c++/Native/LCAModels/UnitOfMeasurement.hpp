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
#ifndef UnitOfMeasurement_h
#define UnitOfMeasurement_h


#include <stdio.h>

#include <string>
#include <vector>

using namespace std;
#include <boost/mpi.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

struct UnitOfMeasurement {
private:


public:

    int UnitId;
    string UnitRefId;
    string SourceUnitName;
    double Factor;
    int DestinationUnitId;
    string DestinationUnitRefId;
    string DestinationUnitName;

    UnitOfMeasurement() {
    }

    UnitOfMeasurement(
            int UnitId_,
            string UnitRefId_,
            string SourceUnitName_,
            double Factor_,
            int DestinationUnitId_,
            string DestinationUnitRefId_,
            string DestinationUnitName_) {


         UnitId=UnitId_;
         UnitRefId=UnitRefId_;
         SourceUnitName=SourceUnitName_;
         Factor=Factor_;
         DestinationUnitId=DestinationUnitId_;
         DestinationUnitRefId=DestinationUnitRefId_;
         DestinationUnitName=DestinationUnitName_;

    }


};


#endif /* UnitOfMeasurement_h */
