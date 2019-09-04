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

#ifndef UnitOfMeasurementObj_hpp
#define UnitOfMeasurementObj_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "../LCAModels/UnitOfMeasurement.hpp"
#include<iostream>
#include <sstream>
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace std;

class UnitOfMeasurementObj {
private:
    
    string sep;
    
public:

    UnitOfMeasurementObj() {
        sep = char(29);
    }
    
    static std::vector<std::string> split(std::string input, std::string sep) {

        vector<string> strs;
        boost::split(strs, input, boost::is_any_of(sep));

        return strs;
    }
    bool stob(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        std::istringstream is(str);
        bool b;
        is >> std::boolalpha >> b;
        return b;
    }
   
    static UnitOfMeasurement parse(string str,string sep) {

        vector<std::string> objArr = split(str, sep);

        UnitOfMeasurement u{

            //            int UnitId;
            //            string UnitRefId;
            //            string SourceUnitName;
            //            double Factor;
            //            int DestinationUnitId;
            //            string DestinationUnitRefId;
            //            string DestinationUnitName;

            stoi(objArr[0]),
            objArr[1],
            objArr[2],
            stod(objArr[3]),
            stoi(objArr[4]),
            objArr[5],
            objArr[6]
        };


        return u;

    }
   
    UnitOfMeasurement create(vector <string> objArr) {

        UnitOfMeasurement u{

            //            int UnitId;
            //            string UnitRefId;
            //            string SourceUnitName;
            //            double Factor;
            //            int DestinationUnitId;
            //            string DestinationUnitRefId;
            //            string DestinationUnitName;

            stoi(objArr[0]),
            objArr[1],
            objArr[2],
            stod(objArr[3]),
            stoi(objArr[4]),
            objArr[5],
            objArr[6]};


        return u;

    }

};



#endif /* UnitOfMeasurement_hpp */
