//
//  IterationsResults.hpp
//  LCA.Kernel
//
//  Created by francois saab on 2/21/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef IterationsResults_hpp
#define IterationsResults_hpp

#include <stdio.h>
#include "AppSettings.hpp"
#include "CalculatorData.h"
#include "../../Utilities/ResultsUtilsSingle.hpp"
#include "TwoDimVectorStore.hpp"
#include "../../LCAModels/LCAIndexes.hpp"
#include <boost/mpi.hpp>

#include <boost/serialization/vector.hpp>
class IterationsResults {
private:

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & store_lcia_ptr;
        ar & store_A_ptr;
        ar & store_B_ptr;
        ar & store_Q_ptr;
        ar & store_P_ptr;

    }
public:

    vector< vector<double > > store_lcia_ptr;
    vector< vector<double > > store_A_ptr;
    vector< vector<double > > store_B_ptr;
    vector< vector<double > > store_Q_ptr;
    vector< vector<double > > store_P_ptr;

    IterationsResults() {}

    IterationsResults(AppSettings settings,long impactCategoriesLength_,
            long A_size_, long B_size_, long Q_size_,
            long P_size_, int rows_) {
                    
        TwoDimVectorStore::initStore(store_lcia_ptr, rows_,impactCategoriesLength_);
        TwoDimVectorStore::initStore(store_A_ptr, rows_,A_size_);
        TwoDimVectorStore::initStore(store_B_ptr, rows_,B_size_);
        TwoDimVectorStore::initStore(store_Q_ptr, rows_, Q_size_);
        TwoDimVectorStore::initStore(store_P_ptr, rows_,P_size_);
    }
};


#endif /* IterationsResults_hpp */
