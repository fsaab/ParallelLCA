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

#ifndef CalcImpactFactorObj_hpp
#define CalcImpactFactorObj_hpp

#include <stdio.h>

#include <string>
#include <vector>
#include "../LCAModels/CalcImpactFactor.hpp"
#include "LCADB.hpp"
#include <sstream>
#include <algorithm>
#include "../Utilities/StringUtils.h"

using namespace std;

class CalcImpactFactorObj
{
  private:
    string sep;

  public:
    CalcImpactFactorObj()
    {
        sep = char(29);
    }

    vector<string> split(string input, char sep)
    {

        std::istringstream ss(input);
        std::string token;

        vector<string> playerInfoVector;
        while (std::getline(ss, token, sep))
        {
            playerInfoVector.push_back(token);
        }

        return playerInfoVector;
    }

    std::vector<std::string> split(std::string input, std::string sep)
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
   

    CalcImpactFactor create(vector<string> objArr, LCADB *lcadb)
    {

        if (objArr.size() != 14)
        {
            string message = "Error while parsing impact factor item. No enough fields to parse.";
            throw LCAException(500, message);
        }

        if (objArr[0] == "\\N" || objArr[1] == "\\N" || objArr[2] == "\\N" || objArr[4] == "\\N" ||
            objArr[0] == "" || objArr[1] == "" || objArr[2] == "" || objArr[4] == "")
        {
            string message = "Error while parsing impact factor item. Non null fields are empty.";
            throw LCAException(600, message);
        }

        if (!StringUtils::is_number(objArr[0]) || !StringUtils::is_number(objArr[1]) ||
         !StringUtils::is_number(objArr[2]) || !StringUtils::is_number(objArr[4]))
        {
            string message = "Error while parsing impact factor item. field must be a number.";
            throw LCAException(601, message);
        }

        return CalcImpactFactor{
            stol(objArr[0]),                                     //id
            stoi(objArr[1]),                                     //imactCategoryId
            stol(objArr[2]),                                     //flowid
            (*lcadb).unitOfMeasurements[stol(objArr[4])].Factor, //conversionFactor
            (objArr[5] == "\\N" ? 0 : stod(objArr[5])),          //amount
            objArr[6],                                           //amountFormula
            (objArr[7] == "\\N" ? 0 : stoi(objArr[7])),          //uncertaintyType
            (objArr[8] == "\\N" ? 0 : stod(objArr[8])),          //parameter1
            (objArr[10] == "\\N" ? 0 : stod(objArr[10])),        //parameter2
            (objArr[12] == "\\N" ? 0 : stod(objArr[12])),        //parameter3
            objArr[9],                                           //parameter1Formula
            objArr[11],                                          //parameter2Formula
            objArr[13],                                          //parameter3Formula
            (*lcadb).impactCategories[stol(objArr[1])].ICImpactMethod,
            (*lcadb).unitOfMeasurements[stol(objArr[4])].DestinationUnitId};
    }

    string write(CalcImpactFactor obj)
    {

        std::stringstream ss;

        ss << obj.id << "," //id

           << obj.imactCategoryId << "," //imactCategoryId
           << obj.flowId << ","          //flowid

           << obj.conversionFactor << "," //conversionFactor
           << obj.amount << ","           //amount
           << obj.amountFormula << ","    //amountFormula
           << obj.uncertaintyType << ","
           << obj.parameter1 << ","
           << obj.parameter2 << ","
           << obj.parameter3 << ","
           << obj.parameter1Formula << ","
           << obj.parameter2Formula << ","
           << obj.parameter3Formula << ","
           << obj.impactMethod << ","
           << obj.unitid;

        std::string str(ss.str());

        return str;
    }
};

#endif /* CalcImpactFactorObj_hpp */
