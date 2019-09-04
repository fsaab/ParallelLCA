package LCAKernel;

/**
 * Created by francoissaab on 2/26/18.
 */
public class LCAKernel {

    public native String createdb(int RootProcessId,double OutputQunatity,
                                String SolvingMethod,String SystemId,String CalculationId,int ImpactMethodId,
                                int  lcia,int montecarlo,int montecarlo_iterations,int Paralleism,
                                String RootPath,long ProjectId,int Version

    );
    public native boolean deletedb(long dbptr);

    public native String updatedb( long dbptr,

                                    int RootProcessId,double OutputQunatity,
                                    String SolvingMethod,String SystemId,String CalculationId,int ImpactMethodId,
                                    int  lcia,int montecarlo,int montecarlo_iterations,int Paralleism,
                                    String RootPath,long ProjectId,int Version

           );
  

    public native double[] QBAinverse(int RootProcessId,double OutputQunatity,
    String SolvingMethod,String SystemId,String CalculationId,int ImpactMethodId,
    int  lcia,int montecarlo,int montecarlo_iterations,int Paralleism,
    String RootPath,long ProjectId,int Version,long dbptr,long simdata
    );


    public native String libUpstreamLCIA(int RootProcessId,double OutputQunatity,
                                      String SolvingMethod,String SystemId,String CalculationId,int ImpactMethodId,
                                      int  lcia,int montecarlo,int montecarlo_iterations,int Paralleism,
                                      String RootPath,long ProjectId,int Version,long dbptr,long simdata
    );

    public native double[] libUpstreamUncertainty(int RootProcessId,double OutputQunatity,
                                      String SolvingMethod,String SystemId,String CalculationId,int ImpactMethodId,
                                      int  lcia,int montecarlo,int montecarlo_iterations,int Paralleism,
                                      String RootPath,long ProjectId,int Version,long dbptr,long simdata
//                                                  String[]exchanges,
//                                                  String[]impactfactors,
//                                                  String[] parameters

    );

    public native String staticAndstochastic(int RootProcessId,double OutputQunatity,
    String SolvingMethod,String SystemId,String CalculationId,
    int ImpactMethodId,int  lcia,int montecarlo,int montecarlo_iterations,
    int Paralleism,String RootPath,long ProjectId,int Version, long dbptr,long simdata,int sensitivity
                                  /*String[]exchanges,
                                  String[]impactfactors,
                                  String[] parameters*/
    );

    public native String stochastic(int RootProcessId,double OutputQunatity,
    String SolvingMethod,String SystemId,String CalculationId,
    int ImpactMethodId,int  lcia,int montecarlo,int montecarlo_iterations,
    int Paralleism,String RootPath,long ProjectId,int Version, long dbptr,long simdata,int sensitivity
                                  /*String[]exchanges,
                                  String[]impactfactors,
                                  String[] parameters*/
    );

    public native String static_calc (int RootProcessId,double OutputQunatity,
                                  String SolvingMethod,String SystemId,String CalculationId,
                                  int ImpactMethodId,int  lcia,int montecarlo,int montecarlo_iterations,
                                  int Paralleism,String RootPath,long ProjectId,int Version,
                                    long dbptr,long simdata
                                    /*String[]exchanges,
                                    String[]impactfactors,
                                    String[] parameters*/);

    public native String static_basic_calc (int RootProcessId,double OutputQunatity,
                                      String SolvingMethod,String SystemId,String CalculationId,
                                      int ImpactMethodId,int  lcia,int montecarlo,int montecarlo_iterations,
                                      int Paralleism,String RootPath,long ProjectId,int Version,
                                      long dbptr,long simdata
                                    /*String[]exchanges,
                                    String[]impactfactors,
                                    String[] parameters*/);

}
