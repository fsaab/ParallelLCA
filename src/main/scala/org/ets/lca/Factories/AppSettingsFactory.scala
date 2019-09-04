
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
package org.ets.lca.Factories

import org.ets.lca.LCACalculators.CalculatorData.CalculationSettings

/**
  * Created by francoissaab on 10/5/16.
  */
class AppSettingsFactory(val args : Array[String]) {

  def create():CalculationSettings={ parseArgs(args, new CalculationSettings())}

  def createLocal():CalculationSettings={

   /* new AppSettings(942228, 1.0,"solve-dist-lu", "alumprodQC1", "solve1", 462933,
      true,false,10,
      Array("6/6exchanges.spark", "6/6processes.spark", "6/6flows.spark", "6/6units.spark",
        "6/6impact_methods.spark", "6/6impact_categories.spark", "6/6impact_factors.spark","6/6parameters.spark"), 4)

    */


   /* new AppSettings(24318377, 1.0,"solve-local-sparse-umfpack", "alumprodQC1", "solve1", 25056796,
      true,false,10,
      Array("10/10exchanges.spark", "10/10processes.spark", "10/10flows.spark", "10/10units.spark",
        "10/10impact_methods.spark", "10/10impact_categories.spark", "10/10impact_factors.spark","10/10parameters.spark"), 4)
*/
    /*

    new AppSettings(1, 1.0,"solve-local-sparse-umfpack", "test1", "solve1", 25056796,
      false,false,10,
      Array("3/10exchanges.spark", "3/10processes.spark", "3/10flows.spark", "3/10units.spark",
        "3/10impact_methods.spark", "3/10impact_categories.spark", "3/10impact_factors.spark","3/10parameters.spark"), 4)

*/

    /* new AppSettings(1, 1.0,"solve-local-sparse-umfpack", "model", "LS", 462933,true,true,10,
      Array("1/1exchanges.spark", "1/1processes.spark", "1/1flows.spark", "1/1units.spark",
        "1/1impact_methods.spark", "1/1impact_categories.spark", "1/1impact_factors.spark","1/1parameters.spark"), 4)
  */

    new CalculationSettings(234364, 1.0,"solve-local-sparse-umfpack", "alumprodQC42", "ETS_123_v1_234364_-52234918", 973039,
      true,true,100,
      "./",123,3,16,true
    )

   /* new CalculationSettings(593976, 1.0,"solve-local-sparse-umfpack", "alumprodQC42", "ETS_123_v1_234364_-522304918", 973039,
      true,false,100, 16,
      "./",555,1
    )*/

    /*new AppSettings(1142701, 1.0,"solve-local-sparse-umfpack", "alumprodQC42", "solve1", 594033,
      true,false,10,
      Array("444/exchanges.spark", "444/processes.spark", "444/flows.spark", "444/units.spark",
        "444/impact_methods.spark", "444/impact_categories.spark", "444/impact_factors.spark","444/parameters.spark"), 4,
      "./",123,2
    )*/
  }

  def parseArgs(args : Array[String],default:CalculationSettings):CalculationSettings={

    val expectedArgsLength=13

    val rootProcessId=args.length match {
      case x: Int if x ==expectedArgsLength  => args(0).toInt
      case _ => default.RootProcessId
    }

    val outQunatity= args.length match {
      case x: Int if x ==expectedArgsLength  => args(1).toDouble
      case _ => default.OutputQunatity
    }
    val solvingMethod= args.length match {
      case x: Int if x ==expectedArgsLength  => args(2).toString()
      case _ => default.SolvingMethod
    }

    val systemProcessId=args.length match {
      case x: Int if x ==expectedArgsLength  => args(3).toString()
      case _ => default.SystemId
    }

    val calculationId=args.length match {
      case x: Int if x ==expectedArgsLength  => args(4).toString()
      case _ => default.CalculationId
    }

    val impactMethodId=args.length match {
      case x: Int if x ==expectedArgsLength  => args(5).toInt
      case _ => default.ImpactMethodId
    }
                      
    val lcia=args.length match {
      case x: Int if x ==expectedArgsLength  => args(6).toBoolean
      case _ => default.lcia
    }

   
    val montecarlo=args.length match {
      case x: Int if x ==expectedArgsLength  => args(7).toBoolean
      case _ => default.montecarlo
    }

    val montecarlo_iterations=args.length match {
      case x: Int if x ==expectedArgsLength  => args(8).toInt
      case _ => default.montecarlo_iterations
    }


    val project=args.length match {
      case x: Int if x ==expectedArgsLength  => args(9).toLong
      case _ => default.ProjectId
    }

    val version=args.length match {
      case x: Int if x ==expectedArgsLength  => args(10).toInt
      case _ => default.Version
    }



    val parallelism=args.length match {
      case x: Int if x ==expectedArgsLength  => args(11).toInt
      case _ => default.Parallelism
    }

    val sensitivity=args.length match {
      case x: Int if x ==expectedArgsLength  => args(12).toBoolean
      case _ => default.Sensitivity
    }

    println("args.length"+args.length)

    println("****** project:"+project+"**** version"+version)

    new CalculationSettings(rootProcessId,outQunatity,solvingMethod,systemProcessId, calculationId,impactMethodId,
      lcia,montecarlo,montecarlo_iterations,
      project,version,
      parallelism,sensitivity)

  }

  def print (settings: CalculationSettings)={

    println("rootProcessId: "+settings.RootProcessId)
    println("outQunatity: "+settings.OutputQunatity)
    println("Matrix_decomposition: "+settings.SolvingMethod)

  }

}
