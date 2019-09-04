//
//  SamplesRanks.hpp
//  LCA.Kernel
//
//  Created by francois saab on 2/17/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef SamplesRanks_hpp
#define SamplesRanks_hpp

#include <iostream>



#include <stdio.h>

#include <string>
#include <vector>

using namespace std;



struct SamplesRanks {
   
    
    long EntityId=0;
    long ProcessId=0;
    double Average=0.0;
    vector<double> ranks;

    SamplesRanks() {
    }
    
    SamplesRanks(long EntityId_,long ProcessId_,double Average_)
             {
        EntityId= EntityId_;
        Average = Average_;
        ProcessId=ProcessId_;
    }


    


};



#endif /* SamplesRanks_hpp */
