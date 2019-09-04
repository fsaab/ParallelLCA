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
#ifndef Descriptive_hpp
#define Descriptive_hpp


#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/tail_quantile.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include <string>
#include <cstring>
#include <sstream>
#include <vector>

using boost::multiprecision::cpp_dec_float_50;

using namespace boost::accumulators;


class Descriptive{
    
    static char* to_string(long double x)
    {
        std::ostringstream ss;
        ss << x;
        std::string str= ss.str();
        char *cstr = new char[str.length() + 1];
        strcpy(cstr, str.c_str());
        return cstr;
    }

public:
    
    static std::vector<double> all(double* arrPtr,int length){
        
        std::size_t c =  length; // cache size
        
        std::vector<double> results;
        
        accumulator_set< double, stats< tag::mean, tag::variance, tag::min, tag::max, tag::median, tag::tail_quantile<boost::accumulators::right> >  > acc(tag::tail<boost::accumulators::right>::cache_size = c) ;
        
        int i=0;
                
        while(i<length){
            
            double x=arrPtr[i];
            
            if(x<0){x=-x;} 

            if(!isnan(x)) 
            {
              acc(x);
              
            }

            i++;
            
        }
        
        
        results.push_back(mean(acc));
        results.push_back(sqrt(variance(acc)));
        results.push_back(boost::accumulators::min(acc));
        results.push_back(boost::accumulators::max(acc));
        results.push_back(median(acc));
        
        results.push_back(quantile(acc, quantile_probability = 0.025));
        results.push_back(quantile(acc, quantile_probability = 0.05));
        results.push_back(quantile(acc, quantile_probability = 0.25));
        results.push_back(quantile(acc, quantile_probability = 0.75));
        results.push_back(quantile(acc, quantile_probability = 0.95));
        results.push_back(quantile(acc, quantile_probability = 0.975));
        
        return results;
        
        
    }

    static double variance_stat(double* arrPtr,int length){


        std::size_t c =  10000; // cache size

        std::vector<double> results;

        accumulator_set< double, stats<tag::variance>  > acc;

        int i=0;

        while(i<length){

            double x=arrPtr[i];

            //if(x<0) x=x * -1;
            acc(x);
            i++;
        }

        return variance(acc);

    }

};

#endif /* Descriptive_hpp */
