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

#ifndef LCADBUTILS_H
#define LCADBUTILS_H

#include <stdio.h>
#include <vector>
#include "../Utilities/FileUtils.hpp"
#include "../LCAModels/Exchange.hpp"
#include "ExchangeObj.hpp"
#include "../LCAModels/CalcImpactFactor.hpp"
#include "CalcImpactFactorObj.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/ExchangeCell.hpp"
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include <sstream>
#include <time.h>
#include <iostream>
#include "LCADB.hpp"
#include "../LCAModels/Process.hpp"
#include "ProcessObj.hpp"
#include <tr1/unordered_map>
#include <boost/range/algorithm.hpp>
#include <string>
#include "FlowObj.hpp"
#include "UnitOfMeasurementObj.hpp"
#include "../Utilities/FileUtils.hpp"
#include "CalcParameterObj.hpp"
#include <omp.h>

class LCADBUtils
{
  private:
    string sep;

  public:
    LCADB *lcadb;
    AppSettings settings;

    LCADBUtils(LCADB *lcadb_, AppSettings settings_) : lcadb(lcadb_), settings(settings_)
    {

        sep = char(29);
    }

    bool stob(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        std::istringstream is(str);
        bool b;
        is >> std::boolalpha >> b;
        return b;
    }

    std::vector<std::string> split(std::string input, string sep)
    {

        vector<string> strs;
        boost::split(strs, input, boost::is_any_of(sep));

        return strs;
    }

    void loadProcesses(vector<string> lines_str)
    {

        for (string str : lines_str)
        {

            Process p = ProcessObj::parse(str, sep);

            (*lcadb).processes[p.ProcId] = p;
        }
    }

    void loadProcesses(AppSettings settings, string path, bool delta)
    {

        FileUtils fileUtils;

        vector<string> lines_str = fileUtils.readTextFromFile(path);

        for (auto &&str : lines_str)
        {

            std::size_t pos = (delta ? str.find(sep) : -1);
            string entityString = (delta ? str.substr(pos + 1) : str);
            string cmd = (delta ? str.substr(0, pos) : "");

            Process p = ProcessObj::parse(entityString, sep);

            if (!delta || cmd != "Delete")
            {
                (*lcadb).processes[p.ProcId] = p;
            }
            else
            {
                deleteProcess(p.ProcId);
            }
        }
    }

    void deleteProcess(long id)
    {
        (*lcadb).processes.erase((*lcadb).processes.find(id));
    }

    void loadFlows(vector<string> lines_str)
    {

        for (string str : lines_str)
        {

            Flow f = FlowObj::parse(str, sep);

            (*lcadb).flows[f.flowId] = f;
        }
    }

    void loadFlows(AppSettings settings, string path, bool delta)
    {

        ResultsUtilsSingle resUtils;
        vector<string> lines_str = resUtils.readStringArray(path);

        for (string str : lines_str)
        {

            std::size_t pos = (delta ? str.find(sep) : -1);
            string entityString = (delta ? str.substr(pos + 1) : str);
            string cmd = (delta ? str.substr(0, pos) : "");

            Flow f = FlowObj::parse(entityString, sep);

            if (!delta || cmd != "Delete")
            {
                (*lcadb).flows[f.flowId] = f;
            }
            else
            {
                deleteFlow(f.flowId);
            }
        }
    }

    void deleteFlow(long id)
    {
        (*lcadb).flows.erase((*lcadb).flows.find(id));
    }

    void loadImpactCategories(vector<string> lines_str)
    {

        for (string str : lines_str)
        {

            ImpactCategory ic = ImpactCategoryObj::parse(str, sep);

            (*lcadb).impactCategories[ic.ICId] = ic;
        }
    }

    void loadImpactCategories(AppSettings settings, string path, bool delta)
    {

        ResultsUtilsSingle resUtils;

        vector<string> lines_str = resUtils.readStringArray(path);

        for (string str : lines_str)
        {

            std::size_t pos = (delta ? str.find(sep) : -1);
            string entityString = (delta ? str.substr(pos + 1) : str);
            string cmd = (delta ? str.substr(0, pos) : "");

            ImpactCategory p = ImpactCategoryObj::parse(entityString, sep);

            if (!delta || cmd != "Delete")
            {
                (*lcadb).impactCategories[p.ICId] = p;
                (*lcadb).impactMethodCategories[p.ICImpactMethod].push_back(p.ICId);
            }
            else
            {
                deleteImpactCategory(p.ICId);
            }
        }
    }

