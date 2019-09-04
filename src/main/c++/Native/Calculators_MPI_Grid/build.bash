root=/opt/workspace
calculatorproject=UBUBI-LCA-Calculator-Native 
echo " compiling c++... " 

mkdir processes

mpic++ -pipe  -D_JNI_IMPLEMENTATION_  -m64  -fPIC ./src/main/c++/Native/Calculators_MPI_GRID/MontecarloCalculatorMPIMain.cpp -I./libs/ -I/opt/boost_1_67_0  -std=c++11 -O3 -fopenmp -DBOOST_SYSTEM_NO_DEPRECATED \
            -I$root/external/libs/ \
            -I/usr/lib/jvm/java-8-oracle/include/  \
            -I/usr/lib/jvm/java-8-oracle/include/linux/ \
            -L/opt/boost_1_67_0/stage/lib -lgsl -lgslcblas -lm  -lboost_mpi -lboost_serialization -march=native -DEIGEN_USE_BLAS -o processes/montecarlo
      
