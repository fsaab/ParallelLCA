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
#include <string>
#include <vector>
#include <chrono>
#include <stdio.h>
#include <time.h>

#include "LCAKernel_LCAKernel.hpp"
#include "../Utilities/ParameterUtils.h"
#include "../DAL/LCADBFactory.hpp"

#include "libs/libStochastic.hpp"
#include "StaticCalculator.hpp"
#include "Factories/CalculatorDataFactory.hpp"
#include "Factories/GraphFactory.hpp"

JNIEXPORT jstring JNICALL Java_LCAKernel_LCAKernel_createdb(JNIEnv *env, jobject,

                                                            jint RootProcessId,
                                                            jdouble OutputQunatity,
                                                            jstring SolvingMethod,
                                                            jstring SystemId,
                                                            jstring CalculationId,
                                                            jint ImpactMethodId,
                                                            jint lcia,
                                                            jint montecarlo,
                                                            jint montecarlo_iterations,
                                                            jint Paralleism,
                                                            jstring RootPath,
                                                            jlong ProjectId,
                                                            jint Version)
{

    try
    {

        const char *nativeSolvingMethod = env->GetStringUTFChars(SolvingMethod, JNI_FALSE);
        const char *nativeSystemId = env->GetStringUTFChars(SystemId, JNI_FALSE);
        const char *nativeCalculationId = env->GetStringUTFChars(CalculationId, JNI_FALSE);
        const char *nativeRootPath = env->GetStringUTFChars(RootPath, JNI_FALSE);

        AppSettings settings{
            (int)RootProcessId,
            (double)OutputQunatity,
            nativeSolvingMethod,
            nativeSystemId,
            nativeCalculationId,
            (int)ImpactMethodId,
            ((int)lcia) == 1,
            ((int)montecarlo) == 1,
            (int)montecarlo_iterations,
            (int)Paralleism,
            nativeRootPath,
            (long)ProjectId,
            (int)Version, false};

        auto strt_db = std::chrono::high_resolution_clock::now();

        LCADBFactory dbfactory(settings);
        LCADB *lcadb = dbfactory.create();

        auto finish_db = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_db = finish_db - strt_db;
        std::cout << "db/c++," << settings.ProjectId << "," << elapsed_db.count() << std::endl;

        string msg = "{\"code\":\"" + to_string(1) + "\",\"message\":\"" + "failed while creating DB" + "\",\"data\":\"" + to_string((jlong)lcadb) + "\"}";
        return (env)->NewStringUTF(msg.c_str());
    }
    catch (LCAException &e)
    {
        //cout << e.code_ << e.message_ << endl;

        return (env)->NewStringUTF(e.toString().c_str());
    }
    catch (exception &e)
    {

        //cout << "................exception 2............." << endl;
        string msg = "{\"code\":\"" + to_string(700) + "\",\"message\":\"" + e.what() + "\",\"data\":\"-1\"}";

        return (env)->NewStringUTF(msg.c_str());
    }
    catch (...)
    {
        //cout << "................exception 3............." << endl;

        //  cout << e.what() << '\n';

        string msg = "{\"code\":\"" + to_string(700) + "\",\"message\":\"Undefined exception while creating DB\",\"data\":\"-1\"}";

        return (env)->NewStringUTF(msg.c_str());
    }
}

/*
 * Class:     LCAKernel_LCAKernel
 * Method:    deletedb
 * Signature: (J)J
 */
JNIEXPORT jboolean JNICALL Java_LCAKernel_LCAKernel_deletedb(JNIEnv *env, jobject, jlong dbptr)
{

    LCADB *lcadb = (LCADB *)dbptr;

    delete lcadb;

    return true;
}

// JNIEXPORT jboolean JNICALL Java_LCAKernel_LCAKernel_deletecalculatorData(JNIEnv *env, jobject, jlong calculatorDataptr)
// {

//     CalculatorData *calculatorData = (CalculatorData *)calculatorDataptr;

//     delete calculatorData;
//     return true;
// }

/*
 * Class:     LCAKernel_LCAKernel
 * Method:    updatedb
 * Signature: (J[Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/String;)Z
 */
