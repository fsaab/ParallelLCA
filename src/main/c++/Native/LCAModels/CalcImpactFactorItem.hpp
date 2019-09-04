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

#ifndef ImpactFactorItem_h
#define ImpactFactorItem_h

#include "CalcImpactFactor.hpp"
#include <boost/mpi.hpp>

class CalcImpactFactorItem {
private:

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {


        ar & i;
        ar & j;
        ar & imf;

    }

public:

    long i;
    long j;
    CalcImpactFactor imf;
    
    CalcImpactFactorItem(){}

    CalcImpactFactorItem(long _i,
            long _j,
            CalcImpactFactor _imf) : imf(_imf) {

        i = _i;
        j = _j;



    };

};


#endif /* ImpactFactorCell_h */
