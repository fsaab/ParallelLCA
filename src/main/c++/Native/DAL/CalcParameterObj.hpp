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

#ifndef CalcParameterObj_hpp
#define CalcParameterObj_hpp

#include <stdio.h>

#include <string>
#include <vector>
#include "../LCAModels/CalcImpactFactor.hpp"
#include "LCADB.hpp"

#include <sstream>
#include <algorithm>

#include <boost/algorithm/string.hpp>

#include "../Utilities/StringUtils.h"

using namespace std;

class CalcParameterObj
{
  private:
    string sep;

  public:
    CalcParameterObj()
    {
        sep = char(29);
    }

    static std::vector<std::string> split(std::string input, std::string sep)
    {

        vector<string> strs;
        boost::split(strs, input, boost::is_any_of(sep));

        return strs;
    }

    bool stob(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        std::istringstream is(str);
        bool b;
        is >> std::boolalpha >> b;
        return b;
    }

    static CalcParameter parse(string str, string sep)
    {

        vector<std::string> fields = split(str, sep);

        if (fields.size() < 18)
        {
            string message = "Error while parsing parameter item. No enough fields to parse.";
            throw LCAException(500, message);
        }

        if (fields[0] == "\\N" || fields[1] == "\\N" || fields[2] == "\\N" || fields[3] == "\\N" || fields[8] == "\\N" || fields[0] == "" || fields[1] == "" || fields[2] == "" || fields[3] == "" || fields[8] == "")
        {

            string message = "Error while parsing parameter item. Non null fields are empty.";
            throw LCAException(600, message);
        }

        if (!StringUtils::is_number(fields[0]) || !StringUtils::is_number(fields[3]))
        {
            string message = "Error while parsing parameter item. field must be a number.";
            throw LCAException(601, message);
        }

        return CalcParameter(
            stol(fields[0]),                                       //param_id
            fields[1],                                             //ref_id
            fields[2],                                             //name
            (fields[7] == "\\N" ? 0 : stol(fields[7])),            //owner
            fields[8],                                             //scope
            fields[3] == "1",                                      //input
            1,                                                     //conversion
            (fields[9] == "\\N" ? 0 : stod(fields[9])),            //value
            (fields[10] == "\\N" ? "" :  fields[10] ), //formula
            (fields[11] == "\\N" ? 0 : stoi(fields[11])),          //uncertainty type
            (fields[12] == "\\N" ? -1 : stod(fields[13])),         //p1
            (fields[14] == "\\N" ? -1 : stod(fields[15])),         //p2
            (fields[16] == "\\N" ? -1 : stod(fields[17])),         //p3
            (fields[13] == "\\N" ? "" : fields[13]),               //pf1
            (fields[15] == "\\N" ? "" : fields[15]),               //pf2
            (fields[17] == "\\N" ? "" : fields[17])                //pf3
        );
    }
};

#endif /* CalcParameter_hpp */
