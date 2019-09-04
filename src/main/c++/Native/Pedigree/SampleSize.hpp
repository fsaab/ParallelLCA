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
#ifndef SampleSize_hpp
#define SampleSize_hpp


#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
class SampleSize
{

public:

    string key;
    string value;
    double indicator;
    double indicatorv2;
    double variance;

    string getKey()
    {
        return key;
    }

    string getValue()
    {
        return value;
    }

    double getIndicator()
    {
        return indicator;
    }

    double getIndicatorv2()
    {
        return indicatorv2;
    }

    double getVariance()
    {
        return variance;
    }

    SampleSize(string key_, string value_, double variance_, double indicator_, double indicatorv2_)
    {
        key = key_;
        value = value_;
        indicator = indicator_;
        indicatorv2 = indicatorv2_;
        variance = variance_;
    }
};



#endif /* SampleSize_hpp */