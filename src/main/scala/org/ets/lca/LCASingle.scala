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

import Messaging._
import Messaging.MessagingUtils
import org.ets.lca.DAL.LCADBSet
import org.ets.lca.LCACalculators.Calculation
import org.ets.lca.Utilities.{FileUtils, TimingUtil}

import scala.util.Try

/**
  * Created by Francois Saab on 2016-07-04.
  */
object LCASingle {

import org.Native._

Session.loadLib("LCAKernel","0.1")

  def main(args : Array[String])={

    val settings = (new org.ets.lca.Factories.AppSettingsFactory(args)).create()

    def init:Try[Boolean]={

      Try {

        val fileUtils = new FileUtils

        fileUtils.rmfolder(settings.RootPath + "data/calculations/" + settings.CalculationId + "/")

        fileUtils.newfolder(settings.RootPath + "data/calculations/" + settings.CalculationId + "/")

         true
      }
    }

    val calc = new Calculation(settings.ProjectId,settings.Version)

    PlayGround.message=new Message(-1,"Pending status!")

    def tryLoadDB:Try[Boolean]= {

     Try { new Factories.LCADataBaseFactoryNative(settings).create(calc) }

    }

    def tryCalculate:Try[Boolean]= {

      Try {
      (new LCACalculator(settings)).calculate()
    }

    }

    PlayGround.message=new Message(MessageCodes.code_1,MessageCodes.message_1)

    init getOrElse(false)

    // if(LCADBSet.lcadbs(settings.ProjectId)!=null){
    //  println("lcadb "+ LCADBSet.lcadbs(settings.ProjectId).id)
    // }

    val resdb=synchronized {
        tryLoadDB getOrElse (false)
      }

    if(resdb==true){

      LCADBSet.lcadbs(settings.ProjectId).currentVersion=settings.Version

      val rescalc=tryCalculate getOrElse(false)

      if(rescalc==false){

        if(PlayGround.message.Code == -1 )//unidentified error
         {

             PlayGround.message=new Message(MessageCodes.code_2,MessageCodes.message_2)
         }

      }else{

        PlayGround.message=new Message(MessageCodes.code1,MessageCodes.message1)

      }
    }

    // val str_log= "Execution Log:" + sys.props("line.separator") + sys.props("line.separator") +
    //   new MessagingUtils().toJSON(PlayGround.message)

    // val str_performance= "Execution Time:" + sys.props("line.separator") + sys.props("line.separator") +
    //   TimingUtil.flush

    // (new FileUtils).writeTextToFile(str_log,settings.RootPath+ "data/calculations/"+settings.CalculationId+"/calculation_log.txt")

    // (new FileUtils).writeTextToFile(str_performance,settings.RootPath+ "data/calculations/"+settings.CalculationId+"/performance_log.txt")

    // println(str_log)

  }

  def getMessage()={ PlayGround.message }

}
