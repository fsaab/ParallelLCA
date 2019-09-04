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
#ifndef AggregatedSimulator_hpp
#define AggregatedSimulator_hpp

#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>
#include <chrono> // for high_resolution_high_resolution_clock
#include <boost/algorithm/string/join.hpp>
#include <tr1/unordered_map>
#include <sstream>
#include <algorithm>
#include <omp.h>
#include <string>
#include <vector>

#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/AppSettings.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/TwoDimVectorStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../Calculators/SimulatorNative.hpp"
#include "../Factories/TechnologyMatrixFactorySingle.hpp"
#include "../Factories/InterventionMatrixFactorySingle.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
#include "../libs/libDescriptive.hpp"
#include "../Upstream/UpstreamLCIACalculator.hpp"

using namespace std;

class AggregatedSampler
{
  public:
    static std::vector<std::string> split(std::string input, std::string sep)
    {

        vector<string> strs;
        boost::split(strs, input, boost::is_any_of(sep));
        return strs;
    }

    static vector<vector<double>> parseSamples(string path)
    {

        FileUtils fileUtils;

        vector<string> samplelines = fileUtils.readTextFromFile(path);
        long samplelines_size = samplelines.size();
        vector<vector<double>> samplesvals;
        samplesvals.resize(samplelines_size);

        for (int i_sampleline = 0; i_sampleline < samplelines_size; i_sampleline++)
        {
            string sampleline = samplelines[i_sampleline];
            vector<string> samples = split(sampleline, ",");
            for (auto &&sample : samples)
            {
                //if (sample != "")
                // {
                samplesvals[i_sampleline].push_back(std::stod(sample));
                // }
            }
        }
        return samplesvals;
    }

