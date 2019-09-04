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
package org.ets.lca.Versioning

import java.nio.file.{Files, Paths}

import org.ets.lca.LCACalculators.CalculatorData.CalculationSettings
import org.ets.lca.Utilities.FileUtils

import scala.io.Codec

/**
  * Created by francoissaab on 1/8/18.
  */
class DiffUtils(settings:CalculationSettings) {

  val fileUtils=new FileUtils

  val sep=29.toChar



  def loadDeltaFile_(name:String,projectId:Long,version:Int,f: ((String,Long,String)) => String)={

    val path=settings.RootPath+"data/projects/"+projectId+"/DBVersions/"+version+"/deltas/"+name
    var lineerror=""

    if(Files.exists(Paths.get(path))) {
      import scala.io.Source
      val source = Source.fromFile(path)(Codec.ISO8859)
      for (line <- source.getLines()){
        val fields = line.split(sep)

        lineerror=f(fields(0), fields(1).toLong, fields.drop(1).mkString(sep.toString))

      }
    }

    lineerror
  }

  def loadDeltaFile(name:String,projectId:Long,version:Int):Vector[(String,Long,String)]={

    val path=settings.RootPath+"data/projects/"+projectId+"/DBVersions/"+version+"/deltas/"+name

    if(Files.exists(Paths.get(path))) {
      (new FileUtils).readTextFromFile(path).map(line => {
        val fields = line.split(sep)

        (fields(0), fields(1).toLong, fields.drop(1).mkString(sep.toString))

      })
    }else{

      Vector[(String,Long,String)]()

    }


  }

  def loadVersionsSorted(project:Long,version:Int): Vector[Int]={

    val store=new StoreUtils(settings)

    store.projectVersions(project)
      /*.filter(v=>v<=version)*/
      .sorted
  }

  def checkVersions(versions:Vector[Int],version:Int):Boolean={

    var i=1
    var ok=true


    if(versions.distinct.length!=version){ok=false}

    while(i<versions.length && ok==true){
      if((versions(i)-versions(i-1))!=1){ ok=false }
      i=i+1
    }

    ok
  }



}