    void deleteImpactCategory(long id)
    {
        (*lcadb).impactCategories.erase((*lcadb).impactCategories.find(id));
    }

    void loadUnitOfMeasurements(vector<string> lines_str)
    {

        for (string str : lines_str)
        {

            UnitOfMeasurement p = UnitOfMeasurementObj::parse(str, sep);
            deleteUnitOfMeasurement(p.UnitId);
        }
    }

    void loadUnitOfMeasurements(AppSettings settings, string path, bool delta)
    {

        ResultsUtilsSingle resUtils;

        vector<string> lines_str = resUtils.readStringArray(path);

        for (string str : lines_str)
        {

            std::size_t pos = (delta ? str.find(sep) : -1);
            string entityString = (delta ? str.substr(pos + 1) : str);
            string cmd = (delta ? str.substr(0, pos) : "");

            UnitOfMeasurement p = UnitOfMeasurementObj::parse(entityString, sep);

            if (!delta || cmd != "Delete")
            {
                (*lcadb).unitOfMeasurements[p.UnitId] = p;
            }
            else
            {
                deleteUnitOfMeasurement(p.UnitId);
            }
        }
    }

    void deleteUnitOfMeasurement(long id)
    {
        (*lcadb).unitOfMeasurements.erase((*lcadb).unitOfMeasurements.find(id));
    }

    void loadParameters(AppSettings settings, string path, bool delta)
    {

        FileUtils fileUtils;

        vector<string> lines_str = fileUtils.readTextFromFile(path);

        for (string str : lines_str)
        {

            std::size_t pos = (delta ? str.find(sep) : -1);
            string entityString = (delta ? str.substr(pos + 1) : str);
            string cmd = (delta ? str.substr(0, pos) : "");

            CalcParameter p = CalcParameterObj::parse(entityString, sep);

            if (!delta || cmd != "Delete")
            {
                if (p.scope == "GLOBAL")
                {
                    (*lcadb).parameters[make_pair(p.scope + "_0", p.name)] = p;
                }
                else
                {
                    (*lcadb).parameters[make_pair(p.scope + "_" + to_string(p.f_owner), p.name)] = p;
                }
            }
            else
            {
                deleteParameter(p.f_owner, p.scope, p.name);
            }
        }
    }

    void deleteParameter(long owner, string scope, string name)
    {

        CalcParameter p = (*lcadb).parameters[std::make_pair(scope + "_" + to_string(owner), name)];

        (*lcadb).parameters.erase(
            (*lcadb).parameters.find(std::make_pair(scope + "_" + to_string(owner), name)));
    }

    void loadParameters(vector<string> parameters_str)
    {

        vector<CalcParameter> parameters = readParameters(parameters_str);

        for (CalcParameter p : parameters)
        {

            if (p.scope == "GLOBAL")
            {

                (*lcadb).parameters[make_pair(p.scope + "_0", p.name)] = p;
            }
            else
            {

                (*lcadb).parameters[make_pair(p.scope + "_" + to_string(p.f_owner), p.name)] = p;
            }
        }
    }

    vector<CalcParameter> readParameters(vector<string> P)
    {

        std::vector<CalcParameter> resV;

        long Plength = P.size();

        for (int i = 0; i < Plength; i++)
        {

            //98908,a2c73588-64c9-4001-94bc-36734699f06f,gd_price_waste,0,GLOBAL,1,3,0.5,,0,0.0,0.0,0.0,,,x

            resV.push_back(CalcParameterObj::parse(P[i], sep));
        }

        return resV;
    }

