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
#include <stdio.h>
#include <iostream>
#include <regex>
#include <iterator>
using namespace std;

/*
 * 
 */
//   mpic++ -pipe   -m64  -fPIC ./src/main/c++/Native/main.cpp -std=c++11 -O3 -o reg
int main(int argc, char **argv)
{

    std::regex e("\\b(?!exp|ln|log|sqrt)[A-Za-z]\\w*\\b");
    std::smatch m;
    string s("(x+y)/exp(z)+ln(h)-sqrt(f+log(t))");

    while (std::regex_search (s,m,e)) {
           for (auto x:m) std::cout << x << " ";
            s = m.suffix().str();
    }

    // AppSettings settings{
    //     234364,
    //     1.0,
    //     "solve-local-sparse-umfpack",
    //     "alumprodQC42", "ETS_123_v1_234364_-52234918", 973039,
    //     true, false, 10, 5, "./", 123, 3,false};

    // Launcher launcher;

    // try{
    // launcher.launch(settings.RootProcessId,
    //         settings.OutputQunatity,
    //         settings.SolvingMethod,
    //         settings.SystemId,
    //         settings.CalculationId,
    //         settings.ImpactMethodId,
    //         settings.lcia,
    //         (settings.montecarlo ? 1 : 0),
    //         settings.montecarlo_iterations,
    //         settings.Parallelism,
    //         settings.RootPath,
    //         settings.ProjectId,
    //         settings.Version);

    // }catch(const exception & e){

    //     cout<< e.what()<<endl;

    // }

    return 0;
}
