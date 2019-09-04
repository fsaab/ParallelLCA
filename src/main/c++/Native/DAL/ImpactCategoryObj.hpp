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

#ifndef ImpactCategoryObj_hpp
#define ImpactCategoryObj_hpp

#include <stdio.h>

#include <string>
#include <vector>
#include "../LCAModels/ImpactCategory.hpp"


using namespace std;


#include<iostream>
#include <sstream>
#include <algorithm>

#include <boost/algorithm/string.hpp>

class ImpactCategoryObj {
private:

    string sep;


public:

    ImpactCategoryObj() {
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

    static ImpactCategory parse(string str,string sep) {

        vector<std::string> objArr = split(str, sep);

        ImpactCategory ic{

            //             int ICId;
            //    string ICName;
            //    string ICDescription;
            //
            //    string ICReferenceUnit;
            //    int ICImpactMethod;

            stoi(objArr[0]),
            objArr[1],
            objArr[2],
            objArr[3],
            stoi(objArr[4])
        };


        return ic;

    }

    ImpactCategory create(vector <string> objArr) {

       ImpactCategory ic{

            //             int ICId;
            //    string ICName;
            //    string ICDescription;
            //
            //    string ICReferenceUnit;
            //    int ICImpactMethod;

            stoi(objArr[0]),
            objArr[1],
            objArr[2],
            objArr[3],
            stoi(objArr[4])
        };


        return ic;

    }


};



#endif /* ImpactCategoryObj_hpp */
