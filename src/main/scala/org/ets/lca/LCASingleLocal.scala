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

import Messaging.{CustomException, Message, MessagingUtils}
import org.ets.lca.LCACalculators.Calculation
import org.ets.lca.LCACalculators.CalculatorData.CalculationSettings
import org.ets.lca.Utilities.{FileUtils, TimingUtil}

/**
  * Created by Francois Saab on 2016-07-04.
  */
object LCASingleLocal {


  def main(args: Array[String]) = {

    val settings = new CalculationSettings(1131751,
      1.0,
      "solve-local-sparse-umfpack",
      "alumprodQC42", "ETS_123_v1_1131751_-52234918",
      973039,
      false,true,10, "./",3,1, 4, true
    )



    val calc = new Calculation(settings.ProjectId,settings.Version)

    import scala.util.Try

    def tryLoadDB/*:Try[Boolean]*/= {

      /*Try {*/ new Factories.LCADataBaseFactoryNative(settings).create(calc) /*}*/

    }

    def tryCalculate:Try[Boolean]= {

      Try {
      (new LCACalculator(settings)).calculate()
    }

    }

    val resdb=tryLoadDB //getOrElse(false)


    if(resdb==true){

      val rescalc=tryCalculate getOrElse(false)


      if(rescalc==false){


          PlayGround.message=new Message(101,"Error In the calculation phase.")


      }else{

        PlayGround.message=new Message(1,"Success ...")

      }
    }else{


        PlayGround.message=new Message(100,"Error while loading Database, please revise The data files, check the versions, the DBTemplates, the file structure on server is available.")

    }

    val str=new MessagingUtils().toJSON(PlayGround.message) + sys.props("line.separator")

    val path= settings.RootPath+ "data/calculations/"+settings.CalculationId+"/log.txt"

    (new FileUtils).writeTextToFile(str,path)

    println(str)




  }


}