    void loadProcLocationCodes(AppSettings settings)
    {

        FileUtils fileUtils;
        ResultsUtilsSingle res;

        vector<string> v = fileUtils.readTextFromFile(settings.RootPath + "data/DBTemplates/" +
                                                      to_string(settings.DBTemplateId) + "/locproc.txt");

        for (auto &&line : v)
        {

            vector<string> fields = res.split(line, ",");

            (*lcadb).processesLocationCodes[stol(fields[0])] = fields[1];
        }
    }

    void deleteImpactFactor(long catid, long flowid)
    {

        std::pair<long, long> key = std::pair<long, long>(catid, flowid);

        /*std::tr1::unordered_*/ map<std::pair<long, long>, CalcImpactFactor>::iterator it =
            (*lcadb).impactFactors.find(key);

        if (it != (*lcadb).impactFactors.end())
            (*lcadb).impactFactors.erase(it);

        std::tr1::unordered_map<long, long>::iterator it2 = (*lcadb).impatCategoriesUnits.find(catid);
        if (it2 != (*lcadb).impatCategoriesUnits.end())
            (*lcadb).impatCategoriesUnits.erase(it2);
    }

    void loadImpactFactor(CalcImpactFactor iif)
    {

        (*lcadb).impactFactors[std::pair<long, long>(iif.imactCategoryId, iif.flowId)] = iif;
        (*lcadb).impatCategoriesUnits[iif.imactCategoryId] = iif.unitid;
    }

    void loadImpactFactors(AppSettings settings, vector<string> impactfactors_str)
    {
        LCADBUtils dbUtils(lcadb, settings);

        vector<CalcImpactFactor> calcImpactFactors = dbUtils.readImpactFactors(impactfactors_str);

        for (CalcImpactFactor iif : calcImpactFactors)
        {
            loadImpactFactor(iif);
        }
    }

    void loadImpactFactors(AppSettings settings, string path, bool delta)
    {

        readLoadImpactFactors(path, delta);

        //cout << "calcImpactFactors" << calcImpactFactors.size() << endl;

        //#pragma omp for
        //        for (CalcImpactFactor iif : calcImpactFactors) {
        //            loadImpactFactor(iif);
        //        }

        //cout <<"(*lcadb).impactFactors"<< (*lcadb).impactFactors.size()<<endl;
    }

    void updateExchangeIndex(Exchange exch)
    {

        if (exch.flowType == 2)
        {

            ((*lcadb).elementaryExchangesByProcess[exch.processId]).push_back(exch.exchangeId);
            //vector<long> v = ((*lcadb).elementaryExchangesByProcess[exch.processId]);
            //boost::copy(boost::unique(boost::sort(v)), std::back_inserter(((*lcadb).elementaryExchangesByProcess[exch.processId])));
        }
        else
        {

            if (exch.input)
            {
                (*lcadb).inputExchangesByProcess[exch.processId].push_back(exch.exchangeId);

                //                vector<long> v = (*lcadb).inputExchangesByProcess[exch.processId];
                //
                //
                //                boost::copy(boost::unique(boost::sort(v)), std::back_inserter(((*lcadb).inputExchangesByProcess[exch.processId])));
            }
            else
            {
                (*lcadb).outputExchangesByProcess[exch.processId].push_back(exch.exchangeId);

                //                vector<long> v = (*lcadb).outputExchangesByProcess[exch.processId];
                //                vector<long> v2;
                //                boost::copy(boost::unique(boost::sort(v)), std::back_inserter(v2));
                //                (*lcadb).outputExchangesByProcess[exch.processId]=v2;

                (*lcadb).producersExchangesByFlow[exch.flowId].push_back(exch.exchangeId);

                //                vector<long> v_ = (*lcadb).producersExchangesByFlow[exch.flowId];
                //                vector<long> v2_;
                //
                //
                //                boost::copy(boost::unique(boost::sort(v_)), std::back_inserter(v2_));
                //                (*lcadb).producersExchangesByFlow[exch.flowId]=v2_;
            }
        }
    }

    void emptyExchangesIndexes()
    {

        (*lcadb).elementaryExchangesByProcess.clear();

        (*lcadb).inputExchangesByProcess.clear();

        (*lcadb).outputExchangesByProcess.clear();

        (*lcadb).producersExchangesByFlow.clear();
    }