JNIEXPORT jstring JNICALL Java_LCAKernel_LCAKernel_updatedb(JNIEnv *env, jobject, jlong dbptr,

                                                            jint RootProcessId,
                                                            jdouble OutputQunatity,
                                                            jstring SolvingMethod,
                                                            jstring SystemId,
                                                            jstring CalculationId,
                                                            jint ImpactMethodId,
                                                            jint lcia,
                                                            jint montecarlo,
                                                            jint montecarlo_iterations,
                                                            jint Paralleism,
                                                            jstring RootPath,
                                                            jlong ProjectId,
                                                            jint Version)
{

    try
    {
        const char *nativeSolvingMethod = env->GetStringUTFChars(SolvingMethod, JNI_FALSE);
        const char *nativeSystemId = env->GetStringUTFChars(SystemId, JNI_FALSE);
        const char *nativeCalculationId = env->GetStringUTFChars(CalculationId, JNI_FALSE);
        const char *nativeRootPath = env->GetStringUTFChars(RootPath, JNI_FALSE);

        AppSettings settings{
            (int)RootProcessId,
            (double)OutputQunatity,
            nativeSolvingMethod,
            nativeSystemId,
            nativeCalculationId,
            (int)ImpactMethodId,
            ((int)lcia) == 1,
            ((int)montecarlo) == 1,
            (int)montecarlo_iterations,
            (int)Paralleism,
            nativeRootPath,
            (long)ProjectId,
            (int)Version, false};

        // A_v, Q_v, P_v
        auto strt_db = std::chrono::high_resolution_clock::now();

        LCADB *lcadb = (LCADB *)dbptr;

        LCADBFactory dbfactory(settings);
        //cout << "-------------update----------------" << endl;
        dbfactory.update(lcadb, settings);

        string msg = "{\"code\":\"1\",\"message\":\"\",\"data\":\"" + to_string((long)lcadb) + "\"}";
        return (env)->NewStringUTF(msg.c_str());
    }
    catch (LCAException &e)
    {
        //cout << e.code_ << e.message_ << endl;
        return (env)->NewStringUTF(e.toString().c_str());
    }
    catch (exception &e)
    {

        //cout << "................exception 2............." << endl;
        string msg = "{\"code\":\"" + to_string(700) + "\",\"message\":\"" + e.what() + "\",\"data\":\"-1\"}";

        return (env)->NewStringUTF(msg.c_str());
    }
    catch (...)
    {
        //cout << "................exception 3............." << endl;
        //  cout << e.what() << '\n';

        string msg = "{\"code\":\"" + to_string(700) + "\",\"message\":\"Undefined exception while creating DB\",\"data\":\"-1\"}";
        return (env)->NewStringUTF(msg.c_str());
    }
}


JNIEXPORT jstring JNICALL Java_LCAKernel_LCAKernel_staticAndstochastic(JNIEnv *env, jobject,
                                                              jint RootProcessId,
                                                              jdouble OutputQunatity,
                                                              jstring SolvingMethod,
                                                              jstring SystemId,
                                                              jstring CalculationId,
                                                              jint ImpactMethodId,
                                                              jint lcia,
                                                              jint montecarlo,
                                                              jint montecarlo_iterations,
                                                              jint Paralleism,
                                                              jstring RootPath,
                                                              jlong ProjectId,
                                                              jint Version,
                                                              jlong dbptr, jlong calculatorData, jint sensitivity
                                                              //        jobjectArray exchanges,
                                                              //        jobjectArray impactfactors,
                                                              //        jobjectArray parameters
)
{

    try
    {

        const char *nativeSolvingMethod = env->GetStringUTFChars(SolvingMethod, JNI_FALSE);
        const char *nativeSystemId = env->GetStringUTFChars(SystemId, JNI_FALSE);
        const char *nativeCalculationId = env->GetStringUTFChars(CalculationId, JNI_FALSE);
        const char *nativeRootPath = env->GetStringUTFChars(RootPath, JNI_FALSE);

        AppSettings settings{
            (int)RootProcessId,
            (double)OutputQunatity,
            nativeSolvingMethod,
            nativeSystemId,
            nativeCalculationId,
            (int)ImpactMethodId,
            ((int)lcia) == 1,
            ((int)montecarlo) == 1,
            (int)montecarlo_iterations,
            (int)Paralleism,
            nativeRootPath,
            (long)ProjectId,
            (int)Version, sensitivity == 1};

        LCADB *lcadb = (LCADB *)dbptr;

        vector<long> v;
        v.push_back(settings.RootProcessId);

        GraphFactory graphFactory(settings);
        GraphData *graph = graphFactory.create(lcadb, v);

        CalculatorDataFactory simFactory(settings);
        CalculatorData *calculatorData;

       //if(calculatorData==-1){

        auto strt = std::chrono::high_resolution_clock::now();

        calculatorData = simFactory.create(lcadb, graph);

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - strt;
        std::cout << "CalculatorData/c++," << settings.ProjectId << "," << elapsed.count() << std::endl;

        /* }else{
        
             calculatorData = (CalculatorData*) calculatorData;
         }*/

        //        CalculatorDataFactory simFactory(settings);
        //    CalculatorData* calculatorData_all = simFactory.loadEcoinvent(lcadb);
        //


        libStochastic lib;

        lib.run((int)RootProcessId,
                (double)OutputQunatity,
                nativeSolvingMethod,
                nativeSystemId,
                nativeCalculationId,
                (int)ImpactMethodId,
                ((int)lcia) == 1,
                ((int)montecarlo) == 1,
                (int)montecarlo_iterations,
                (int)Paralleism,
                nativeRootPath,
                (long)ProjectId,
                (int)Version,
                lcadb,
                sensitivity == 1,
                calculatorData,
                graph);

        StaticCalculator StaticCalculator(calculatorData, false, graph, lcadb);

        StaticCalculator.run((int)RootProcessId,
                             (double)OutputQunatity,
                             nativeSolvingMethod,
                             nativeSystemId,
                             nativeCalculationId,
                             (int)ImpactMethodId,
                             (int)lcia,
                             (int)montecarlo,
                             (int)montecarlo_iterations,
                             (int)Paralleism,
                             nativeRootPath,
                             (long)ProjectId,
                             (int)Version);

       

        env->ReleaseStringUTFChars(SolvingMethod, nativeSolvingMethod);
        env->ReleaseStringUTFChars(SystemId, nativeSystemId);
        env->ReleaseStringUTFChars(CalculationId, nativeCalculationId);
        env->ReleaseStringUTFChars(RootPath, nativeRootPath);

        delete graph;

        //long calculatorData_ptr = (long)calculatorData;

        string result = "{\"code\":\"1\",\"message\":\"success in stochastic phase ............. \",\"data\":\"" + to_string(-1) + "\"}";

        return (env)->NewStringUTF(result.c_str());
    }
    catch (exception &e)
    {

        return (env)->NewStringUTF(e.what());
    }
    catch (...)
    {

        //  cout << e.what() << '\n';

        string result = "{\"code\":\"0\",\"message\":\"failed in stochastic phase ............... \",\"data\":\"\"}";

        return (env)->NewStringUTF(result.c_str());
    }
}


