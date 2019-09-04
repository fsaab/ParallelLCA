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
package org.ets.lca

import LCAKernel.LCAKernel
import org.ets.lca.DAL.{LCADBSet}
import org.ets.lca.Factories._
import org.ets.lca.LCACalculators.CalculatorData.CalculationSettings
import org.ets.lca.LCACalculators._
import org.ets.lca.Utilities._
import java.util

import Messaging.{Message, MessagingUtils}

import scala.collection.parallel.immutable.ParVector
import scala.util.Try
import scala.util.parsing.json.JSON

/**
  * Created by francoissaab on 11/9/16.
  */

class LCACalculator(val settings: CalculationSettings) {

  import org.Native._

  Session.loadLib("LCAKernel","0.1")

  val resultUtils=new ResultsUtilsSingle(settings)

  def staticNative()={

    val jsonresult= new LCAKernel().static_calc(
        settings.RootProcessId,
        settings.OutputQunatity,
        settings.SolvingMethod,
        settings.SystemId,
        settings.CalculationId,
        settings.ImpactMethodId,
        if (settings.lcia) 1 else 0,
        if (settings.montecarlo) 1 else 0,
        settings.montecarlo_iterations,
        settings.Parallelism,
        settings.RootPath,
        settings.ProjectId,
        settings.Version,
        LCADBSet.lcadbs(settings.ProjectId).id,
      LCADBSet.lcadbs(settings.ProjectId).simdata
    )


    import scala.util.parsing.json._

    val resp=Try {

      val parsed = JSON.parseFull(jsonresult).get.asInstanceOf[Map[String, Any]]

      val code = parsed("code").asInstanceOf[String]
      val message = parsed("message").asInstanceOf[String]
      val data = parsed("data").asInstanceOf[String]

      println(code)

      /*reset simData*/
      //LCADBSet.lcadbs(settings.ProjectId).simdata = data.toLong

      if(code.toLong !=1) (new MessagingUtils).throwMessageAsException(new Message(code.toLong,message))

      true

    }

   if( resp.getOrElse(false) == false)
     (new MessagingUtils).throwMessageAsException(
     new Message(Messaging.MessageCodes.code13,Messaging.MessageCodes.message13)

   )

    true
  }

  def staticBasicNative()={

    val jsonresult= new LCAKernel().static_basic_calc(
      settings.RootProcessId,
      settings.OutputQunatity,
      settings.SolvingMethod,
      settings.SystemId,
      settings.CalculationId,
      settings.ImpactMethodId,
      if (settings.lcia) 1 else 0,
      if (settings.montecarlo) 1 else 0,
      settings.montecarlo_iterations,
      settings.Parallelism,
      settings.RootPath,
      settings.ProjectId,
      settings.Version,
      LCADBSet.lcadbs(settings.ProjectId).id,
      LCADBSet.lcadbs(settings.ProjectId).simdata
    )


    // import scala.util.parsing.json._

    // val resp=Try {

    //   println(jsonresult)

    //   val parsed = JSON.parseFull(jsonresult).get.asInstanceOf[Map[String, Any]]

    //   val code = parsed("code").asInstanceOf[String]
    //   println(code)
    //   val message = parsed("message").asInstanceOf[String]
    //   println(message)
    //   val data = parsed("data").asInstanceOf[String]
    //   println(data)

    //  LCADBSet.lcadbs(settings.ProjectId).simdata = data.toLong

    //   if(code.toLong !=1) (new MessagingUtils).throwMessageAsException(new Message(code.toLong,message))

    //   true

    // }

    // if( resp.getOrElse(false) == false)
    //   (new MessagingUtils).throwMessageAsException(
    //     new Message(Messaging.MessageCodes.code13,Messaging.MessageCodes.message13)

    //   )

    true
  }

  def stochasticNative()={

      val kernel =new LCAKernel

      val jsonresult= kernel.stochastic(
        settings.RootProcessId,
        settings.OutputQunatity,
        settings.SolvingMethod,
        settings.SystemId,
        settings.CalculationId,
        settings.ImpactMethodId,
        if (settings.lcia) 1 else 0,
        if (settings.montecarlo) 1 else 0,
        settings.montecarlo_iterations,
        settings.Parallelism ,
        settings.RootPath,
        settings.ProjectId,
        settings.Version,
        LCADBSet.lcadbs(settings.ProjectId).id,
        LCADBSet.lcadbs(settings.ProjectId).simdata, 
        ( if(settings.Sensitivity) 1 else 0 ) )
      

      val resp=Try {

        val parsed = JSON.parseFull(jsonresult).get.asInstanceOf[Map[String, Any]]
        val code = parsed("code").asInstanceOf[String]
        val message = parsed("message").asInstanceOf[String]
        val data = parsed("data").asInstanceOf[String]

        //LCADBSet.lcadbs(settings.ProjectId).simdata = data.toLong

        if(code.toLong !=1) (new MessagingUtils).throwMessageAsException(new Message(code.toLong,message))
     }

      if( resp.getOrElse(false) == false) (new MessagingUtils).throwMessageAsException(
        new Message(Messaging.MessageCodes.code13,Messaging.MessageCodes.message13)
      )

     true

    
  }
  
  def staticAndstochastic()={

      val kernel =new LCAKernel

      val jsonresult= kernel.staticAndstochastic(
        settings.RootProcessId,
        settings.OutputQunatity,
        settings.SolvingMethod,
        settings.SystemId,
        settings.CalculationId,
        settings.ImpactMethodId,
        if (settings.lcia) 1 else 0,
        if (settings.montecarlo) 1 else 0,
        settings.montecarlo_iterations,
        settings.Parallelism ,
        settings.RootPath,
        settings.ProjectId,
        settings.Version,
        LCADBSet.lcadbs(settings.ProjectId).id,
        LCADBSet.lcadbs(settings.ProjectId).simdata, 
        ( if(settings.Sensitivity) 1 else 0 ) )
      
      val resp=Try {

        val parsed = JSON.parseFull(jsonresult).get.asInstanceOf[Map[String, Any]]
        val code = parsed("code").asInstanceOf[String]
        val message = parsed("message").asInstanceOf[String]
        val data = parsed("data").asInstanceOf[String]

        //LCADBSet.lcadbs(settings.ProjectId).simdata = data.toLong

        if(code.toLong !=1) (new MessagingUtils).throwMessageAsException(new Message(code.toLong,message))
     }

      if( resp.getOrElse(false) == false) (new MessagingUtils).throwMessageAsException(
        new Message(Messaging.MessageCodes.code13,Messaging.MessageCodes.message13)
      )

     true
  }

  def calculate():Boolean={

    synchronized {
     staticAndstochastic()
    }

    true
  }

}