    void loadExchange(Exchange exch)
    {
        (*lcadb).exchanges.insert(pair<long, Exchange>(exch.exchangeId, exch));
    }

    void loadExchanges(AppSettings settings, vector<string> exchanges_str)
    {

        LCADBUtils dbUtils(lcadb, settings);

        vector<Exchange> exchanges = dbUtils.loadExchanges(exchanges_str);

        //                .readExchanges(
        //
        //                settings.RootPath + "data/calculations/ETS_123_v1_234364_-52234918/calcexchanges.txt"
        //                //settings.RootPath + "data/calculations/" + settings.CalculationId + "/calcexchanges.txt"
        //
        //                );

        emptyExchangesIndexes();

        for (Exchange exch : exchanges)
        {
            loadExchange(exch);
        }

        std::tr1::unordered_map<long, Exchange>::iterator it;

        for (it = (*lcadb).exchanges.begin(); it != (*lcadb).exchanges.end(); it++)
        {

            updateExchangeIndex((*it).second);
        }
    }

    void loadExchanges(AppSettings settings, string path, bool delta)
    {

        emptyExchangesIndexes();

        readLoadExchanges(path, delta);

        std::tr1::unordered_map<long, Exchange>::iterator it;

        for (it = (*lcadb).exchanges.begin(); it != (*lcadb).exchanges.end(); it++)
        {
            updateExchangeIndex((*it).second);
        }

        // int xid = 2;

        //        for (Exchange exch : exchanges) {
        //            loadExchange(exch);
        //
        //            //            if (exch.input == 0 && exch.flowType == 1
        //            //                    //&& GraphUtilsSingle::isCountry((*lcadb).processesLocationCodes[exch.processId])
        //            //                    ) {
        //            //                if (exch.amount*exch.conversionFactor !=0) {
        //            //                    loadExchange(Exchange(xid, settings.RootProcessId, exch.flowId, 1, 1, 1, "",
        //            //                            0, 0.0, 0.0, 0.0, "", "", "",
        //            //                            1, exch.processId, "UNIT_PROCESS", 0, 509));
        //            //                    xid++;
        //            //                }
        //            //
        //            //            }
        //
        //        }

        //
        //        loadExchange(Exchange(1, settings.RootProcessId, 1, 0, 1, 1, "",
        //                0, 0.0, 0.0, 0.0, "", "", "",
        //                1, 0, "UNIT_PROCESS", 0, 509));

        //        std::tr1::unordered_map<long, Exchange>::iterator it;
        //
        //        for (it = (*lcadb).exchanges.begin(); it != (*lcadb).exchanges.end(); it++) {
        //            updateIndex((*it).second);
        //
        //        }

        //        cout << (*lcadb).elementaryExchangesByProcess.size() << endl;
        //
        //        cout << (*lcadb).exchanges.size() << endl;
        //        cout << (*lcadb).inputExchangesByProcess.size() << endl;
        //        cout << (*lcadb).outputExchangesByProcess.size() << endl;
        //
        //        cout << (*lcadb).producersExchangesByFlow.size() << endl;
    }

    void deleteExchange(long exchid)
    {

        ResultsUtilsSingle resUtils;

        std::tr1::unordered_map<long, Exchange>::iterator it = (*lcadb).exchanges.find(exchid);

        if (it != (*lcadb).exchanges.end())
        {

            (

                *lcadb)
                .exchanges.erase(it);
        }
    }

    vector<ExchangeCell> readExchangeCells(string path)
    {

        FileUtils fileUtils;

        vector<string> lines = fileUtils.readTextFromFile(path);

        std::vector<ExchangeCell> resV;

        for (const auto &s : lines)
        {

            //println("l: "+ l)

            vector<string> fields = split(s, sep);

            long i = stol(fields[0]);
            long j = stol(fields[1]);
            double amount = stod(fields[2]);

            ExchangeCell cell = ExchangeCell{

                i, j, amount

            };

            resV.push_back(cell);
        }

        return resV;
    }

