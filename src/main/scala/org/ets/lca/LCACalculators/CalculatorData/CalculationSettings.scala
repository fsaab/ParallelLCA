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

package org.ets.lca.LCACalculators.CalculatorData

/**
  * Created by francoissaab on 8/22/16.
  */
class CalculationSettings(_RootProcessId:Int,
                          _OutputQunatity:Double,
                          _SolvingMethod:String,
                          _SystemId:String,
                          _CalculationId:String,
                          _ImpactMethodId:Int,
                          _lcia:Boolean,
                          _montecarlo:Boolean,
                          _montecarlo_iterations:Int,
                          _RootPath:String,
                          _ProjectId:Long,
                          _Version:Int,
                          _Parallelism:Int,
                          _Sensitivity:Boolean

                 ) {

  def RootPath=_RootPath

  def RootProcessId:Int=_RootProcessId

  def OutputQunatity:Double= _OutputQunatity

  def SolvingMethod:String= _SolvingMethod

  def SystemId:String= _SystemId

  def CalculationId:String= _CalculationId

  def Parallelism:Int= _Parallelism

  def ImpactMethodId:Int= _ImpactMethodId

  def lcia:Boolean=_lcia

  def montecarlo:Boolean=_montecarlo

  def montecarlo_iterations:Int=_montecarlo_iterations

  def ProjectId:Long=_ProjectId
  def Version:Int=_Version
  def Sensitivity:Boolean=_Sensitivity



  def this(){

    this(0,0.0,"svd","","",255109/*Traci2.1*/,true,true,10,"./",123,1,/*Runtime.getRuntime().availableProcessors()-2*/ 2,true)
  }


  def this(projectid:Long){

    this(0,0.0,"svd","","",255109/*Traci2.1*/,true,true,10,"./",projectid,1,/*Runtime.getRuntime().availableProcessors()-2*/ 2,true)
  }

  def this(_RootProcess:Int,_OutputQunatity:Double,
           _SolvingMethod:String,_SystemId:String,_CalculationId:String,_ImpactMethodId:Int,
           lcia:Boolean, montecarlo:Boolean,montecarlo_iterations:Int,
          _ProjectId:Long,_Version:Int,_Paralleism:Int,_Sensitivity:Boolean)={

    this(_RootProcess,_OutputQunatity,_SolvingMethod,_SystemId,_CalculationId,_ImpactMethodId,lcia,
      montecarlo,montecarlo_iterations,
       "./",_ProjectId,_Version,_Paralleism,_Sensitivity)
  }

}