JNIEXPORT jstring JNICALL Java_LCAKernel_LCAKernel_stochastic(JNIEnv *env, jobject,
                                                              jint RootProcessId,
                                                              jdouble OutputQunatity,
                                                              jstring SolvingMethod,
                                                              jstring SystemId,
                                                              jstring CalculationId,
                                                              jint ImpactMethodId,
                                                              jint lcia,
                                                              jint montecarlo,
                                                              jint montecarlo_iterations,
                                                              jint Paralleism,
                                                              jstring RootPath,
                                                              jlong ProjectId,
                                                              jint Version,
                                                              jlong dbptr, jlong calculatorData, jint sensitivity
                                                              //        jobjectArray exchanges,
                                                              //        jobjectArray impactfactors,
                                                              //        jobjectArray parameters
)
{

    try
    {

        const char *nativeSolvingMethod = env->GetStringUTFChars(SolvingMethod, JNI_FALSE);
        const char *nativeSystemId = env->GetStringUTFChars(SystemId, JNI_FALSE);
        const char *nativeCalculationId = env->GetStringUTFChars(CalculationId, JNI_FALSE);
        const char *nativeRootPath = env->GetStringUTFChars(RootPath, JNI_FALSE);

        AppSettings settings{
            (int)RootProcessId,
            (double)OutputQunatity,
            nativeSolvingMethod,
            nativeSystemId,
            nativeCalculationId,
            (int)ImpactMethodId,
            ((int)lcia) == 1,
            ((int)montecarlo) == 1,
            (int)montecarlo_iterations,
            (int)Paralleism,
            nativeRootPath,
            (long)ProjectId,
            (int)Version, sensitivity == 1};

        LCADB *lcadb = (LCADB *)dbptr;

        vector<long> v;
        v.push_back(settings.RootProcessId);

         auto strt_g = std::chrono::high_resolution_clock::now();

        GraphFactory graphFactory(settings);
        GraphData *graph = graphFactory.create(lcadb, v);
        
        auto finish_g = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_g = finish_g - strt_g;
        std::cout << "Graph/c++," << settings.ProjectId << "," << elapsed_g.count() << std::endl;

        CalculatorDataFactory simFactory(settings);
        CalculatorData *calculatorData;

       //if(calculatorData==-1){

        auto strt = std::chrono::high_resolution_clock::now();

        calculatorData = simFactory.create(lcadb, graph);

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - strt;
        std::cout << "CalculatorData/c++," << settings.ProjectId << "," << elapsed.count() << std::endl;

        /* }else{
        
             calculatorData = (CalculatorData*) calculatorData;
         }*/

        //        CalculatorDataFactory simFactory(settings);
        //    CalculatorData* calculatorData_all = simFactory.loadEcoinvent(lcadb);
        //

        //libStochasticMPI lib;
        libStochastic lib;

        lib.run((int)RootProcessId,
                (double)OutputQunatity,
                nativeSolvingMethod,
                nativeSystemId,
                nativeCalculationId,
                (int)ImpactMethodId,
                ((int)lcia) == 1,
                ((int)montecarlo) == 1,
                (int)montecarlo_iterations,
                (int)Paralleism,
                nativeRootPath,
                (long)ProjectId,
                (int)Version,
                lcadb,
                sensitivity == 1,
                calculatorData,graph);

        env->ReleaseStringUTFChars(SolvingMethod, nativeSolvingMethod);
        env->ReleaseStringUTFChars(SystemId, nativeSystemId);
        env->ReleaseStringUTFChars(CalculationId, nativeCalculationId);
        env->ReleaseStringUTFChars(RootPath, nativeRootPath);

        delete graph;

        //long calculatorData_ptr = (long)calculatorData;

        string result = "{\"code\":\"1\",\"message\":\"success in stochastic phase ............. \",\"data\":\"" + to_string(-1) + "\"}";

        return (env)->NewStringUTF(result.c_str());
    }
    catch (exception &e)
    {

        return (env)->NewStringUTF(e.what());
    }
    catch (...)
    {

        //  cout << e.what() << '\n';

        string result = "{\"code\":\"0\",\"message\":\"failed in stochastic phase ............... \",\"data\":\"\"}";

        return (env)->NewStringUTF(result.c_str());
    }
}