    static void presample_QBAInv(int start, int end, int iterationOffset,
                                 int slotsP,
                                 int slotsize,
                                 CalculatorData *calculatorData,
                                 AppSettings settings,
                                 double *scalars_PTR_g
                                 // ParameterTable ptbl /*IterationsResults* iterationsResults*/
    )
    {

        ParameterUtils<double> paramUtils;

        srand(time(NULL) + iterationOffset);
        unsigned seed = rand();

        //int(time(NULL)) ^ omp_get_thread_num(); //

        std::default_random_engine gen(seed);

        long ElementaryFlowsIndexLength = (*calculatorData).elementaryFlowsLength;
        long ProcessesIndexLength = (*calculatorData).processesLength;
        long ImpactCategoryIndexLength = (*calculatorData).impactCategoriesLength;

        long Plength = 3; //ptbl.parameters.size();
        int iter = start;

        EntitySampler<double> entitySampler;

        for (auto &&cell : (*calculatorData).A_exchanges)
        {
            paramUtils.reloadExchangesDistribution(cell);
        }

        for (auto &&cell : (*calculatorData).B_exchanges)
        {
            paramUtils.reloadExchangesDistribution(cell);
        }

        /**read file for corresponding iteration offset*/

        FileUtils fileUtils;
        std::stringstream ss_Asamples_path;
        ss_Asamples_path << "data/Samples/" << settings.montecarlo_iterations << "/ASamples/" << iterationOffset << ".txt";
        cout << "reading A" << ss_Asamples_path.str() << endl;

        vector<string> Asamplelines = fileUtils.readTextFromFile(ss_Asamples_path.str());

        vector<vector<double>> Asamplesvals;
        Asamplesvals.resize(Asamplelines.size());
        cout << "Asamplelines.size()" << Asamplelines.size() << endl;

        for (int i_sampleline = 0; i_sampleline < Asamplelines.size(); i_sampleline++)
        {
            string sampleline = Asamplelines[i_sampleline];

            vector<string> samples = split(sampleline, ",");

            for (auto &&sample : samples)
            {
                if (sample != "")
                {
                    Asamplesvals[i_sampleline].push_back(std::stod(sample));
                }
            }
        }

        cout << "samples parsed for" << iterationOffset << endl;

        std::stringstream ss_B_samples_path;
        ss_B_samples_path << "data/Samples/" << settings.montecarlo_iterations << "/BSamples/" << iterationOffset << ".txt";
        cout << "reading " << ss_B_samples_path.str() << endl;

        vector<string> Bsamplelines = fileUtils.readTextFromFile(ss_B_samples_path.str());

        vector<vector<double>> Bsamplesvals;
        Bsamplesvals.resize(Bsamplelines.size());

        for (int i_sampleline = 0; i_sampleline < Bsamplelines.size(); i_sampleline++)
        {
            string sampleline = Bsamplelines[i_sampleline];

            vector<string> samples = split(sampleline, ",");

            for (auto &&sample : samples)
            {
                if (sample != "")
                {
                    Bsamplesvals[i_sampleline].push_back(std::stod(sample));
                }
            }
        }

        cout << "samples B parsed for" << iterationOffset << endl;

        std::stringstream path1;
        path1 << settings.RootPath << "data/Scores/" << settings.montecarlo_iterations;
        fileUtils.newfolder(path1.str());

        std::stringstream path2;
        path2 << settings.RootPath << "data/Scores/" << settings.montecarlo_iterations << "/" << settings.ImpactMethodId;

        fileUtils.newfolder(path2.str());

        std::stringstream path3;
        path3 << settings.RootPath << "data/Scores/" << settings.montecarlo_iterations << "/" << settings.ImpactMethodId << "/" << iterationOffset;
        fileUtils.newfolder(path3.str());

        /**END   read file fo corresponding iteration offset*/

        while (iter <= end)
        {

            cout<< "iter "<<iter<<endl;

            //map<string, expression_t > interpreters;
            //
            //map<std::pair<string, string>, double > parameters_sim;
            //
            //interpreters = entitySampler.sampleParameterTable(ptbl, gen, parameters_sim);

            double *dataP_ptr = new double[Plength];
            //int parami = 0;
            //for (map< std::pair<string, string>, double >::iterator it = parameters_sim.begin(); it != parameters_sim.end(); ++it) {
            //
            //dataP_ptr[parami] = it->second;
            //parami++;
            //}

            long Alength = (*calculatorData).A_size;
            long *rowsA_ptr = new long[Alength];
            long *colsA_ptr = new long[Alength];
            double *dataA_ptr = new double[Alength];

            vector<double> dataA_ptr_unc;


            int i = 0;
            for (auto &&cell : (*calculatorData).A_exchanges)
            {

                ExchangeItem ent = cell;
                rowsA_ptr[i] = ent.i;
                colsA_ptr[i] = ent.j;

                if (!ent.exch.isBackground || ent.exch.uncertaintyType == 0)
                {
                    dataA_ptr[i] = entitySampler.getCellValue(ent.exch);
                }
                else
                {
                    double v = Asamplesvals[(iter - iterationOffset)][(*calculatorData).ASamplesIds_indices[ent.exch.exchangeId]];

                    dataA_ptr[i] = v;
                }

                if (ent.exch.uncertaintyType != 0)
                {

                    dataA_ptr_unc.push_back(dataA_ptr[i]);
                }
                i++;
            }

            long Blength = (*calculatorData).B_size;
            long *rowsB_ptr = new long[Blength];
            long *colsB_ptr = new long[Blength];
            double *dataB_ptr = new double[Blength];
            vector<double> dataB_ptr_unc;

            i = 0;

            for (auto &&cell : (*calculatorData).B_exchanges)
            {
                ExchangeItem ent = cell;

                rowsB_ptr[i] = ent.i;
                colsB_ptr[i] = ent.j;

                if (!ent.exch.isBackground || ent.exch.uncertaintyType == 0)
                {
                    dataB_ptr[i] = entitySampler.getCellValue(ent.exch);
                }
                else
                {
                    double v = Bsamplesvals[(iter - iterationOffset)][(*calculatorData).BSamplesIds_indices[ent.exch.exchangeId]];

                    dataB_ptr[i] = v;
                }

                if (ent.exch.uncertaintyType != 0)
                {
                    dataB_ptr_unc.push_back(dataB_ptr[i] /*scalars_PTR[ent.j]*/);
                }

                i++;
            }

            long Qlength = (*calculatorData).Q_size;
            long *rowsQ_ptr = new long[Qlength];
            long *colsQ_ptr = new long[Qlength];
            double *dataQ_ptr = new double[Qlength];


            i = 0;
            for (auto &&cell : (*calculatorData).Q_cells)
            {

                CalcImpactFactorItem ent = cell;
                rowsQ_ptr[i] = ent.i;
                colsQ_ptr[i] = ent.j;
                dataQ_ptr[i] = entitySampler.getImpactCellValue(ent.imf
                                                                //,paramUtils.getIterpreter(cell.imf.id, interpreters, "GLOBAL")
                );

                i++;
            }


            libQBAInvEigen lib;

            Eigen::MatrixXd QBAinv;
            QBAinv = lib.calculateByTransposeSystemBICGSTAB_Simple(
                Alength, rowsA_ptr, colsA_ptr, dataA_ptr,
                Blength, rowsB_ptr, colsB_ptr, dataB_ptr,
                Qlength, rowsQ_ptr, colsQ_ptr, dataQ_ptr,
                settings,
                calculatorData);


            /*writing samples to file*/

            for (int j = 0; j < QBAinv.cols(); j++)
            {

                std::stringstream ss;
                std::stringstream path;
                path << settings.RootPath << "data/Scores/" << settings.montecarlo_iterations << "/" << settings.ImpactMethodId << "/" << iterationOffset
                     << "/" << (*calculatorData).lcaIndexes.ProcessesIndex[j] << ".txt";

                vector<string> elems;
                for (int i = 0; i < QBAinv.rows(); i++)
                {
                    std::stringstream ss;
                    ss << QBAinv.coeff(i, j);
                    elems.push_back(ss.str());

                    //if(i<10) cout<<QBAinv.coeff(i, j)<<",";
                }
                //cout<<endl;

                std::string joinedString = boost::algorithm::join(elems, ",");

                ss << joinedString << endl;

                fileUtils.appendTextToFile(ss.str(), path.str());
            }

            cout << "Solved, written" << endl;

            /* save the data*/

            delete[] rowsA_ptr;
            delete[] colsA_ptr;
            delete[] dataA_ptr;
            delete[] rowsB_ptr;
            delete[] colsB_ptr;
            delete[] dataB_ptr;
            delete[] rowsQ_ptr;
            delete[] colsQ_ptr;
            delete[] dataQ_ptr;
            delete[] dataP_ptr;

            iter++;
        }

        cout << "done slot " << iterationOffset << endl;
    }
};

#endif /* AggregatedSimulator_hpp */
