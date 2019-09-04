mpic++ -pipe  -D_JNI_IMPLEMENTATION_  -m64  -fPIC ./Native/LCAKernel_LCAKernel.cpp  -I./libs/ -I/opt/boost_1_67_0  -std=c++11 -O3 -fopenmp -DBOOST_SYSTEM_NO_DEPRECATED \
            -I/usr/lib/jvm/java-8-oracle/include/  \
            -I/usr/lib/jvm/java-8-oracle/include/linux/ \
            -L /opt/boost_1_67_0/stage/lib -lgsl -lgslcblas -lm  -lboost_mpi -lboost_serialization  -shared -o libLCAKernel.so


mpic++ -pipe  -D_JNI_IMPLEMENTATION_  -m64  -fPIC ./Native/Calculators_MPI/MontecarloCalculatorMPIMain.cpp -I./libs/ -I/opt/boost_1_67_0  -std=c++11 -O3 -fopenmp -DBOOST_SYSTEM_NO_DEPRECATED \
            -I/usr/lib/jvm/java-8-oracle/include/  \
            -I/usr/lib/jvm/java-8-oracle/include/linux/ \
            -L/opt/boost_1_67_0/stage/lib -lgsl -lgslcblas -lm  -lboost_mpi -lboost_serialization -march=native -DEIGEN_USE_BLAS -o processes/montecarlo
      