JNIEXPORT jstring JNICALL Java_LCAKernel_LCAKernel_static_1calc(JNIEnv *env, jobject,
                                                                jint RootProcessId,
                                                                jdouble OutputQunatity,
                                                                jstring SolvingMethod,
                                                                jstring SystemId,
                                                                jstring CalculationId,
                                                                jint ImpactMethodId,
                                                                jint lcia,
                                                                jint montecarlo,
                                                                jint montecarlo_iterations,
                                                                jint Paralleism,
                                                                jstring RootPath,
                                                                jlong ProjectId,
                                                                jint Version,
                                                                jlong dbptr, jlong calculatorDataptr

)
{

    try
    {

        const char *nativeSolvingMethod = env->GetStringUTFChars(SolvingMethod, JNI_FALSE);
        const char *nativeSystemId = env->GetStringUTFChars(SystemId, JNI_FALSE);
        const char *nativeCalculationId = env->GetStringUTFChars(CalculationId, JNI_FALSE);
        const char *nativeRootPath = env->GetStringUTFChars(RootPath, JNI_FALSE);

        AppSettings settings{
            (int)RootProcessId,
            (double)OutputQunatity,
            nativeSolvingMethod,
            nativeSystemId,
            nativeCalculationId,
            (int)ImpactMethodId,
            ((int)lcia) == 1,
            ((int)montecarlo) == 1,
            (int)montecarlo_iterations,
            (int)Paralleism,
            nativeRootPath,
            (long)ProjectId,
            (int)Version,
            false};

        LCADB *lcadb = (LCADB *)dbptr;

        GraphFactory graphFactory(settings);

        vector<long> v;
        v.push_back(settings.RootProcessId);

        GraphData *graph = graphFactory.create(lcadb, v);

        CalculatorDataFactory simFactory(settings);

        CalculatorData *calculatorData;

        // if (calculatorDataptr == -1)
        // {
            auto strt = std::chrono::high_resolution_clock::now();
            calculatorData = simFactory.create(lcadb, graph);
            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = finish - strt;
            std::cout << "CalculatorData/c++," << settings.ProjectId << "," << elapsed.count() << std::endl;
        // }
        // else
        // {
        //     calculatorData = (CalculatorData *)calculatorDataptr;
        // }

        StaticCalculator StaticCalculator(calculatorData, false, graph, lcadb);

        StaticCalculator.run((int)RootProcessId,
                             (double)OutputQunatity,
                             nativeSolvingMethod,
                             nativeSystemId,
                             nativeCalculationId,
                             (int)ImpactMethodId,
                             (int)lcia,
                             (int)montecarlo,
                             (int)montecarlo_iterations,
                             (int)Paralleism,
                             nativeRootPath,
                             (long)ProjectId,
                             (int)Version);

        env->ReleaseStringUTFChars(SolvingMethod, nativeSolvingMethod);
        env->ReleaseStringUTFChars(SystemId, nativeSystemId);
        env->ReleaseStringUTFChars(CalculationId, nativeCalculationId);
        env->ReleaseStringUTFChars(RootPath, nativeRootPath);

        delete graph;
        delete calculatorData;

        string result = "{\"code\":\"1\",\"message\":\"success in static phase ............. \",\"data\":\"" + to_string(-1) + "\"}";

        return (env)->NewStringUTF(result.c_str());
    }
    catch (exception &e)
    {

        return (env)->NewStringUTF(e.what());
    }
    catch (...)
    {

        //    cout << e.what() << '\n';

        string result = "{\"code\":\"0\",\"message\":\"failed in static phase ...................\",\"data\":\"\"}";

        return (env)->NewStringUTF(result.c_str());
    }
}

