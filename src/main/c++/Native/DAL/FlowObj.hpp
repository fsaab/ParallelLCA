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

#ifndef Flow_hpp
#define Flow_hpp

#include <stdio.h>

#include <string>
#include <vector>
#include "../LCAModels/Flow.hpp"


using namespace std;


#include<iostream>
#include <sstream>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include "FlowObj.hpp"
#include "ProcessObj.hpp"
#include "ExchangeObj.hpp"
#include "UnitOfMeasurementObj.hpp"
#include "ImpactCategoryObj.hpp"

#include "../Utilities/StringUtils.h"


class FlowObj {
private:

    string sep;


public:

    FlowObj() {
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

    static Flow parse(string str,string sep) {

        vector<std::string> objArr = split(str, sep);

         if (objArr.size() !=4)
        {
            string message = "Error while parsing flow item. No enough fields to parse.";
            throw LCAException(500, message);
        }

        if (objArr[0] == "\\N" || objArr[2] == "\\N"  ||
            objArr[0] == ""  || objArr[2] == "")
        {
            string message = "Error while parsing flow item. Non null fields are empty.";
            throw LCAException(600, message);
        }

        int flowtype= (objArr[2]=="ELEMENTARY_FLOW"? 2:1) ;

        Flow f{
            
            stol(objArr[0]), //flowid
            
            flowtype, //flowType
            
            objArr[3],
          };


        return f;

    }

    Flow create(vector <string> objArr) {

       Flow f{
            
            stol(objArr[0]), //flowid
            
            stoi(objArr[2]), //flowType
            
            objArr[3],
          };


        return f;

    }

   
};



#endif /* Exchange_hpp */
