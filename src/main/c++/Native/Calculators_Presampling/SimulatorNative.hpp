//
//  SimulatorNative.hpp
//  LCA.Kernel
//
//  Created by francois saab on 2/18/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef SimulatorNative_hpp
#define SimulatorNative_hpp


#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>
#include <chrono>

#include <sstream>
#include <algorithm>
#include <omp.h>
#include <string>
#include <vector>

#include <tr1/unordered_map>
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>

//#include <Eigen/UmfPackSupport>
using Eigen::BiCGSTAB;
using Eigen::ConjugateGradient;
using Eigen::SparseMatrix;
using Eigen::VectorXd;

typedef Eigen::Triplet<double> Triplet;
typedef Eigen::SparseMatrix<double> SMatrix;

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


using namespace std;

class SimulatorNative
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
           
            for ( int c=0; c< samples.size();c++)
            {
                //if (sample != "")
                // {
                samplesvals[i_sampleline].push_back(std::stod(samples[c]));
                // }
            }
        }
        return samplesvals;
    }

    static IterationsResults *iterate_presampling(int start, int end, int iterationOffset,
                                                  int slotsP,
                                                  int slotsize,
                                                  CalculatorData *calculatorData,
                                                  AppSettings settings, //,
                                                  double *scalars_PTR_g
                                                  // ,ParameterTable ptbl
    )
    {

        cout<<"iterationOffset "<<iterationOffset<<endl;
        //ParameterUtils<double> paramUtils;

        /*Initialise random number geenrator*/
        srand(time(NULL) + iterationOffset);
        unsigned seed = rand();
        std::default_random_engine gen(seed);

        /*Cache some variables*/
        long ElementaryFlowsIndexLength = (*calculatorData).elementaryFlowsLength;
        long ProcessesIndexLength = (*calculatorData).processesLength;
        long ImpactCategoryIndexLength = (*calculatorData).impactCategoriesLength;
        // long Plength = ptbl.parameters.size();

        int iter = start;

        IterationsResults *iterationResults_i = new IterationsResults(settings,
                                                                      (*calculatorData).impactCategoriesLength,
                                                                      (*calculatorData).A_unc_size,
                                                                      (*calculatorData).B_unc_size,
                                                                      (*calculatorData).Q_size,
                                                                      0,
                                                                      slotsize);
        
        std::stringstream ss_Asamples_path;
        ss_Asamples_path << "data/Samples/" << settings.montecarlo_iterations << "/ASamples/" << iterationOffset << ".txt";
        vector<vector<double>> Asamplesvals = parseSamples(ss_Asamples_path.str());

        cout<<"Asamplesvals "<<Asamplesvals[0].size()<<endl;

        std::stringstream ss_B_samples_path;
        ss_B_samples_path << "data/Samples/" << settings.montecarlo_iterations << "/BSamples/" << iterationOffset << ".txt";
        vector<vector<double>> Bsamplesvals = parseSamples(ss_B_samples_path.str());

        cout<<"Bsamplesvals "<<Bsamplesvals[0].size()<<endl;

        EntitySampler<double> entitySampler;

        while (iter <= end)
        {

            //map<string, expression_t > interpreters;
            //map<std::pair<string, string>, double > parameters_sim;
            //interpreters = entitySampler.sampleParameterTable(ptbl, gen, parameters_sim);

            //double *dataP_ptr = new double[Plength];

            // int parami = 0;
            // for (map< std::pair<string, string>, double >::iterator it = parameters_sim.begin(); it != parameters_sim.end(); ++it) {
            // dataP_ptr[parami] = it->second;
            // parami++;
            // }

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
                    dataA_ptr[i] = entitySampler.getCellValue(ent.exch
                          //,paramUtils.getIterpreter(cell.exch.exchangeId, interpreters, "PROCESS")
                    );
                }
                else
                {
                    dataA_ptr[i] =Asamplesvals[(iter - iterationOffset)][(*calculatorData).ASamplesIds_indices[ent.exch.exchangeId]];

                //    if(Asamplesvals[(iter - iterationOffset)][(*calculatorData).ASamplesIds_indices[ent.exch.exchangeId]]
                //    ==entitySampler.getCellValue(ent.exch))  cout<<Asamplesvals[(iter - iterationOffset)][(*calculatorData).ASamplesIds_indices[ent.exch.exchangeId]]
                //    <<" , "<<entitySampler.getCellValue(ent.exch)<<" ; ";
                    
                    //  entitySampler.getCellValue(ent.exch
                    //       /*,paramUtils.getIterpreter(cell.exch.exchangeId, interpreters, "PROCESS")*/);

                    
                    //Asamplesvals[(iter - iterationOffset)][(*calculatorData).ASamplesIds_indices[ent.exch.exchangeId]];

                    
                    
                    //cout<<Asamplesvals[(iter - iterationOffset)][(*calculatorData).ASamplesIds_indices[ent.exch.exchangeId]]<<" , ";
                
                }
                // if (ent.exch.uncertaintyType != 0) {
                //     dataA_ptr_unc.push_back(dataA_ptr[i]);
                // }
                i++;
            }

            double *demand = &((*calculatorData).demand_demandVectorArray)[0];
            cout<<"solving.........."<<endl;
            libEigen lib;
            double *scalars_PTR = lib.solve_bicgstab(ProcessesIndexLength,
                                                     Alength,
                                                     rowsA_ptr,
                                                     colsA_ptr,
                                                     dataA_ptr,
                                                     demand);
            cout<<"solved.........."<<endl;

            //scalars_PTR_g, ILU, b, sol);

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

                if (!ent.exch.isBackground ||  ent.exch.uncertaintyType == 0)
                {
                    dataB_ptr[i] = entitySampler.getCellValue(ent.exch
                                                              //,paramUtils.getIterpreter(cell.exch.exchangeId, interpreters, "PROCESS")
                    );
                }
                else
                {

                    dataB_ptr[i] = Bsamplesvals[(iter - iterationOffset)][(*calculatorData).BSamplesIds_indices[ent.exch.exchangeId]];

                    //cout<<Bsamplesvals[(iter - iterationOffset)][(*calculatorData).BSamplesIds_indices[ent.exch.exchangeId]]<<" , ";
                }

                // if (ent.exch.uncertaintyType != 0) {
                //     dataB_ptr_unc.push_back(dataB_ptr[i]/*scalars_PTR[ent.j]*/);
                // }

                i++;
            }

            double *lciR = lib.MV_mult(ElementaryFlowsIndexLength,
                                       ProcessesIndexLength,
                                       Blength,
                                       rowsB_ptr,
                                       colsB_ptr,
                                       dataB_ptr,
                                       ProcessesIndexLength,
                                       scalars_PTR);

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

            double *lciaR = lib.MV_mult(ImpactCategoryIndexLength,
                                        ElementaryFlowsIndexLength,
                                        Qlength,
                                        rowsQ_ptr,
                                        colsQ_ptr,
                                        dataQ_ptr,
                                        ElementaryFlowsIndexLength,
                                        lciR);

            TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_lcia_ptr, iter - iterationOffset, lciaR, ImpactCategoryIndexLength);

            // if (dataA_ptr_unc.size() > 0)
            //     TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_A_ptr, iter - iterationOffset, &dataA_ptr_unc[0], (*calculatorData).A_unc_size);

            // if (dataB_ptr_unc.size() > 0)
            //     TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_B_ptr, iter - iterationOffset, &dataB_ptr_unc[0], (*calculatorData).B_unc_size);

            // TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_Q_ptr, iter - iterationOffset, dataQ_ptr, Qlength);

            // TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_P_ptr, iter - iterationOffset, dataP_ptr, Plength);

            delete[] rowsA_ptr;
            delete[] colsA_ptr;
            delete[] dataA_ptr;
            delete[] rowsB_ptr;
            delete[] colsB_ptr;
            delete[] dataB_ptr;
            delete[] rowsQ_ptr;
            delete[] colsQ_ptr;
            delete[] dataQ_ptr;
            //delete[] dataP_ptr;
            delete[] scalars_PTR;
            delete[] lciaR;
            delete[] lciR;

            iter++;
        }

        return iterationResults_i;
    }
};

#endif /* SimulatorNative_hpp */
