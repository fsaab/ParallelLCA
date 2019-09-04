#move compiled c++ to jar location, build jar, move jar into service location 
root=/app
calculatorproject=UBUBI-LCA-Calculator-Native
module=Calculators
echo " compiling c++ to:" 
echo $root/$calculatorproject/src/main/c++/Native/$module/libLCAKernel.so

mpic++ -pipe  -D_JNI_IMPLEMENTATION_  -m64   -fPIC \
  $root/$calculatorproject/src/main/c++/Native/$module/LCAKernel_LCAKernel.cpp \
  -I$root/external/libs \
  -I/app/external/libs/boost_1_67_0/ -std=c++11  -O3 -fopenmp -DBOOST_SYSTEM_NO_DEPRECATED \
  -I/usr/lib/jvm/java-8-openjdk-amd64/include/  \
  -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux/ \
   -lgsl -lgslcblas -lm   -shared -o $root/$calculatorproject/src/main/c++/Native/$module/libLCAKernel.so


#-lboost_mpi -lboost_serialization
#-I/usr/lib/jvm/java-8-oracle/include/  \
#-I/usr/lib/jvm/java-8-oracle/include/linux/ \
#-lboost_mpi -lboost_serialization
