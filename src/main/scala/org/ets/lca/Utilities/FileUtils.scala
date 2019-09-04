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

import java.io.{FileWriter, BufferedWriter, File}

import scala.io.Codec

/**
  * Created by francoissaab on 5/8/17.
  */
class FileUtils {

  def checkFile(path:String)= {



    val file = new File(path)

    file.exists()

  }
  def newfolder(path:String)={



    val dir = new File(path)

    // attempt to create the directory here
    val successful = dir.mkdir();


  }
  def rmfolder(path:String)={

    val  dir = new File(path)

    // attempt to create the directory here
    dir.delete();

  }
  def writeTextToFile(text:String,path:String)={


    try {
      val file = new File(path)
      file.createNewFile()
      import java.io.FileWriter
      val writer = new FileWriter(file)

      writer.write(text)

     /* val bw = new BufferedWriter(new FileWriter(file))
      bw.write(text)
      bw.close()*/

      writer.close()
    } catch {
      case e: Exception => {
        println(e.getMessage+"--> "+path)
      }

    }
    finally {

    }

  }
  def readTextFromFile(path:String)={

    import scala.io.Source

    Source.fromFile(path)(Codec.ISO8859).getLines.toVector

  }

  def getListOfSubDirectories(directoryName: String): Array[String] = {


    val subDirs:Array[String]=

      try{
        (new File(directoryName)).listFiles.filter(_.isDirectory).map(_.getName)
      }catch{

        case e: Exception => {

          val empty:Array[String]=Array()

          empty

        }


      }

    subDirs
  }



}