JNIEXPORT jstring JNICALL Java_LCAKernel_LCAKernel_static_1basic_1calc(JNIEnv *env, jobject,
                                                                       jint RootProcessId,
                                                                       jdouble OutputQunatity,
                                                                       jstring SolvingMethod,
                                                                       jstring SystemId,
                                                                       jstring CalculationId,
                                                                       jint ImpactMethodId,
                                                                       jint lcia,
                                                                       jint montecarlo,
                                                                       jint montecarlo_iterations,
                                                                       jint Paralleism,
                                                                       jstring RootPath,
                                                                       jlong ProjectId,
                                                                       jint Version,
                                                                       jlong dbptr, jlong calculatorDataptr

)
{

    try
    {

        const char *nativeSolvingMethod = env->GetStringUTFChars(SolvingMethod, JNI_FALSE);
        const char *nativeSystemId = env->GetStringUTFChars(SystemId, JNI_FALSE);
        const char *nativeCalculationId = env->GetStringUTFChars(CalculationId, JNI_FALSE);
        const char *nativeRootPath = env->GetStringUTFChars(RootPath, JNI_FALSE);

        AppSettings settings{
            (int)RootProcessId,
            (double)OutputQunatity,
            nativeSolvingMethod,
            nativeSystemId,
            nativeCalculationId,
            (int)ImpactMethodId,
            ((int)lcia) == 1,
            ((int)montecarlo) == 1,
            (int)montecarlo_iterations,
            (int)Paralleism,
            nativeRootPath,
            (long)ProjectId,
            (int)Version, false};

        LCADB *lcadb = (LCADB *)dbptr;

        GraphFactory graphFactory(settings);

        vector<long> v;
        v.push_back(settings.RootProcessId);

        GraphData *graph = graphFactory.create(lcadb, v);

        CalculatorDataFactory simFactory(settings);

        CalculatorData *calculatorData;

        // if (calculatorDataptr == -1)
        // {

        auto strt = std::chrono::high_resolution_clock::now();
        calculatorData = simFactory.create(lcadb, graph);
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - strt;
        std::cout << "CalculatorData/c++," << settings.ProjectId << "," << elapsed.count() << std::endl;
        // }
        // else
        // {

        //     calculatorData = (CalculatorData *)calculatorDataptr;
        // }

        StaticCalculator StaticCalculator(calculatorData, true, graph, lcadb);
        StaticCalculator.run((int)RootProcessId,
                             (double)OutputQunatity,
                             nativeSolvingMethod,
                             nativeSystemId,
                             nativeCalculationId,
                             (int)ImpactMethodId,
                             (int)lcia,
                             (int)montecarlo,
                             (int)montecarlo_iterations,
                             (int)Paralleism,
                             nativeRootPath,
                             (long)ProjectId,
                             (int)Version);

        env->ReleaseStringUTFChars(SolvingMethod, nativeSolvingMethod);
        env->ReleaseStringUTFChars(SystemId, nativeSystemId);
        env->ReleaseStringUTFChars(CalculationId, nativeCalculationId);
        env->ReleaseStringUTFChars(RootPath, nativeRootPath);

        delete graph;
        delete calculatorData;

        string result = "{\"code\":1,\"message\":\"success in static phase ............. \",\"data\":\"" + to_string(-1) + "\"}";

        return (env)->NewStringUTF(result.c_str());
    }
    catch (exception &e)
    {

        return (env)->NewStringUTF(e.what());
    }
    catch (...)
    {

        //    cout << e.what() << '\n';

        string result = "{\"code\":0,\"message\":\"failed in static phase ...................\",\"data\":\"\"}";

        return (env)->NewStringUTF(result.c_str());
    }
}