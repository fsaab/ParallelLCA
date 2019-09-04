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

import org.ets.lca.LCACalculators.CalculatorData.CalculationSettings
import org.ets.lca.Utilities.FileUtils


/**
  * Created by francoissaab on 1/9/18.
  */
class StoreUtils(settings:CalculationSettings) {

  def DBTemplates() :Vector[Int]={

    val fileUtils=new FileUtils

    fileUtils.getListOfSubDirectories(settings.RootPath+"data/DBTemplates/").map(_.toInt).toVector

  }


  def checkProject(project:Long): Boolean={


    projects().find(_==project).size==1

  }

  def projects():Vector[Long]={

    val fileUtils=new FileUtils

   fileUtils.getListOfSubDirectories(settings.RootPath+"data/projects/").map(_.toLong).toVector



  }

  def projectVersions(projectId:Long):Vector[Int]={

    val fileUtils=new FileUtils

    fileUtils.getListOfSubDirectories(settings.RootPath+"data/projects/"+projectId+"/DBVersions/").map(_.toInt).toVector



  }

}
