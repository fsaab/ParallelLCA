import breeze.linalg.{LSMR, CSCMatrix}
import org.ets.lca.Utilities.BreezeUtils
import org.ets.lca.Utilities

val builderA= new CSCMatrix.Builder[Double](rows=4,
  cols=4)

builderA.add(0,0,1)
builderA.add(0,1,1)
builderA.add(0,2,2)
builderA.add(0,3,-5)

builderA.add(1,0,2)
builderA.add(1,1,5)
builderA.add(1,2,-1)
builderA.add(1,3,-9)

builderA.add(2,0,2)
builderA.add(2,1,1)
builderA.add(2,2,-1)
builderA.add(2,3,3)

builderA.add(3,0,1)
builderA.add(3,1,3)
builderA.add(3,2,2)
builderA.add(3,3,7)


val f_vect=  breeze.linalg.DenseVector(Array(3.0,-3,-11,-5)).toDenseVector


val A_sparse=builderA.result()
val iter:Int=1000*1000*1000
val sol = (new BreezeUtils()).solveSparseLeastSquaredLocal(A_sparse,f_vect).toArray
sol