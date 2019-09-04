package LCAKernel;

/**
 * Created by francoissaab on 2/26/18.
 */
public class LCAKernel_bk {


    // --- Native methods

    public native double[] QBAinverse(int RootProcessId,double OutputQunatity,
    String SolvingMethod,String SystemId,String CalculationId,int ImpactMethodId,
    int  lcia,int montecarlo,int montecarlo_iterations,int Paralleism,
    String RootPath,long ProjectId,int Version,
    long[] Ai,long[]Aj,String[]A,
    long[] Bi,long[]Bj,String[]B,
    long[] Qi,long[]Qj,String[]Q,
    String[] Pname,String[] P);

    public native double[] libUpstreamUncertainty(int RootProcessId,double OutputQunatity,
                                      String SolvingMethod,String SystemId,String CalculationId,int ImpactMethodId,
                                      int  lcia,int montecarlo,int montecarlo_iterations,int Paralleism,
                                      String RootPath,long ProjectId,int Version,
                                      long[] Ai,long[]Aj,String[]A,
                                      long[] Bi,long[]Bj,String[]B,
                                      long[] Qi,long[]Qj,String[]Q,
                                      String[] Pname,String[] P);

    public native void stochastic(int RootProcessId,double OutputQunatity,
    String SolvingMethod,String SystemId,String CalculationId,
    int ImpactMethodId,int  lcia,int montecarlo,int montecarlo_iterations,
    int Paralleism,String RootPath,long ProjectId,int Version,
    long[] Ai,long[]Aj,String[]A,
    long[] Bi,long[]Bj,String[]B,
    long[] Qi,long[]Qj,String[]Q,
    String[] Pname,String[] P);

    public native void static_calc (int RootProcessId,double OutputQunatity,
                                  String SolvingMethod,String SystemId,String CalculationId,
                                  int ImpactMethodId,int  lcia,int montecarlo,int montecarlo_iterations,
                                  int Paralleism,String RootPath,long ProjectId,int Version,
                                  long[] Ai,long[]Aj,String[]A,
                                  long[] Bi,long[]Bj,String[]B,
                                  long[] Qi,long[]Qj,String[]Q,
                                  String[] Pname,String[] P);

}
