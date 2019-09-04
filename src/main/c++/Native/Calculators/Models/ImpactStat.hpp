//
//  CalcImpactFactor.h
//  LCA.Kernel
//
//  Created by francois saab on 2/17/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef ImpactStat_h
#define ImpactStat_h

#include <stdio.h>

#include <string>
#include <vector>

using namespace std;

#include <boost/mpi.hpp>

#include <boost/serialization/vector.hpp>

struct  ImpactStat {
    
    
    private:

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {


    ar & imactCategoryId ;

     ar & mean; //mean
            ar & std; //std
            ar & min; // min
            ar & max; //max
            ar & median; //median
            ar & q1;
            ar & q2;
            ar & q3;
            ar & q4;
            ar & q5;
            ar & q6;
      
        
    }
    
public:
    
    int imactCategoryId = 0;

     double mean=0; //mean
            double std=0; //std
            double min=0; // min
            double max=0; //max
            double median=0; //median
            double q1=0;
            double q2=0;
            double q3=0;
            double q4=0;
            double q5=0;
            double q6=0;
    
    ImpactStat(){}
    ImpactStat(  int _imactCategoryId ,
                  double _mean=0, //mean
            double _std=0, //std
            double _min=0, // min
            double _max=0, //max
            double _median=0, //median
            double _q1=0,
            double _q2=0,
            double _q3=0,
            double _q4=0,
            double _q5=0,
            double _q6=0
                )
    {
        
        imactCategoryId=_imactCategoryId ;
                   mean=_mean; //mean
             std=_std; //std
             min=_min; // min
             max=_min; //max
             median=_median; //median
             q1=_q1;
             q2=_q2;
             q3=_q3;
             q4=_q4;
             q5=_q5;
             q6=_q6;
        
    }
    
    
};






#endif /* ImpactStat_h */
