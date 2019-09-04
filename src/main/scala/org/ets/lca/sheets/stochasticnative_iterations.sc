import LCAKernel.LCAKernel
import org.Native.Session
import org.ets.lca.DAL.{LCADBSet}
import org.ets.lca.LCACalculators.CalculatorData._
import org.ets.lca.LCACalculators.Calculation
import org.ets.lca.Utilities._
import org.ets.lca.LCACalculator
import org.ets.lca.Factories._

Session.loadLib("LCAKernel","0.1")



val settings = new CalculationSettings(234364 , 1.0,
  "", "alumprodQC42",
  "testagg_"+234364, 973039,
  true,true,5000,
  "./",123,3,30,false
)

val calc = new Calculation(settings.ProjectId,settings.Version)
new LCADataBaseFactoryNative(settings).create(calc)
val lcadb = LCADBSet.lcadbs(settings.ProjectId)

Vector(1000,5000,10000).foreach(iter=>{
println("iters,234364,"+iter)
val settings = new CalculationSettings(234364 , 1.0,
  "", "alumprodQC42",
  "testmulti_"+234364, 973039,
  true,true,iter,
  "./",123,3,1,false
)
val calc = new Calculation(settings.ProjectId,settings.Version)
val fileUtils=new FileUtils
fileUtils.rmfolder( settings.RootPath+ "data/calculations/"+ settings.CalculationId+"/")
fileUtils.newfolder( settings.RootPath+ "data/calculations/"+ settings.CalculationId+"/")
var main=(new LCACalculator(settings))

main.stochasticNative()
})