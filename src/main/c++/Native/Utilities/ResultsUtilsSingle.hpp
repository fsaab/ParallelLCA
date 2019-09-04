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
#ifndef ResultsUtilsSingle_hpp
#define ResultsUtilsSingle_hpp

#include <stdio.h>
#include <vector>
#include "FileUtils.hpp"
#include <sstream>
#include <time.h>
#include <iostream>
#include <string>
#include <string.h>
#include <boost/algorithm/string.hpp>

class ResultsUtilsSingle
{

  private:
    string sep;

  public:
    ResultsUtilsSingle()
    {
        sep = char(29);
    }

    std::vector<std::string> split(std::string input, string sep)
    {

        vector<string> strs;
        boost::split(strs, input, boost::is_any_of(sep));

        return strs;
    }

    vector<int> readIntIndex(string path)
    {

        FileUtils fileUtils;
        vector<string> lines = fileUtils.readTextFromFile(path);

        std::vector<int> intsv;
        for (const auto &s : lines)
        {

            intsv.push_back(stoi(s));
        }

        return intsv;
    }

    vector<long> readLongIndex(string path)
    {

        FileUtils fileUtils;

        vector<string> lines = fileUtils.readTextFromFile(path);

        std::vector<long> longsv;
        for (const auto &s : lines)
        {

            longsv.push_back(stol(s));
        }

        return longsv;
    }

    vector<string> readStringArray(string path)
    {

        FileUtils fileUtils;

        vector<string> lines = fileUtils.readTextFromFile(path);

        return lines;
    }

    vector<double> readDoubleArray(string path)
    {

        FileUtils fileUtils;

        vector<string> lines = fileUtils.readTextFromFile(path);

        std::vector<double> doublesv;
        for (const auto &s : lines)
        {

            doublesv.push_back(stod(s));
        }

        return doublesv;
    }
};

#endif /* ResultsUtilsSingle_hpp */
