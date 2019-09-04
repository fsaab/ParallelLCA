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
#ifndef AppSettings_hpp
#define AppSettings_hpp

#include <iostream>



#include <stdio.h>

#include <string>
#include <vector>
#include <boost/mpi.hpp>
#include <boost/serialization/string.hpp>

#include <boost/serialization/vector.hpp>
using namespace std;



class AppSettings {
   
public:
    
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {


        ar & RootProcessId;
        ar & OutputQunatity;
        ar & SolvingMethod;
        ar & SystemId;
        ar & CalculationId;
        ar & ImpactMethodId;
        ar & lcia;
        ar & montecarlo;
        ar & montecarlo_iterations;
        ar & Parallelism;
        ar & RootPath;
        ar & ProjectId;
        ar & Version;
        ar & DBTemplateId;
        ar & Sensitivity;
    }

    int RootProcessId;
    double OutputQunatity;
    string SolvingMethod;
    string SystemId;
    string CalculationId;
    int ImpactMethodId;
    bool lcia;
    bool montecarlo;
    int montecarlo_iterations;
    int Parallelism;
    string RootPath;
    long ProjectId;
    int Version;
    int DBTemplateId = 1;
    bool Sensitivity;

    AppSettings() {
    }

    AppSettings(int _RootProcessId,
            double _OutputQunatity,
            string _SolvingMethod,
            string _SystemId,
            string _CalculationId,
            int _ImpactMethodId,
            bool _lcia,
            bool _montecarlo,
            int _montecarlo_iterations,
            /* _Files :Array[String],*/
            int _Parallelism,
            string _RootPath,
            long _ProjectId,
            int _Version,bool _Sensitivity) {
        RootProcessId = _RootProcessId;
        OutputQunatity = _OutputQunatity;
        SolvingMethod = _SolvingMethod;
        SystemId = _SystemId;
        CalculationId = _CalculationId;
        ImpactMethodId = _ImpactMethodId;
        lcia = _lcia;
        montecarlo = _montecarlo;
        montecarlo_iterations = _montecarlo_iterations;
        Parallelism = _Parallelism;
        RootPath = _RootPath;
        ProjectId = _ProjectId;
        Version = _Version;
        Sensitivity=_Sensitivity;

    }


};



#endif /* AppSettings_hpp */
