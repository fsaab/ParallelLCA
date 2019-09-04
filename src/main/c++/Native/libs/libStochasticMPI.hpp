//
//  libStochastic.hpp
//  LCA.Kernel
//
//  Created by francois saab on 2/26/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef libStochastic_hpp
#define libStochastic_hpp

#include <stdio.h>
#include <iostream>
#include <omp.h>
#include <boost/mpi.hpp>
#include <boost/serialization/vector.hpp>

#include "../Calculators/Models/AppSettings.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../Factories/TechnologyMatrixFactorySingle.hpp"
#include "../Factories/InterventionMatrixFactorySingle.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
#include "../Calculators/MontecarloCalculator.hpp"
//#include <chrono>  // for high_resolution_clock
#include "../Calculators/Models/IterationsResults.hpp"
#include "../Calculators/SensitivityCalculator.hpp"
#include "../Utilities/FileUtils.hpp"

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

class libStochasticMPI
{
  public:
    bool run(int RootProcessId,
             double OutputQunatity,
             string SolvingMethod,
             string SystemId,
             string CalculationId,
             int ImpactMethodId,
             int lcia,
             int ismontecarlo,
             int montecarlo_iterations,
             int Parallelism,
             string RootPath,
             long ProjectId,
             int Version,
             LCADB *lcadb,
             bool sensitivity,
             CalculatorData *calculatorData)
    {

        int ierr;
        int initialized, finalized;
        MPI_Initialized(&initialized);

        setenv("OMPI_MCA_hwloc_base_use_hwthreads_as_cpus", "true", true);

        if (!initialized)
        {
            ierr = MPI_Init(NULL, NULL);
        }

        MPI_Status status;
        MPI_Comm workercomm;

        AppSettings settings{
            RootProcessId,
            OutputQunatity,
            SolvingMethod,
            SystemId,
            CalculationId,
            ImpactMethodId,
            lcia == 1,
            ismontecarlo == 1,
            montecarlo_iterations,
            Parallelism,
            RootPath,
            ProjectId,
            Version, sensitivity};

        vector<double> demand_demandVectorArray = (*calculatorData).demand_demandVectorArray;

        //caching some variables
        //long Plength = (*lcadb).ptbl.parameters.size();
        int rows = settings.montecarlo_iterations;
        int impactCategoriesLength = (*calculatorData).impactCategoriesLength;
        int numworkers = settings.Parallelism;

        //MPI_Comm_set_errhandler(MPI_COMM_SELF, MPI_ERRORS_RETURN);

        /* to enable hyperthreading*/
        // char **newargv = new char *[2];
        // newargv[0] = (char *)malloc(sizeof(char) * 50);
        // newargv[1] = NULL;
        // string s = "--use-hwthread-cpus";
        // strncpy(newargv[0], s.c_str(), 50); //copy argv to newargv

        /*Spawn processes for montecarlo simulation*/
        int res = MPI_Comm_spawn("./processes/montecarlo", MPI_ARGV_NULL, numworkers,
                                 MPI_INFO_NULL,
                                 0, MPI_COMM_SELF, &workercomm, MPI_ERRCODES_IGNORE);

        if (MPI_SUCCESS != res)
        {
            throw LCAException(300, "not enough resources. Possible reason recent calculation is not yet finished and taking all resources.");
        }

        boost::mpi::environment env;
        boost::mpi::communicator world;
        boost::mpi::communicator boost_worker = boost::mpi::intercommunicator(workercomm, boost::mpi::comm_take_ownership);

        /* receive ack from workers that they are all ready. this is Blocking*/
        int result_worker_ready;
        for (int i = 0; i < numworkers; i++)
        {
            boost_worker.recv(i, 8, result_worker_ready);
            //cout << "worker " << i << " is ready" << endl;
        }

        /*send settings object to workers*/
        for (int i = 0; i < numworkers; i++)
        {
            boost_worker.send(i, 5, settings);
        }
        // boost_worker.send(0, 5, settings);

        /* receive ack from workers that they are all ready. this is Blocking*/
         result_worker_ready;
        for (int i = 0; i < numworkers; i++)
        {
            boost_worker.recv(i, 8, result_worker_ready);
            //cout << "worker " << i << " is ready" << endl;
        }


        CalculatorData calcData = (*calculatorData);
        /*send SimulatorData to workers*/
        for (int i = 0; i < numworkers; i++)
        {
            //cout << "sending SimulatorData, to worker " << i << endl;
            boost_worker.send(i, 30, calcData);
            //cout << "sent SimulatorData to worker " << i << endl;
        }

        /*recieve results of montecarlo simulation as  vector<ImpactStat>*/
        vector<ImpactStat> impactStats;
        boost_worker.recv(0, 11, impactStats);

        (*calculatorData).impactStats = impactStats;

        //cout << "recieveed results of montecarlo, size " << impactStats.size() << endl;

        /*Blocking: Waiting for sensitivty analysis to finsih, receive ACK that calculation is complete*/
        int result;
        boost_worker.recv(0, 4, result);

        return result == 1;
    }
};
#endif /* libStochastic_hpp */
