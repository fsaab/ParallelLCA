import LCAKernel.LCAKernel
import org.Native.Session
import org.ets.lca.DAL.{LCADBSet}
import org.ets.lca.LCACalculators.CalculatorData._
import org.ets.lca.LCACalculators.Calculation
import org.ets.lca.Utilities._
import org.ets.lca.LCACalculator
import org.ets.lca.Factories._

Session.loadLib("LCAKernel","0.1")

val settings = new CalculationSettings(234364, 1.0,
  "solve-local-sparse-umfpack", "alumprodQC42",
  "testlci_27jan_", 973039,
  true,false,100,
  "./",123,3,10,false
)

val calc = new Calculation(settings.ProjectId,settings.Version)

new LCADataBaseFactoryNative(settings).create(calc)

val lcadb = LCADBSet.lcadbs(settings.ProjectId)

val fileUtils=new FileUtils

val proclocs=fileUtils.readTextFromFile("data/DBTemplates/1/locproc.txt")
                          .map(line=>{
                            val arr=line.split(",")
                             (arr(0).toLong,arr(1))
                          }).toMap

val procNamesLocs=fileUtils.readTextFromFile("data/DBTemplates/1/processes.spark")
                          .map(line=>{
                            val arr=line.split(29.toChar)
                             (arr(0).toLong,arr(1),proclocs(arr(0).toLong))
                          })

def basic(n:Int){
 
procNamesLocs.take(n).foreach(p=>{

  val name=p._2.substring(0,p._2.indexOf("|")).trim()

  val settings = new CalculationSettings(234364, 1.0,
    "solve-local-sparse-umfpack", "alumprodQC42",
    name+"_"+p._3, 973039,
    true,false,100,
    "./",123,3,10,false
    )

    fileUtils.newfolder( settings.RootPath+ "data/calculations/"+ name+"_"+p._3+"/")

    var main=(new LCACalculator(settings))

    main.staticBasicNative()

  })
}