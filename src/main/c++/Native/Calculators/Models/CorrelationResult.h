//
//  CorrelationResult.h
//  LCA.Kernel
//
//  Created by francois saab on 2/21/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef CorrelationResult_h
#define CorrelationResult_h

class CorrelationResult{
   

public:
    long in_id;
    char in_type;
    int out_id;
    double cor;
    CorrelationResult(long _in_id,char _in_type,int _out_id,double _cor){

        in_id=_in_id;
         in_type=_in_type;
         out_id=_out_id;
         cor=_cor;


        
    }

    
};


#endif /* CorrelationResult_h */