    vector<ExchangeItem> loadExchangeItems(long *mi_ptr, long *mj_ptr, vector<string> m)
    {

        std::vector<ExchangeItem> resV;

        long mlength = m.size();

        for (int i = 0; i < mlength; i++)
        {

            vector<string> fields = split(m[i], sep);

            ExchangeObj calcobj;
            Exchange calc = calcobj.create(fields, lcadb);

            ExchangeItem cell = ExchangeItem{

                mi_ptr[i], mj_ptr[i], calc

            };

            resV.push_back(cell);
        }

        return resV;
    }

    vector<Exchange> loadExchanges(vector<string> m)
    {

        std::vector<Exchange> resV;

        long mlength = m.size();

        for (int i = 0; i < mlength; i++)
        {

            vector<string> fields = split(m[i], sep);

            ExchangeObj calcobj;
            Exchange calc = calcobj.create(fields, lcadb);
            resV.push_back(calc);
        }

        return resV;
    }

    vector<ExchangeItem> readExchangeItems(string path)
    {

        FileUtils fileUtils;

        vector<string> lines = fileUtils.readTextFromFile(path);

        //cout << path << endl;
        //cout << lines.size() << endl;

        std::vector<ExchangeItem> resV;

        for (const auto &s : lines)
        {

            //println("l: "+ l)

            vector<string> fields = split(s, sep);

            long i = stol(fields[0]);
            long j = stol(fields[1]);

            fields.erase(fields.begin(), fields.begin() + 2);

            ExchangeObj calcobj;
            Exchange calc = calcobj.create(fields, lcadb);
            ExchangeItem cell = ExchangeItem{

                i, j, calc

            };

            resV.push_back(cell);
        }

        return resV;
    }

    vector<Exchange> readExchanges(string path)
    {

        FileUtils fileUtils;

        vector<string> lines = fileUtils.readTextFromFile(path);

        std::vector<Exchange> resV;

        for (const auto &s : lines)
        {

            vector<string> fields = split(s, sep);

            ExchangeObj calcobj;
            Exchange calc = calcobj.create(fields, lcadb);

            resV.push_back(calc);
        }

        return resV;
    }

    void readLoadExchanges(string path, bool delta)
    {
        FileUtils fileUtils;

        vector<string> lines = fileUtils.readTextFromFile(path);

        int num_processors = settings.Parallelism;
        int vectorLength = lines.size();
        int slotsize = ceil((float)vectorLength / num_processors);
        slotsize = (slotsize < 1 ? 1 : slotsize);

        LCAException lcaex(-1, "");
        exception ex;
        bool isexception=false;

#pragma omp parallel for shared(lines)
        for (int th = 0; th < num_processors; th++)
        {
            std::vector<std::pair<string, Exchange>> resV_th;

            try
            {

                int ii = th * slotsize;

                while (ii < (th * slotsize) + slotsize)
                {

                    if (ii >= vectorLength)
                    {
                        break;
                    };

                    string str = lines[ii];

                    std::size_t pos = (delta ? str.find(sep) : -1);
                    string entityString = (delta ? str.substr(pos + 1) : str);
                    string cmd = (delta ? str.substr(0, pos) : "");

                    vector<string> fields = split(entityString, sep);

                    ExchangeObj calcobj;
                    Exchange calc = calcobj.create(fields, lcadb);

                    resV_th.push_back(std::make_pair(cmd, calc));

                    ii++;
                }
            }
            catch (LCAException &e)
            {
                lcaex = e;
            }
            catch (exception & e2)
            {
                isexception=true;
                ex=e2;
            }

#pragma omp critical
            {
                for (auto &&calc : resV_th)
                {
                    if (!delta || calc.first != "Delete")
                    {
                        deleteExchange(calc.second.exchangeId);

                        loadExchange(calc.second);
                    }
                    else
                    {

                        deleteExchange(calc.second.exchangeId);
                    }
                }
            }
        }

        if (lcaex.code_ != -1)
        {
            throw ex;
        }

        if(isexception){ throw ex; }
    };

