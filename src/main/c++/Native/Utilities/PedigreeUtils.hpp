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
#ifndef PedigreeUtils_H
#define PedigreeUtils_H

#include "../Pedigree/PedigreeMatrixBuilder.hpp"
#include <stdio.h>
#include <boost/algorithm/string/join.hpp>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include "../Pedigree/includes.h"
using namespace std;
class PedigreeUtils
{

public:
  static std::vector<std::string> split(std::string input, std::string sep)
  {

    vector<string> strs;
    boost::split(strs, input, boost::is_any_of(sep));
    return strs;
  }
  
  static vector<double> tranformUncertaintiesUsingPedigree(
    double parameter1, double parameter2, double parameter3,
        int uncertaintyType, 
           string pedigree_uncertainty)
  {

    vector<double> uncParams = {-1, -1, -1};

    if (uncertaintyType != 0 && pedigree_uncertainty != "\\N")
    {
      PedigreeMatrixBuilder pmB = PedigreeMatrixBuilder();

      if (uncertaintyType == 1 || uncertaintyType == 2)
          pmB.setBasicUncertainty(parameter2);

      boost::replace_all(pedigree_uncertainty, "(", "");
      boost::replace_all(pedigree_uncertainty, ")", "");
      vector<string> pedigrees = split(pedigree_uncertainty, ";");

      pmB.setReliability(pedigrees[0]);
      pmB.setCompleteness(pedigrees[1]);
      pmB.setTemporalCorrelation(pedigrees[2]);
      pmB.setGeographicalCorrelation(pedigrees[3]);
      pmB.setTechnologicalCorrelation(pedigrees[4]);

      if (pedigrees.size() == 6)
      {
        pmB.setSampleSize(pedigrees[5]);
      }

      vector<double> arr;

      switch (uncertaintyType)
      {  
      case 1:
        arr = pmB.getUncertaintyLogNormal(parameter1);
        uncParams = {arr[0], arr[1], -1};
        break;

      case 2:
        arr = pmB.getUncertaintyNormal(parameter1);
        uncParams = {arr[0], arr[1], -1};
        break;

        // case 3:
        //   arr = pmB.getUncertaintyTriangular(parameter1, parameter2, parameter3);
        //   uncParams = {to_string(arr[0]), to_string(arr[1]), arr[2]};
        //   break;

        // case 4:
        //   arr = pmB.getUncertaintyUniform(parameter1, parameter2);
        //   uncParams = {to_string(arr[0]), to_string(arr[1]), ""};
        //   break;

      default:
        uncParams = {-1, -1, -1};
        break;
      }
    }

    return uncParams;
  }
};

#endif /* PedigreeUtils_H */
