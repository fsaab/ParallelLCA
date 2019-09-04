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

#ifndef LCADBFactory_hpp
#define LCADBFactory_hpp

#include <stdio.h>
#include <vector>
#include "../Utilities/FileUtils.hpp"
#include "../LCAModels/Exchange.hpp"
#include "ExchangeObj.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/ExchangeCell.hpp"
#include <chrono>
#include <sstream>
#include <iostream>
#include "../Calculators/Models/AppSettings.hpp"
#include <vector>
#include "../LCAModels/ExchangeLink.hpp"
#include "../LCAModels/Exchange.hpp"
#include "../Graph/LCAEdgeCompact.hpp"
#include <omp.h>
#include <tr1/unordered_map>
#include "../LCAModels/EdgeShare.hpp"
#include "LCADB.hpp"
#include "../Utilities/LCAUtils.hpp"
#include <cmath>
#include <queue>
#include "../LCAModels/ContributionTReeNode.hpp"
#include "../Graph/GraphData.h"
#include "../Calculators/Models/CalculatorData.h"
#include "../Utilities/ParameterUtils.h"
#include "LCADBUtils.h"

class LCADBFactory {
private:

public:

    AppSettings settings;

    LCADBFactory(AppSettings settings_) : settings(settings_) {
    }

    LCADB* create() {

        LCADB * lcadb = new LCADB;

        LCADBUtils lcadbutils(lcadb, settings);

        string processes_path = settings.RootPath + "data/DBTemplates/" +to_string(settings.DBTemplateId) +"/processes.spark";

        lcadbutils.loadProcesses(settings, processes_path, false);

        string flows_path = settings.RootPath + "data/DBTemplates/" +to_string(settings.DBTemplateId) + "/flows.spark";

        lcadbutils.loadFlows(settings, flows_path, false);

        string impactscats_path = settings.RootPath + "data/DBTemplates/" + to_string(settings.DBTemplateId) + "/impact_categories.spark";

        lcadbutils.loadImpactCategories(settings, impactscats_path, false);

        string uom_path = settings.RootPath + "data/DBTemplates/" +to_string(settings.DBTemplateId) + "/units.spark";

        lcadbutils.loadUnitOfMeasurements(settings, uom_path, false);

        string exchanges_path = settings.RootPath + "data/DBTemplates/" + to_string(settings.DBTemplateId) + "/exchanges.spark";

        lcadbutils.loadExchanges(settings, exchanges_path, false);

        string impactfactors_path = settings.RootPath + "data/DBTemplates/" + to_string(settings.DBTemplateId) + "/impact_factors.spark";

        lcadbutils.loadImpactFactors(settings, impactfactors_path, false);

        lcadbutils.loadProcLocationCodes(settings);

        string parameters_path = settings.RootPath + "data/DBTemplates/" + to_string(settings.DBTemplateId) + "/parameters.spark";

        lcadbutils.loadParameters(settings, parameters_path, false);

        

        return lcadb;
    }

    void update(LCADB* lcadb, AppSettings settings) {

        LCADBUtils lcadbutils(lcadb, settings);

        string prefixPath = settings.RootPath + "data/projects/" +
                to_string(settings.ProjectId) + "/DBVersions/" +
                to_string(settings.Version) + "/deltas";

        string processes_path = prefixPath + "/ProcessesDiff.txt";
        lcadbutils.loadProcesses(settings, processes_path, true);

        string flows_path = prefixPath + "/FlowsDiff.txt";
        lcadbutils.loadFlows(settings, flows_path, true);

        string units_path = prefixPath + "/UnitsDiff.spark";
        lcadbutils.loadUnitOfMeasurements(settings, units_path, true);

        string ic_path = prefixPath + "/ImpactCategoriesDiff.txt";
        lcadbutils.loadImpactCategories(settings, ic_path, true);

        string params_path = prefixPath + "/ParametersDiff.txt";
        lcadbutils.loadParameters(settings, params_path, true);

        string if_path = prefixPath + "/ImpactFactorsDiff.txt";
        lcadbutils.loadImpactFactors(settings, if_path, true);

        string e_path = prefixPath + "/ExchangesDiff.txt";
        lcadbutils.loadExchanges(settings, e_path, true);

    }

};

#endif /* LCADBFactory_hpp */
