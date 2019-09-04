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

import java.nio.charset.Charset

import scala.collection.mutable
import scala.collection.mutable.HashMap

import java.nio.file.{Files, Paths}
import scala.collection.JavaConversions._
import scala.concurrent.forkjoin.ThreadLocalRandom

class DeltasGenerator {

  val maxprocid_init:Long=593975
  var maxprocid=maxprocid_init
  val procsDeltasStr=new StringBuilder
  var maxexchid_init=593982
  var maxexchid=maxexchid_init
  val exchDeltasStr=new StringBuilder
  val maxflowid_init=104491
  var flowsids:HashMap[Long,Vector[Long]]= new HashMap[Long,Vector[Long]]()  { override def default(key:Long) = Vector[Long]() }
  var maxflowid:Long=maxflowid_init
  val flowsDeltasStr=new StringBuilder
  var processids: HashMap[Long,Array[Long]]= new HashMap[Long,Array[Long]]()  { override def default(key:Long) = Array[Long]() }

  def init_root(number_flows:Int,background:Boolean)={

    var processes_ids_deltas= generate_layer_processes(1,1)
    val rootProccessId=maxprocid_init+1
    val root_inputexch_deltas=generate_process_input_exchanges(1,number_flows,rootProccessId,background,0)
    val root_outputexch_deltas=generate_process_output_exchanges(1,1,rootProccessId,false,0)
    processids(1)=processes_ids_deltas.unzip._1

  }

  def generate_layer_processes(layer:Long,number_processes:Int)= {

    println("number_processes"+number_processes)

    val processes_ids_deltas = Range(1, number_processes + 1).map(iter => {

      val line=Files.readAllLines(Paths.get("./data/sample/process.txt")).head

      var fields = line.split(29.toChar.toString)

      val newid = maxprocid + 1

      fields(1) = s"process_${layer}_${iter}"

      fields(0) = newid.toString
      fields(4) = "0"
      maxprocid = newid

      procsDeltasStr.append( "insert"+29.toChar.toString + fields.mkString(29.toChar.toString)).append(sys.props("line.separator"))

      (newid, "insert"+29.toChar.toString + fields.mkString(29.toChar.toString))

    }).toArray

    processids(layer)=processes_ids_deltas.unzip._1

    processes_ids_deltas
  }

  def generate_newflow(iter:Int,layer:Long,proc:Long): Unit ={

    val sampleflow = "1044111\u001DFlow\u001DPRODUCT_FLOW\u001D095528f2-b554-4cfb-9dc8-764b50bcbd7b"
    var sampleflowfields = sampleflow.split(29.toChar.toString)
    val newid=(maxflowid + 1)
    sampleflowfields(0) = newid.toString
    sampleflowfields(1) = sampleflowfields(1) + s"_$newid"

    flowsDeltasStr.append( "insert"+29.toChar.toString + sampleflowfields.mkString(29.toChar.toString) + sys.props("line.separator"))
    flowsids(layer)= flowsids(layer) :+ (maxflowid + 1)

    maxflowid = newid

  }

  def generate_process_output_exchanges(layer:Long,number:Int,processid:Long, reuse:Boolean,ind:Int)={


    val line= Files.readAllLines(Paths.get("./data/sample/exchange.txt")).head

    Range(1, number + 1).map(iter => {

      var newexchid = maxexchid +1

      if(!reuse) {
        generate_newflow(iter,layer,processid)
      }

      var fields = line.split(29.toChar.toString)

      val value = fields(4).toDouble
      val newval = 1//ThreadLocalRandom.current().nextDouble(value.toDouble - 1, value.toDouble + 1)

      val flowid= if(!reuse){
        maxflowid.toString
        }else{
        val newid_i = ind  //ThreadLocalRandom.current().nextInt(0, flowsids(layer-1).length-1)
        flowsids(layer-1)(newid_i).toString
      }

      fields(0) = newexchid.toString
      fields(1) = processid.toString
      fields(2) = flowid
      fields(3) = "0"
      fields(4) = newval.toString
      maxexchid = newexchid

      exchDeltasStr.append("insert"+29.toChar.toString + fields.mkString(29.toChar.toString)).append(sys.props("line.separator"))

      "insert"+29.toChar.toString + fields.mkString(29.toChar.toString)+sys.props("line.separator")

    })
  }

  val backgroundProductFlows={

    Files.readAllLines(Paths.get("./data/DBTemplates/1/flows.spark"),Charset.forName("ISO-8859-1"))
      .filter(line=>{

        line.split(29.toChar.toString)(2)=="PRODUCT_FLOW"


      }).map(x=>x.split(29.toChar.toString)(0)).toArray
  }

  def generate_process_input_exchanges(layer:Long,number:Int,processid:Long,background:Boolean,ind:Int)={

    val line= Files.readAllLines(Paths.get("./data/sample/exchange.txt")).head

    Range(1,number+1).map(iter=> {

      var newexchid = maxexchid +1

      if(!background){generate_newflow(iter,layer,processid)}

      var fields = line.split(29.toChar.toString)
      val value = fields(4).toDouble
      val newval = 1//ThreadLocalRandom.current().nextDouble(value.toDouble - 5, value.toDouble + 5)

      val newid= if(!background){
        maxflowid
      }else{
        val newid_i =ind//ThreadLocalRandom.current().nextInt(0, backgroundProductFlows.length-1)
        backgroundProductFlows(newid_i)
      }

      fields(0) =  newexchid.toString
      fields(1) =  processid.toString
      fields(2) =  newid.toString
      fields(3) =  newval.toString
      fields(4) =  newval.toString //newval.toString

      maxexchid=newexchid

      exchDeltasStr.append("insert"+29.toChar.toString + fields.mkString(29.toChar.toString)).append(sys.props("line.separator"))

      "insert"+29.toChar.toString + fields.mkString(29.toChar.toString)+sys.props("line.separator")
    })

  }

}