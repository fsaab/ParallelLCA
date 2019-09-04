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
package org.ets.lca.DAL
import scala.collection.mutable.HashMap
import LCAKernel.LCAKernel

class lcadb{

  var id:Long = -1

  var simdata:Long = -1

  var currentProject:Int = -1

  var currentVersion:Int = -1

  def resetDB(ProjectId:Long,hard:Boolean):Boolean = {

   
    if(hard && LCADBSet.lcadbs(ProjectId)!=null){
       if(LCADBSet.lcadbs(ProjectId).id>0) new LCAKernel().deletedb(
          LCADBSet.lcadbs(ProjectId).id)
    }

    LCADBSet.lcadbs(ProjectId) = null
    LCADBSet.lcadbs(ProjectId) = new lcadb()

   true

  }

}