    vector<CalcImpactFactorItem> loadImpactCells(long *Qi_ptr, long *Qj_ptr, vector<string> Q)
    {

        std::vector<CalcImpactFactorItem> resV;

        long Qlength = Q.size();

        for (int i = 0; i < Qlength; i++)
        {

            //println("l: "+ l)

            vector<string> fields = split(Q[i], sep);

            CalcImpactFactorObj calcObj;
            CalcImpactFactor calc = calcObj.create(fields, lcadb);

            CalcImpactFactorItem cell = CalcImpactFactorItem{

                Qi_ptr[i], Qj_ptr[i], calc

            };

            resV.push_back(cell);
        }

        return resV;
    }

    vector<CalcImpactFactor> readImpactFactors(vector<string> Q)
    {

        std::vector<CalcImpactFactor> resV;

        long Qlength = Q.size();

        for (int i = 0; i < Qlength; i++)
        {

            vector<string> fields = split(Q[i], sep);

            CalcImpactFactorObj calcObj;
            CalcImpactFactor calc = calcObj.create(fields, lcadb);

            resV.push_back(calc);
        }

        return resV;
    }

    vector<CalcImpactFactorItem> readImpactCells(string path)
    {

        FileUtils fileUtils;

        vector<string> lines = fileUtils.readTextFromFile(path);

        std::vector<CalcImpactFactorItem> resV;

        for (const auto &s : lines)
        {

            //println("l: "+ l)

            vector<string> fields = split(s, sep);

            long i = stol(fields[0]);
            long j = stol(fields[1]);

            fields.erase(fields.begin(), fields.begin() + 2);

            CalcImpactFactorObj calcObj;
            CalcImpactFactor calc = calcObj.create(fields, lcadb);

            CalcImpactFactorItem cell = CalcImpactFactorItem{

                i, j, calc

            };

            resV.push_back(cell);
        }

        return resV;
    }

    void readLoadImpactFactors(string path, bool delta)
    {
        FileUtils fileUtils;

        vector<string> lines = fileUtils.readTextFromFile(path);

        int num_processors = settings.Parallelism;
        int vectorLength = lines.size();
        int slotsize = ceil((float)vectorLength / num_processors);
        LCAException lcaex(-1, "");
         exception ex;
        bool isexception=false;

#pragma omp parallel for shared(lines)
        for (int th = 0; th < num_processors; th++)
        {
            std::vector<std::pair<string, CalcImpactFactor>> resV_th;
            int ii = th * slotsize;
            try
            {
                while (ii < (th * slotsize) + slotsize)
                {

                    if (ii >= vectorLength)
                    {
                        break;
                    };

                    string str = lines[ii];

                    std::size_t pos = (delta ? str.find(sep) : -1);
                    string entityString = (delta ? str.substr(pos + 1) : str);
                    string cmd = (delta ? str.substr(0, pos) : "");

                    ResultsUtilsSingle res;

                    vector<string> fields = res.split(entityString, sep);
                    CalcImpactFactorObj calcObj;
                    CalcImpactFactor calc = calcObj.create(fields, lcadb);
                    resV_th.push_back(std::make_pair(cmd, calc));

                    ii++;
                }
            }
           catch (LCAException &e)
            {
                lcaex = e;
            }
            catch (exception & e2)
            {
                isexception=true;
                ex=e2;
            }
#pragma omp critical
            {
                for (auto &&calc : resV_th)
                {

                    if (!delta || calc.first != "Delete")
                    {
                        loadImpactFactor(calc.second);
                    }
                    else
                    {

                        deleteImpactFactor(calc.second.id, calc.second.flowId);
                    }
                }
            }
        }
       if (lcaex.code_ != -1)
        {
            throw ex;
        }

        if(isexception){ throw ex; }
    }

    vector<CalcImpactFactor> readImpactFactors(string path)
    {

        FileUtils fileUtils;

        vector<string> lines = fileUtils.readTextFromFile(path);

        std::vector<CalcImpactFactor> resV;

        for (const auto &s : lines)
        {

            ResultsUtilsSingle res;

            vector<string> fields = res.split(s, sep);

            CalcImpactFactorObj calcObj;
            CalcImpactFactor calc = calcObj.create(fields, lcadb);

            resV.push_back(calc);
        }

        return resV;
    }
};

#endif /* LCADBUTILS_H */
