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

#ifndef ExchangeItem_h
#define ExchangeItem_h

#include "Exchange.hpp"
#include <random>
#include <boost/mpi.hpp>

#include <boost/serialization/vector.hpp>

class ExchangeItem
{
  private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {

        ar &i;
        ar &j;
        ar &i_fl;
        ar &j_fl;
        ar &i_bl;
        ar &j_bl;
        ar &exch;
    }

  public:
    long i;
    long j;
    long i_fl;
    long j_fl;

    long i_bl;
    long j_bl;
    Exchange exch;
    //    std::mt19937 generator;
    std::normal_distribution<float> distribution;

    ExchangeItem()
    {
    }

    ExchangeItem(long _i,
                 long _j,

                 Exchange _exch
                 //                     ,
                 //                      std::mt19937 _generator
                 ) : exch(_exch)
    {

        i = _i;
        j = _j;

        //        generator=_generator;

        double mean = 0;

        double stddev = 0;

        switch (_exch.uncertaintyType)
        {

        case 1:

            mean = log(fabs(_exch.parameter1));

            stddev = log(fabs(_exch.parameter2));

            distribution = std::normal_distribution<float>(mean, stddev);

            break;

        case 2:

            mean = _exch.parameter1;

            stddev = _exch.parameter2;

            distribution = std::normal_distribution<float>(mean, stddev);

            break;
        }
    };

    ExchangeItem(long _i,
                 long _j,
                 long _i_fl,
                 long _j_fl,
                 long _i_bl,
                 long _j_bl,
                 Exchange _exch

                 ) : exch(_exch)
    {

        i = _i;
        j = _j;
        i_fl = _i_fl;
        j_fl = _j_fl;
        i_bl = _i_bl;
        j_bl = _j_bl;

    };

};
#endif /* ExchangeItem_h */
