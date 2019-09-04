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
package org.ets.lca.Utilities

import java.io.{BufferedWriter, File, FileWriter}
import java.nio.file.{Files, Paths}

import org.ets.lca.LCACalculators.CalculatorData.CalculationSettings

/*import com.amazonaws.services.s3._*/
/*import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.fs.FileSystem*/

import scala.io.Codec

/**
  * Created by francoissaab on 4/28/17.
  */
class ResultsUtilsSingle(settings: CalculationSettings) {

  val sep=29.toChar

  val fileUtils=new FileUtils

  def readIntIndex(path:String)={fileUtils.readTextFromFile(path).map(_.toInt)}
  def readLongIndex(path:String)={fileUtils.readTextFromFile(path).map(_.toLong)}
  def readDoubleArray(path:String)={fileUtils.readTextFromFile(path).map(_.toDouble).toArray}

  def readKeyValueConf(path:String):Array[(String,String)]={

    val content=fileUtils.readTextFromFile(path).toArray

    content.flatMap(x=> x.split(","))
      .map(line=>(line.split("=")(0),line.split("=")(1)))
  }

  def readLCI(path:String)={fileUtils.readTextFromFile(path).map(_.toDouble)}
  def readLCIA(settings:CalculationSettings, iter:Int)={


    fileUtils.readTextFromFile(settings.RootPath + "data/calculations/" + settings.CalculationId+ "/iters/" + iter + "_lcia_.txt").map(_.toDouble)
  }

  def readUNCA(iter:Int,settings:CalculationSettings)={

    fileUtils.readTextFromFile(settings.RootPath + "data/calculations/" + settings.CalculationId+ "/iters/" + iter + "_unca_.txt").par.map(l => {
      val s = l.split(",")
      ((s(0).toLong
        , s(1).toLong), s(2).toDouble)
    })
  }
  def readUNCB(iter:Int,settings: CalculationSettings)={


    fileUtils.readTextFromFile(settings.RootPath + "data/calculations/" + settings.CalculationId+ "/iters/" + iter + "_uncb_.txt").par.map(l => {
      val s = l.split(",")
      ((s(0).toLong
        , s(1).toLong), s(2).toDouble)
    })

  }
  def readUNCQ(path:String)={


    fileUtils.readTextFromFile(path).par.map(l => {
      val s = l.split(",")
      ((s(0).toInt
        , s(1).toInt), s(2).toDouble)
    })

  }

}
