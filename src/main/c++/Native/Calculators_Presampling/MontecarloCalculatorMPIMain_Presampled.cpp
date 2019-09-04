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
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>


#include <stdio.h>
#include <vector>
#include <chrono>


#include "../Calculators/Models/AppSettings.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/TwoDimVectorStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../libs/libEigen.hpp"
#include "../Uncertainties/EntitySampler.hpp"
#include "../Calculators/Models/IterationsResults.hpp"
#include "../Utilities/ParameterUtils.h"

#include "SimulatorNative.hpp"
#include "MontecarloCalculatorMPI.hpp"
#include "SensitivityCalculator.hpp"


namespace mpi = boost::mpi;
using namespace std;

namespace boost
{
namespace mpi
{

template <>
struct is_mpi_datatype<std::string> : public mpl::true_
{
};
} // namespace mpi
} // namespace boost

int main(int argc, char **argv)
{

    /* INIT MPI and boostMPI*/
    MPI_Status status;
    MPI_Comm parentcomm;
    MPI_Init(NULL, NULL);
    MPI_Comm_get_parent(&parentcomm);
    int numprocs, myrank;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    boost::mpi::communicator boost_parent = boost::mpi::intercommunicator(parentcomm, boost::mpi::comm_take_ownership);
    boost::mpi::environment ev;
    boost::mpi::communicator world;

    /*send ACK from individual worker*/
    boost_parent.send(0, 8, 1);

    /*Block: receive settings object*/
    AppSettings settings;
    boost_parent.recv(0, 5, settings);

    /*Block: receive simulator data object*/
    CalculatorData calculatorData;
    boost_parent.recv(0, 30, calculatorData);

    /*loading samples indices*/
    calculatorData.loadSamplesIndices();

    cout<<"calculatorData.ASamplesIds_indices.size()"<<calculatorData.ASamplesIds_indices.size()<<endl;
        
    cout<<"calculatorData.BSamplesIds_indices.size()"<<calculatorData.BSamplesIds_indices.size()<<endl;


    IterationsResults *iterationsResults_input;
    if (world.rank() == 0)
    {
        iterationsResults_input = new IterationsResults(settings,
                                                        0,
                                                        (calculatorData).A_unc_size,
                                                        (calculatorData).B_unc_size,
                                                        (calculatorData).Q_size,
                                                        3,
                                                        settings.montecarlo_iterations);
    }

    /*Presample the input parameters, save the samples on Disk. if sensitivity ==true keep samples in memory*/
    //MontecarloCalculatorMPI::pre_sample(settings, &calculatorData, iterationsResults_input, world.size() - 1);

    IterationsResults *iterationsResults_output;

    if (world.rank() == 0)
    {
        unique_ptr<IterationsResults> pLarge(iterationsResults_input);
        pLarge.reset();

        iterationsResults_output = new IterationsResults(settings,
                                                         calculatorData.impactCategoriesLength,
                                                         0,
                                                         0,
                                                         0,
                                                         3,
                                                         settings.montecarlo_iterations);
    }

    double *scalars_PTR_g;

    cout<<"world.size() "<<world.size()<<endl;

    MontecarloCalculatorMPI::run_presampled(settings,
                                            &calculatorData,
                                            iterationsResults_output,
                                            world.size() - 1,
                                            scalars_PTR_g);

    if (world.rank() == 0)
    {
        /*Calculate Uncertainty*/
        MontecarloCalculatorMPI::uncertainty(true, settings, &calculatorData, iterationsResults_output);

        /*Send uncertainty back to root process which prints the report*/
        boost_parent.send(0, 11, calculatorData.impactStats);

        /*Run Sensitivity on Pre-ranked arrays*/
        // if (settings.Sensitivity)
        // {
        //     SensitivityCalculator sensitivityAnalyser = SensitivityCalculator(settings, &calculatorData, iterationsResults_output);
        //     sensitivityAnalyser.run_preRanked();
        // }

        delete iterationsResults_output;

        /*calculation is done successfully: Send Root process confirmation*/
        boost_parent.send(0, 4, 1);

        //MPI_Comm_free(&parentcomm);
        MPI_Finalize();
    }
    else
    {
        MPI_Finalize();
    }

    return 0;
}
