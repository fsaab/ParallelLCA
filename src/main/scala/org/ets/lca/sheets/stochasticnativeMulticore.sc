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


val settings = new CalculationSettings(234364 , 1.0,
  "", "alumprodQC42",
  "testmulti_"+234364, 973039,
  true,true,5000,
  "./",123,1,30,false
)

val calc = new Calculation(settings.ProjectId,settings.Version)
new LCADataBaseFactoryNative(settings).create(calc)
val lcadb = LCADBSet.lcadbs(settings.ProjectId)


def becnhmark(n:Int,iterations:Int, p:Int){  

  Range(1,n+1).foreach(t=>{
    
  

   println("cores,234364,"+p)

    val settings = new CalculationSettings(234364 , 1.0,
    "", "alumprodQC42",
    "testmulti_"+234364+"_"+iterations, 973039,
    true,true,closestUpperNumber(iterations,p),
    "./",123,1,p,true)

  val calc = new Calculation(settings.ProjectId,settings.Version)
  val fileUtils=new FileUtils
  fileUtils.rmfolder( settings.RootPath+ "data/calculations/"+ settings.CalculationId+"/")
  fileUtils.newfolder( settings.RootPath+ "data/calculations/"+ settings.CalculationId+"/")
  var main=(new LCACalculator(settings))

  main.stochasticNative()


  })

}

//Range(1,35,2).foreach(p=>{becnhmark(1,1000,p) })
//Vector(1,5,10,15,20,22,24,26,28).foreach(p=>{becnhmark(1,1000,p) })

//becnhmark(1,1000,p) 