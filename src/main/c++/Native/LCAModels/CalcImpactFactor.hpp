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
#ifndef CalcImpactFactor_h
#define CalcImpactFactor_h





#include <stdio.h>

#include <string>
#include <vector>
#include <boost/mpi.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
using namespace std;

struct CalcImpactFactor {
    
private:
    
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {

     ar & id;
     ar & imactCategoryId ;

     ar & flowId;

     ar & conversionFactor ;
     ar & amount ;
     //ar & amountFormula;
     ar & uncertaintyType ;
     ar & parameter1 ;
     ar & parameter2 ;
     ar & parameter3 ;
     //ar & parameter1Formula;
     //ar & parameter2Formula;
     //ar & parameter3Formula;

     ar & impactMethod ;
     ar & unitid ;
    }
    
public:
    
    long id;
    int imactCategoryId = 0;

    long flowId = 0;

    double conversionFactor = .0;
    double amount = .0;
    string amountFormula;
    int uncertaintyType = 0;
    double parameter1 = 0.0;
    double parameter2 = 0.0;
    double parameter3 = 0.0;
    string parameter1Formula;
    string parameter2Formula;
    string parameter3Formula;

    int impactMethod = 0;
    int unitid = 0;

    CalcImpactFactor() {
        id = -1;
        imactCategoryId = -1;
        flowId = -1;
        conversionFactor = -1;
        amount = -1;
        amountFormula = "";
        uncertaintyType = -1;
        parameter1 = -1;
        parameter2 = -1;
        parameter3 = -1;
        parameter1Formula = "";
        parameter2Formula = "";
        parameter3Formula = "";
        impactMethod = -1;
        unitid = -1;
    }

    CalcImpactFactor(
            long id_,
            int _imactCategoryId,
            long _flowId,

            double _conversionFactor,
            double _amount,
            string _amountFormula,
            int _uncertaintyType,
            double _parameter1,
            double _parameter2,
            double _parameter3,
            string _parameter1Formula,
            string _parameter2Formula,
            string _parameter3Formula,
            int _impactMethod,
            int _unitid
            ) {

                    id = id_;
                    imactCategoryId = _imactCategoryId;
                    flowId = _flowId;
                    conversionFactor = _conversionFactor;
                    amount = _amount;
                    amountFormula = _amountFormula;
                    uncertaintyType = _uncertaintyType;
                    parameter1 = _parameter1;
                    parameter2 = _parameter2;
                    parameter3 = _parameter3;
                    parameter1Formula = _parameter1Formula;
                    parameter2Formula = _parameter2Formula;
                    parameter3Formula = _parameter3Formula;
                    impactMethod = _impactMethod;
                    unitid = _unitid;

    }


};






#endif /* CalcImpactFactor_h */
