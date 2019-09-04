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

#ifndef DemandVectorFactory_hpp
#define DemandVectorFactory_hpp

#include <stdio.h>


#include <stdio.h>
#include <vector>
#include "../LCAModels/ExchangeItem.hpp"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/AppSettings.hpp"

#include <Eigen/SparseCore>
/*#include <Eigen>*/
#include <iostream>
using Eigen::VectorXd;

using namespace std;

class DemandVectorFactory
{
public:
    
    static vector<double> load(AppSettings settings){
        
        ResultsUtilsSingle resUtils;
        
        return resUtils.readDoubleArray(settings.RootPath+"data/calculations/"+settings.CalculationId+"/demand.txt");
        
    }
    
    static VectorXd build(vector<double> demand,long size){
        
          VectorXd b(size);
        
        for(int i=0;i<size;i++){
            b[i]=demand[i];
            
        }
        
        return b;
        
    }
    
};




#endif /* DemandVectorFactory_hpp */
