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

import LCAKernel.LCAKernel
import Messaging.{Message, MessageCodes, MessagingUtils}
import org.ets.lca.DAL.{LCADBSet, lcadb}
import org.ets.lca.LCACalculators.Calculation
import org.ets.lca.LCACalculators.CalculatorData.CalculationSettings
import org.ets.lca.Versioning.{DiffUtils, StoreUtils}

import scala.util.Try

/**
  * Created by francoissaab on 11/9/16.
  */
class LCADataBaseFactoryNative(var settings: CalculationSettings) {

  def applyIndividualDelta(projectId: Long, version: Int) = {

    val lcadb=LCADBSet.lcadbs(projectId)

    val jsonresult =new LCAKernel().updatedb(lcadb.id,
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
        projectId,
        version)

    import scala.util.parsing.json._

    var code = ""
    var message = ""
    var data = ""

    val resp=Try {

      val parsed = JSON.parseFull(jsonresult).get.asInstanceOf[Map[String, Any]]

       code = parsed("code").asInstanceOf[String]
       message = parsed("message").asInstanceOf[String]
       data = parsed("data").asInstanceOf[String]

      if(code.toLong != 1) (new MessagingUtils).throwMessageAsException(new Message(code.toLong,message))

      true

    }

    if( resp.getOrElse(false) == false ){
      resetDB(true)

      if(code=="")
      {
       (new MessagingUtils).throwMessageAsException(
         new Message(Messaging.MessageCodes.code13,Messaging.MessageCodes.message13)
       )
      }
      else {
       (new MessagingUtils).throwMessageAsException( new Message(code.toLong,message) )
      }
      
    }

    true

  }

  def applyDeltasOnEcoinvent(projectId: Long, version: Int) = {

    val dbTemplates = new StoreUtils(settings).DBTemplates()

    if (dbTemplates == null || dbTemplates.isEmpty) {

      resetDB(false)

      (new MessagingUtils).throwMessageAsException(new Message(MessageCodes.code4, MessageCodes.message4))

    } else if (dbTemplates.length >= 1) {

     if (LCADBSet.lcadbs(settings.ProjectId)==null || LCADBSet.lcadbs(settings.ProjectId).id == -1) {
          initEcoInvent(dbTemplates.head)
          }

      val diffUtils = new DiffUtils(settings)

      val versions = diffUtils.loadVersionsSorted(projectId, version)

      if (diffUtils.checkVersions(versions, version)) {

        versions.foreach(v => { 
          //println("apply for v" + v)
          applyIndividualDelta(projectId, v) 
          })

      } else {

        resetDB(false)

        (new MessagingUtils).throwMessageAsException(new Message(MessageCodes.code2, MessageCodes.message2))
      }

    }

  }

  def loadDeltasForProjectAndVersion(projectId: Long, version: Int) = {


    // project is already loaded

    if (LCADBSet.lcadbs(projectId) != null &&
      ((version - LCADBSet.lcadbs(projectId).currentVersion) == 1)
      )
    //just move one step forward
    {

     // println("sequentially apply.applying version:" + version)

      val diffUtils = new DiffUtils(settings)
      val versions = diffUtils.loadVersionsSorted(projectId, version)

      if (!diffUtils.checkVersions(versions, version)) {

        resetDB(false)

        (new MessagingUtils).throwMessageAsException(new Message(MessageCodes.code2, MessageCodes.message2))
      }

      applyIndividualDelta(projectId, version)

    } else {
      //println("loading non sequentially. apply of deltas from ecoinvent.")

      applyDeltasOnEcoinvent(projectId, version)
    }
  }

  def create(calc:Calculation): Boolean = {

    if (!(new StoreUtils(settings)).checkProject(calc.ProjectId)) {

      println("Error: Project does not exists")
      resetDB(false)
      (new MessagingUtils).throwMessageAsException(new Message(MessageCodes.code5, MessageCodes.message5))

    }


     if(LCADBSet.lcadbs(calc.ProjectId)!=null && ( (calc.ProjectId == LCADBSet.lcadbs(calc.ProjectId).currentProject )
     &&( calc.Version == LCADBSet.lcadbs(calc.ProjectId).currentVersion) )){
        println("No updates on DB ")
     }else{
       // println("updating DB using loadDeltasForProjectAndVersion")
        loadDeltasForProjectAndVersion(calc.ProjectId, calc.Version)
     }

    true
  }

  def init = {

    initEcoInvent(new StoreUtils(settings).DBTemplates().head)
  }

  def initEcoInvent(t: Int) = {

        resetDB(false)

        val lcadb = LCADBSet.lcadbs(settings.ProjectId)

        val jsonresult= new LCAKernel().createdb(
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
            settings.Version
          )

    import scala.util.parsing.json._
    var code = ""
    var message = ""
    var data = ""

    val resp=Try {

      val parsed = JSON.parseFull(jsonresult).get.asInstanceOf[Map[String, Any]]

       code = parsed("code").asInstanceOf[String]
       message = parsed("message").asInstanceOf[String]
       data = parsed("data").asInstanceOf[String]

      LCADBSet.lcadbs(settings.ProjectId).id = data.toLong

      if(code.toLong != 1) (new MessagingUtils).throwMessageAsException(new Message(code.toLong,message))

      true

    }

    if( resp.getOrElse(false) == false ){
      resetDB(true)

      if(code=="")
      {

       (new MessagingUtils).throwMessageAsException(
         new Message(Messaging.MessageCodes.code13,Messaging.MessageCodes.message13)
       )
      }
      else {
       (new MessagingUtils).throwMessageAsException(new Message(code.toLong,message))
      }
      
    }

    true
        
       

  }

  def resetDB(hard:Boolean) = {

   
    if(hard && LCADBSet.lcadbs(settings.ProjectId)!=null){
       if(LCADBSet.lcadbs(settings.ProjectId).id>0) new LCAKernel().deletedb( LCADBSet.lcadbs(settings.ProjectId).id)
    }

     LCADBSet.lcadbs(settings.ProjectId) = null
    LCADBSet.lcadbs(settings.ProjectId) = new lcadb()


  }

}
