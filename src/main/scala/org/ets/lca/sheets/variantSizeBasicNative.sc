import LCAKernel.LCAKernel
import org.Native.Session
import org.ets.lca.DAL.{LCADBSet}
import org.ets.lca.LCACalculators.CalculatorData._
import org.ets.lca.LCACalculators.Calculation
import org.ets.lca.Utilities._
import org.ets.lca.LCACalculator
import org.ets.lca.Factories._
import scala.util.Try

Session.loadLib("LCAKernel","0.1")

def benchmark(num:Int, pr:Long,v:Int,iters:Int,par:Int)={

val settings = new CalculationSettings(593976, 1.0,
      "llll", "llll",
      "test_variantsize_"+pr, 973039,
      true,false, iters, 
      "./",pr,v,par,false
    )

val fileUtils=new FileUtils
fileUtils.newfolder( settings.RootPath+ "data/calculations/"+ "test_variantsize_"+pr+"/")

val calc = new Calculation(settings.ProjectId,settings.Version)
new LCADataBaseFactoryNative(settings).create(calc)
val lcadb = LCADBSet.lcadbs(settings.ProjectId)
var main=(new LCACalculator(settings))


Range(1,num+1).foreach(x=>{
Try{
main.staticNative()
//main.stochasticNative()
 // main.staticAndstochastic()
//main.staticBasicNative()
}


})

new LCADataBaseFactoryNative(settings).resetDB(true)

}

//Range(2,100,10).foreach(pr=>{benchmark(1,pr.toLong,1,1,1)})



//benchmark(1,162,1,1,1)

//Vector(2,42,162,502,752,1002).foreach(pr=>{benchmark(1,pr.toLong,1,1,1)})

