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

#ifndef Exchange_h
#define Exchange_h


#include <stdio.h>

#include <string>
#include <vector>

using namespace std;
#include <boost/mpi.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>



struct Exchange  {
    
    private:

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {

 ar & exchangeId;
    ar & processId ;
    ar & flowId ;
    ar & input ;
    ar & conversionFactor ;
    ar &  amount ;
    ar & amountFormula ;
    ar & uncertaintyType;
    ar & parameter1 ;
    ar & parameter2 ;
    ar & parameter3 ;
    ar & parameter1Formula;
    ar & parameter2Formula;
    ar & parameter3Formula;
    ar & flowType ;
    ar & defaultProviderId ;

    ar & processType;
    ar & isBackground;
    ar & unitid;
        
    }
    
public:
    long exchangeId;
    long processId = 0;
    long flowId = 0;
    bool input = false;
    double conversionFactor = .0;
    double  amount = .0;
    string amountFormula ;
    int uncertaintyType= 0;
    double parameter1 = 0.0;
    double parameter2 = 0.0;
    double parameter3 = 0.0;
    string parameter1Formula;
    string parameter2Formula;
    string parameter3Formula;
    int flowType = -1;
    long defaultProviderId = 0L;
    string processType="";
    bool isBackground=true;
    int unitid;
    
    Exchange(){}
    Exchange( long _exchangeId,
             long _processId ,
                 long _flowId ,
                 bool _input ,
                 double _conversionFactor,
                 double  _amount ,
                 string _amountFormula ,
                 int _uncertaintyType,
                 double _parameter1 ,
                 double _parameter2 ,
                 double _parameter3 ,
                 string _parameter1Formula,
                 string _parameter2Formula,
                 string _parameter3Formula,
                 int _flowType ,
                 long _defaultProviderId,
                 
                 string processType_,
                 bool _isBackground, int _unitid)
    {
        
        exchangeId= _exchangeId;
        processId =_processId;
        flowId =_flowId;
        input =_input;
        conversionFactor=_conversionFactor;
        amount=_amount ;
        amountFormula=_amountFormula ;
        uncertaintyType=_uncertaintyType;
        parameter1 =_parameter1;
        parameter2=_parameter2 ;
        parameter3=_parameter3 ;
        parameter1Formula=_parameter1Formula;
        parameter2Formula=_parameter2Formula;
        parameter3Formula=_parameter3Formula;
        flowType=_flowType ;
        defaultProviderId=_defaultProviderId;
        processType=processType_;
        isBackground=_isBackground;
        unitid=_unitid;
    }


};


#endif /* Exchange_h */
