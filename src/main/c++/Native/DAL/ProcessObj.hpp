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

#ifndef Process_hpp
#define Process_hpp

#include <stdio.h>

#include <string>
#include <vector>
#include "../LCAModels/Process.hpp"

using namespace std;

#include<iostream>
#include <sstream>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "../Utilities/StringUtils.h"

class ProcessObj {
private:

    string sep;

public:

    ProcessObj() {
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

    static Process parse(string str,string sep) {

        vector<std::string> objArr = split(str, sep);

         if (objArr.size() !=5)
        {
            string message = "Error while parsing process item. No enough fields to parse.";
            throw LCAException(500, message);
        }

        if (objArr[0] == "\\N" || objArr[2] == "\\N" || objArr[4] == "\\N" ||
            objArr[0] == ""  || objArr[2] == "" || objArr[4] == "")
        {
            string message = "Error while parsing process item. Non null fields are empty.";
            throw LCAException(600, message);
        }
        
        string input=objArr[4];
        

        Process p{


            stol(objArr[0]),
            objArr[1],
            objArr[2],
            objArr[3],
            input=="1"};


        return p;

    }

    


};



#endif /* Process_hpp */
