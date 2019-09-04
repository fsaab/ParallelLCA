/*
 * Copyright (C) 2018 ubuntu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   MontecarloCalculatorMPIMain.cpp
 * Author: ubuntu
 *
 * Created on June 19, 2018, 3:22 PM
 */

#include <cstdlib>
#include <iostream>
#include <boost/mpi.hpp>
#include <boost/serialization/vector.hpp>


using namespace std;
#include <stdio.h>
#include <vector>
#include <boost/mpi.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <chrono>


#include "../Calculators/Models/AppSettings.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
#include "../Calculators/Models/IterationsResults.hpp"
#include "../Utilities/FileUtils.hpp"
#include "../Factories/CalculatorDataFactory.hpp"
#include "../DAL/LCADBFactory.hpp"

#include "MontecarloCalculatorMPIQBAInv.hpp"

namespace mpi = boost::mpi;


/*
 * 
 */

//#include "mpi.h"

//
namespace boost {
    namespace mpi {

        template<> struct is_mpi_datatype<std::string> : public mpl::true_ {
        };
    }
}

int main(int argc, char** argv) {

    MPI_Status status;
    MPI_Comm parentcomm;

    MPI_Init(NULL, NULL);

    MPI_Comm_get_parent(&parentcomm);
    int numprocs, myrank;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    boost::mpi::communicator boost_parent = boost::mpi::intercommunicator(parentcomm, boost::mpi::comm_take_ownership);

    CalculatorData calculatorData;

    boost::mpi::environment ev; //set number of processes

    boost::mpi::communicator world;

    boost_parent.send(0, 8, 1);

    AppSettings settings;
    
    boost_parent.recv(0, 5, settings);

    boost_parent.send(0, 8, 1);
   
    boost_parent.recv(0, 30, calculatorData);
    
     /**----Presampling----*/
    calculatorData.loadSamplesIndices();
    
    double * scalars_PTR_g;

    MontecarloCalculatorMPIQBAInv::run_presample_QBAinv(settings, &calculatorData,world.size() - 1, scalars_PTR_g);

    if (world.rank() == 0) {

        boost_parent.send(0, 4, 1);

        MPI_Finalize();
    } else {

        MPI_Finalize();
    }

    return 0;
}

