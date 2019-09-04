root=app
calculatorproject=UBUBI-LCA-Calculator-Native 
echo "compiling c++, for Aggregated Calculator..." 

mpic++ -pipe  -D_JNI_IMPLEMENTATION_  -m64   -fPIC \
  /$root/$calculatorproject/src/main/c++/Native/Calculators_Aggregated/AggregatedMPIMain.cpp \
  -I/$root/external/libs \
  -I/app/external/libs/boost_1_67_0/ -std=c++11  -O3 -fopenmp -DBOOST_SYSTEM_NO_DEPRECATED \
  -I/usr/lib/jvm/java-8-oracle/include/  \
  -I/usr/lib/jvm/java-8-oracle/include/linux/ \
   -lgsl -lgslcblas -lm -lboost_mpi -lboost_serialization   -o /$root/$calculatorproject/processes/montecarlo