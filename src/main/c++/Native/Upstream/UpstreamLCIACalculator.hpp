//
//  UpstreamLCIA.hpp
//  LCA.Kernel
//
//  Created by francois saab on 2/26/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef UpstreamLCIACalculator_hpp
#define UpstreamLCIACalculator_hpp

#include <stdio.h>
#include <omp.h>
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Dense>
/*#include <Eigen>*/
//#include "../libs/libMUMPS.hpp"
//#include <Eigen/UmfPackSupport>
using Eigen::BiCGSTAB;
using Eigen::SparseMatrix;
using Eigen::VectorXd;
typedef Eigen::Triplet<double> Triplet;
typedef Eigen::SparseMatrix<double> SMatrix;

#include "../Messaging/LCAException.hpp"
#include "LinearGraphUtil.hpp"
#include "../Reports/ContributionAnalysisReport.hpp"
#include "../Graph/GraphData.h"
#include "../libs/libQBAInvEigen.hpp"
//#include "../Optimisations/libQBAInv.hpp"
#include "../Factories/TechnologyMatrixFactorySingle.hpp"
#include "../Factories/InterventionMatrixFactorySingle.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
#include "../Calculators/Models/AppSettings.hpp"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../LCAModels/LCAIndexes.hpp"

class UpstreamLCIACalculator
{
    AppSettings settings;
    CalculatorData *calculatorData;
    LCADB *lcadb;
    GraphData *graph;

public:
    UpstreamLCIACalculator(
        AppSettings _settings,
        CalculatorData *_calculatorData,
        LCADB *_lcadb,
        GraphData *_graph) : settings(_settings), calculatorData(_calculatorData), lcadb(_lcadb), graph(_graph)
    {
    }

    UpstreamLCIACalculator()
    {
    }

    void run(double *lciaR)
    {

        auto strtmt = std::chrono::high_resolution_clock::now();

        Eigen::MatrixXd QBAinv = libQBAInvEigen::calculateByTransposeSystemBICGSTAB(calculatorData, settings);

        auto finishmt = std::chrono::high_resolution_clock::now();
        auto elapsedmt = std::chrono::duration_cast<std::chrono::microseconds>(finishmt - strtmt);
        std::cout << "mt/c++," << settings.ProjectId << "," << elapsedmt.count() << std::endl;

        auto strtlg = std::chrono::high_resolution_clock::now();

        LinearGraphUtils linearGraphUtils(lcadb, graph, settings);
        vector<ContributionTreeNode> tree = linearGraphUtils.buildLinearTree(settings.RootProcessId, calculatorData);

        auto finishlg = std::chrono::high_resolution_clock::now();
        auto elapsedlg = std::chrono::duration_cast<std::chrono::microseconds>(finishlg - strtlg);
        std::cout << "lg/c++," << settings.ProjectId << "," << elapsedlg.count() << std::endl;

        auto strtshare = std::chrono::high_resolution_clock::now();
        ContributionAnalysisReport report(settings, calculatorData, lcadb);
        SMatrix ASparse = TechnologyMatrixFactorySingle::build(settings, calculatorData);

        report.printLCIA(graph, ASparse, lciaR, QBAinv, tree);

        auto finishshare = std::chrono::high_resolution_clock::now();
        auto elapsedshare = std::chrono::duration_cast<std::chrono::microseconds>(finishshare - strtshare);
        std::cout << "share/c++," << settings.ProjectId << "," << elapsedshare.count() << std::endl;
       
    }
};

#endif /* UpstreamLCIACalculator_hpp */
