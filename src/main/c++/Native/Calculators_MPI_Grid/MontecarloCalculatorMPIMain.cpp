//  LCA.Kernel
//  Thesis At Ecole De technolgie Superieure De Montreal : www.etsmtl.ca
//  Created by francois saab on 2/18/18.
//  saab.francois@gmail.com
//  francois.saab.1@ens.etsmtl.ca

#include <cstdlib>
#include <iostream>
#include <boost/mpi.hpp>
#include <boost/serialization/vector.hpp>
#include <stdio.h>
#include <vector>
#include <chrono>
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/AppSettings.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/TwoDimVectorStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "./SimulatorNative.hpp"
#include "../Factories/TechnologyMatrixFactorySingle.hpp"
#include "../Factories/InterventionMatrixFactorySingle.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
#include "../libs/libDescriptive.hpp"
#include "../DAL/LCADBFactory.hpp"
#include "../Factories/GraphFactory.hpp"
#include "../Graph/GraphData.h"
#include "../Factories/CalculatorDataFactory.hpp"
#include "MontecarloCalculatorMPI.hpp"
#include "../Calculators/SensitivityCalculator.hpp"
#include <boost/mpi.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
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
    //boost_parent.send(0, 8, 1);

    /*Block: receive settings object*/
    AppSettings settings;
    boost_parent.recv(0, 5, settings);

    /*Block: receive simulator data object*/
    CalculatorData calculatorData;

    boost_parent.recv(0, 30, calculatorData);

    double *scalars_PTR_g;

    if (world.rank() == 0)
    {
        FileUtils fileUtils;
        string slotPath = settings.RootPath + "data/calculations/" + settings.CalculationId + "/Iters/";
        fileUtils.newfolder(slotPath);
        slotPath = settings.RootPath + "data/calculations/" + settings.CalculationId + "/Iters/LCIA/";
        fileUtils.newfolder(slotPath);

        slotPath = settings.RootPath + "data/calculations/" + settings.CalculationId + "/Iters/Samples/";
        fileUtils.newfolder(slotPath);
    }

    long parallelism = settings.Parallelism;                        //settings.Parallelism;//5
    long threadSize = settings.montecarlo_iterations / parallelism; //1000/5=200
    long slotsize = 200;                                            //memory=slotsize*parallelism
    slotsize = (slotsize > threadSize ? threadSize : slotsize);
    long slotnum = settings.montecarlo_iterations / slotsize; //1000/100=10
    long dataslotsize_samples = 1000;
    long dataslotsize_output=5;

    /*Run montecarlo iterations on all processes*/
    MontecarloCalculatorMPI::run(settings, &calculatorData, world.size(), slotsize, dataslotsize_samples,dataslotsize_output);

    /*calculate uncertainty and sensitivity on the master process only*/
    if (world.rank() == 0)
    {

        for (int i = 1; i < parallelism; i++)
        {
            int res;
            world.recv(i, 1, res);
        }

        /*calculate uncertainty  on the master process only*/
        MontecarloCalculatorMPI::uncertainty(true, settings, &calculatorData,
                                             threadSize, parallelism, slotnum / parallelism,
                                             ceil((float)calculatorData.impactCategoriesLength / dataslotsize_output),
                                             dataslotsize_output, calculatorData.impactCategoriesLength);

        /*Send the results of the montecarlo simulation back to the ROOT master process*/
        boost_parent.send(0, 11, calculatorData.impactStats);

        /*calculate sensitivity on the master process only*/
        // if (settings.Sensitivity)
        // {
        //     SensitivityCalculator sensitivityAnalyser = SensitivityCalculator(settings, &calculatorData, iterationsResults);
        //     sensitivityAnalyser.run();
        // }

        /*Send Root process that calculation is done successfully*/
        boost_parent.send(0, 4, 1);

        MPI_Finalize();
    }
    else
    {
        MPI_Finalize();
    }

    return 0;
}
