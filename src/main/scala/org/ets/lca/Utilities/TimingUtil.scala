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

/**
  * Created by francoissaab on 3/19/17.
  */

object TimingUtil {
  
  var timing = new StringBuffer

  var latestTiming=""

  def reset={timing=new StringBuffer}

  def flush={
    val str=timing.toString
    println(timing.toString)
    reset
    str
  }

  def timed[T](label: String, code: => T): T = {
    val start = System.nanoTime()/1000
    val result = code
    val stop = System.nanoTime()/1000
    latestTiming=s"$label,${stop - start}\n"
    timing.append(latestTiming)
    //println(latestTiming)
    result
  }

}
