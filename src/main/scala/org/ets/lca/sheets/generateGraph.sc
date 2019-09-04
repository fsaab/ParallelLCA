import org.ets.lca.Utilities.{DeltasGenerator, FileUtils}

import scala.collection.mutable.HashMap

def generate(proj:Int): Unit ={

  // val lastlayer_processes=13831
  //val offset_layer=5
  val inflowsPerProcess=1
  val inflowsPerRoot=1000 //13831

  val gen= new DeltasGenerator
  val max_layers=proj

  gen.init_root(inflowsPerRoot,false)

  Range(2,max_layers+1).foreach(l=>{

    val layer=l.toLong

    println("layer "+ layer)

    val num_processes_layer=inflowsPerRoot

    //  {
    //   if (layer >= offset_layer) {
    //     lastlayer_processes
    //   }
    //   else  {
    //     inflowsPerRoot
    //   }
    // }

    /*LAYER */
    gen.generate_layer_processes(layer,num_processes_layer)

    // if (layer == offset_layer-1) {
    //   lastlayer_processes/inflowsPerRoot
    // }
    // else{inflowsPerProcess}

    val processes_input_exchanges_deltas=gen.processids(layer).zipWithIndex.flatMap(p=>{
      gen.generate_process_input_exchanges(layer,inflowsPerProcess, p._1, if(layer<max_layers)false else true ,p._2)
    })

    val processes_out_exchanges_deltas=gen.processids(layer).zipWithIndex.flatMap(p=>{
      gen.generate_process_output_exchanges(layer,1,p._1,true,p._2)
    })

  })

  (new FileUtils).newfolder("./data/gen2_projects/")
  (new FileUtils).newfolder("./data/gen2_projects/"+proj)
  (new FileUtils).newfolder("./data/gen2_projects/"+proj+"/DBVersions")
  (new FileUtils).newfolder("./data/gen2_projects/"+proj+"/DBVersions/1")
  (new FileUtils).newfolder("./data/gen2_projects/"+proj+"/DBVersions/1/deltas")
  (new FileUtils).writeTextToFile(gen.procsDeltasStr.toString(),"./data/gen2_projects/"+proj+"/DBVersions/1/deltas/ProcessesDiff.txt")
  (new FileUtils).writeTextToFile(gen.exchDeltasStr.toString(),"./data/gen2_projects/"+proj+"/DBVersions/1/deltas/ExchangesDiff.txt")
  (new FileUtils).writeTextToFile(gen.flowsDeltasStr.toString(),"./data/gen2_projects/"+proj+"/DBVersions/1/deltas/FlowsDiff.txt")

}

//val maxprojects=200
//Range(2,maxprojects+1,10).foreach(proj=>{generate(proj)})

//generate(1)
