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

#ifndef Exchange_hpp
#define Exchange_hpp

#include <stdio.h>

#include <string>
#include <vector>
#include "../LCAModels/Exchange.hpp"
#include "LCADB.hpp"
#include "../Messaging/LCAException.hpp"
using namespace std;

#include <iostream>
#include <sstream>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include "../Utilities/PedigreeUtils.hpp"

#include "../Utilities/StringUtils.h"

class ExchangeObj
{
  private:
    string sep;

  public:
    ExchangeObj()
    {
        sep = char(29);
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

    Exchange create(vector<string> objArr, LCADB *lcadb)
    {
        if (objArr.size() != 18)
        {
            string message = "Error while parsing exchange item. No enough fields to parse.";
            throw LCAException(500, message);
        }

        if (objArr[0] == "\\N" || objArr[1] == "\\N" || objArr[2] == "\\N" || objArr[3] == "\\N" || objArr[4] == "\\N" || objArr[6] == "\\N" ||
            objArr[0] == "" || objArr[1] == "" || objArr[2] == "" || objArr[3] == "" || objArr[4] == "" || objArr[6] == "")
        {
            string message = "Error while parsing exchange item. Non null fields are empty.";
            throw LCAException(600, message);
        }

        if (!StringUtils::is_number(objArr[0]) || !StringUtils::is_number(objArr[1]) ||
         !StringUtils::is_number(objArr[2]) || !StringUtils::is_number(objArr[3]) || 
         !StringUtils::is_number(objArr[4]) || !StringUtils::is_number(objArr[6]))
        {

            //cout<<objArr[0] << " " << objArr[1] << " " << objArr[2] << " " << objArr[3] << " " << objArr[4] << " " << objArr[6] <<endl;
            string message = "Error while parsing exchange item. field must be a number.";
            throw LCAException(601, message);
        }

        vector<double> pedigree_params = PedigreeUtils::tranformUncertaintiesUsingPedigree(
            (objArr[10] == "\\N" ? -1 : stod(objArr[10])), //p1
            (objArr[13] == "\\N" ? -1 : stod(objArr[13])), //p2
            (objArr[14] == "\\N" ? -1 : stod(objArr[14])), //p3
            (objArr[9] == "\\N" ? 0 : stoi(objArr[9])),    //unctype
            objArr[16]);                                   //pedigree

        Exchange e = Exchange{

            stol(objArr[0]),                                      //exchnageid
            stol(objArr[1]),                                      //processId
            stol(objArr[2]),                                      //flowid
            objArr[3] == "1",                                     //input
            (*lcadb).unitOfMeasurements[stol(objArr[6])].Factor,  //conversionFactor
            stod(objArr[4]),                                      //amount
            (objArr[8] == "\\N" ? "" : objArr[8]),                //amountFormula
            (objArr[9] == "\\N" ? 0 : stoi(objArr[9])),           //uncertaintyType
            (pedigree_params[0] != -1 ? pedigree_params[0] : -1), //parameter1
            (pedigree_params[1] != -1 ? pedigree_params[1] : -1), //parameter2
            (pedigree_params[2] != -1 ? pedigree_params[2] : -1), //parameter3
            (objArr[11] == "\\N" ? "" :objArr[11]),    //parameter1Formula
            (objArr[12] == "\\N" ? "" :objArr[12]),    //parameter2Formula
            (objArr[15] == "\\N" ? "" :objArr[15]),    //parameter3Formula
            (*lcadb).flows[stol(objArr[2])].flowType,             //flowType
            (objArr[5] == "\\N" ? 0 : stol(objArr[5])),           //defaultProviderId
            (*lcadb).processes[stol(objArr[1])].Process_Type,
            (*lcadb).processes[stol(objArr[1])].IsBackgroundLayer,
            stoi(objArr[6])};

        return e;
    }

    string write(Exchange obj)
    {
        std::stringstream ss;

        ss << obj.exchangeId << ","
           << obj.processId << ","        //processId
           << obj.flowId << ","           //flowid
           << obj.input << ","            //input
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
           << obj.flowType << ","
           << obj.defaultProviderId << ","
           << obj.processType << ","
           << obj.isBackground << ","
           << obj.unitid;

        std::string str(ss.str());
        return str;
    }
};

#endif /* Exchange_hpp */
