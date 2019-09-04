import LCAKernel.LCAKernel
import org.Native.Session
import org.ets.lca.DAL.{LCADBSet}
import org.ets.lca.LCACalculators.CalculatorData._
import org.ets.lca.LCACalculators.Calculation
import org.ets.lca.Utilities._
import org.ets.lca.LCACalculator
import org.ets.lca.Factories._

Session.loadLib("LCAKernel","0.1")

  def closestUpperNumber(n: Int, m: Int): Int = { // find the quotient
    if(n%m==0) n
    var i=n
    while(i%m!=0){
      i=i+1

    }
    i
  }

// val settings = new CalculationSettings( 1154251, 1.0,
//   "solve-local-sparse-umfpack", "alumprodQC42",
//   "test_"+1154251, 973039,
//   true,true,1000,
//   "./",7000,3,10,false
// )

// val settings = new CalculationSettings(1134151 , 1.0,
//   "solve-local-sparse-umfpack", "alumprodQC42",
//   "testagg_"+1134151, 973039,
//   true,true,10000,
//   "./",1100000,1,30,false
// )

val par=10

val settings = new CalculationSettings(234364 , 1.0,
  "", "alumprodQC42",
  "testmulti_"+234364, 973039,
  true,true,closestUpperNumber(1000,par),
  "./",123,1,par,true
)
val calc = new Calculation(settings.ProjectId,settings.Version)
new LCADataBaseFactoryNative(settings).create(calc)
val lcadb = LCADBSet.lcadbs(settings.ProjectId)

def benchmark(n:Int,iters:Int,par:Int)={

println("iterations,0,0,"+iters)
  
val settings = new CalculationSettings(234364 , 1.0,
  "", "alumprodQC42",
  "testmulti_"+234364, 973039,
  true,true,closestUpperNumber(iters,par),
  "./",123,1,par,true
)

val fileUtils=new FileUtils
fileUtils.rmfolder( settings.RootPath+ "data/calculations/"+ settings.CalculationId+"/")
fileUtils.newfolder( settings.RootPath+ "data/calculations/"+ settings.CalculationId+"/")
var main=(new LCACalculator(settings))
main.stochasticNative()

//main.staticAndstochastic

//main.staticNative()
}

//benchmark(1,100,1)

//Range(1000,10000,1000).foreach(x=>benchmark(1,x,35))