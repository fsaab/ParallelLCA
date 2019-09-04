import LCAKernel.LCAKernel
import org.Native.Session
import org.ets.lca.DAL.{LCADBSet}
import org.ets.lca.LCACalculators.CalculatorData._
import org.ets.lca.LCACalculators.Calculation
import org.ets.lca.Utilities._
import org.ets.lca.LCACalculator
import org.ets.lca.Factories._

Session.loadLib("LCAKernel","0.1")

 var settings = new CalculationSettings(234364, 1.0,
          "", "",
          "test_"+234364, 973039,
          true,false,100,
          "./",123,1,1,false
        )

 val calc = new Calculation(settings.ProjectId,settings.Version)

  new LCADataBaseFactoryNative(settings).create(calc)

  val lcadb = LCADBSet.lcadbs(settings.ProjectId)

  val fileUtils=new FileUtils

  val procids=fileUtils.readTextFromFile("data/DBTemplates/1/processes.spark")
                                  .map(line=>{
                                    val arr=line.split(29.toChar)
                                    arr(0).toLong
                                  })

def run(times:Int,count:Int)={
          
       

        procids.take(count).foreach(p=>{

          Range(0,times).foreach(t=>{
                  

                fileUtils.newfolder( settings.RootPath+ "data/calculations/"+ "test_"+p+"/")


                settings = new CalculationSettings(p.toInt, 1.0,
                                                          "", "",
                                                          "test_"+p, 973039,
                                                          true,false,100,
                                                          "./",123,1,35,false
                                                        )


                var main=(new LCACalculator(settings))

                main.staticNative()

          })
            
        })

}

//run(5,20)
                      
