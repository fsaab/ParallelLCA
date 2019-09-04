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
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <boost/range/algorithm.hpp>
#include <tr1/unordered_map>

#include "../Calculators/Models/AppSettings.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
#include "../Calculators/Models/IterationsResults.hpp"
#include "../Utilities/FileUtils.hpp"
#include "../Factories/CalculatorDataFactory.hpp"
#include "../DAL/LCADBFactory.hpp"
#include "../Factories/GraphFactory.hpp"

using namespace std;

int main(int argc, char** argv) {

    int ierr;
    int initialized, finalized;
    MPI_Initialized(&initialized);

    setenv("OMPI_MCA_hwloc_base_use_hwthreads_as_cpus", "true", true);

    if (!initialized) {
        ierr = MPI_Init(NULL, NULL);
    }

    MPI_Status status;
    MPI_Comm workercomm;

    AppSettings settings{
       593976,
        1.0,
        "solve-local-sparse-umfpack",
        "testsampleQBAinV", "testsampleQBAinV", 973039,// MAKE THIS PARAMETER
        true, true, atoi(argv[1]), atoi(argv[2]), "./", 1, 1, false};


    LCADBFactory dbFactory(settings);
    LCADB* lcadb = dbFactory.create();

    dbFactory.update(lcadb,settings);

    CalculatorDataFactory simFactory(settings);

    GraphFactory graphFactory(settings);
   
    vector<long> v;
    v.push_back(settings.RootProcessId);

    GraphData* graph = graphFactory.create(lcadb, v);

    CalculatorData* calculatorData = simFactory.create(lcadb, graph);

    vector<double> demand_demandVectorArray = (*calculatorData).demand_demandVectorArray;
    long Plength = (*lcadb).parameters.size();

    int rows = settings.montecarlo_iterations;
    int impactCategoriesLength = (*calculatorData).impactCategoriesLength;
    int numworkers = (settings.Parallelism);

    char ** newargv = new char * [2]; //create new arg
    newargv[0] = (char *) malloc(sizeof (char)*50);
    newargv[1] = NULL;

    string s = "--use-hwthread-cpus";

    strncpy(newargv[0], s.c_str(), 50); //copy argv to newargv

    MPI_Comm_set_errhandler(MPI_COMM_SELF, MPI_ERRORS_RETURN);

    int res = MPI_Comm_spawn("./processes/SamplerMPIMainQBAInv", newargv /*MPI_ARGV_NULL*/, numworkers,
            MPI_INFO_NULL,
            0, MPI_COMM_SELF, &workercomm, MPI_ERRCODES_IGNORE);

    if (MPI_SUCCESS != res) {
        throw LCAException(300, "not enough resources. Possible reason recent calculation is not yet finished and taking all resources.");
    }

    boost::mpi::environment env;
    boost::mpi::communicator world;

    boost::mpi::communicator boost_worker = boost::mpi::intercommunicator(workercomm, boost::mpi::comm_take_ownership);

    int result_worker_ready;
    for (int i = 0; i < numworkers; i++) {
        boost_worker.recv(i, 8, result_worker_ready);
        cout << "proc " << i << " ready? " << result_worker_ready << endl;
    }

    for (int i = 0; i < numworkers; i++) {
        cout << "sending for settings...." << i << endl;
        boost_worker.send(i, 5, settings);
    }

    for (int i = 0; i < numworkers; i++) {
        boost_worker.recv(i, 8, result_worker_ready);
        cout << "proc " << i << " ready? " << result_worker_ready << endl;
    }

    for (int i = 0; i < numworkers; i++) {
        cout << "sending for CalculatorData...." << i << endl;
        boost_worker.send(i, 30, (*calculatorData));
    }

    int result;
    boost_worker.recv(0, 4, result);

    return 0;
}

