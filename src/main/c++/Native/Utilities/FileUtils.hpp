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
#ifndef FileUtils_hpp
#define FileUtils_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using std::ifstream;
#include <iterator>
using std::istream_iterator;
#include <algorithm>
using std::copy;
using namespace std;

#include <sys/stat.h>
//#define BOOST_FILESYSTEM_NO_DEPRECATED
//#include "boost/filesystem/operations.hpp"
//#include "boost/filesystem/path.hpp"
//#include "boost/progress.hpp"

//namespace fs = boost::filesystem;

class FileUtils
{
  public:
    inline bool exists(const std::string &name)
    {
        struct stat buffer;
        return (stat(name.c_str(), &buffer) == 0);
    }

    vector<string> readTextFromFile(string path)
    {

        vector<string> vecOfStrs;

        // Open the File
        std::ifstream in(path.c_str());


        std::string str;
        // Read the next line from File untill it reaches the end.
        while (std::getline(in, str))
        {
            // Line contains string of length > 0 then save it in vector
            if (str.size() > 0)
                vecOfStrs.push_back(str);
        }
        //Close The File
        in.close();

        // vector<string> lines;

        // ifstream file;
        // file.open(path);

        // string data; // it could be int, or any data type you want

        // while (std::getline(file, data)) // loop until end of file
        // {

        //     lines.push_back(data);
        // }

        // file.close();

        return vecOfStrs;
    }

    bool writeTextToFile(string data, string path)
    {
        ofstream file =  ofstream(path.c_str());
        file << data;
        file.close();
        return true;
    }

    bool appendTextToFile(string data, string path)
    {

        ofstream file =  ofstream(path.c_str(), std::ofstream::out | std::ofstream::app);
        file << data;
        file.close();
        return true;
    }

    bool newfolder(string path)
    {

        mkdir(path.c_str(), S_IRWXU);

        return true;
    }
};
#endif /* FileUtils_hpp */
