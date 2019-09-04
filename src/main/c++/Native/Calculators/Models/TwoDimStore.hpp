
#ifndef TwoDimStore_hpp
#define TwoDimStore_hpp


#include "boost/multi_array.hpp"


class TwoDimStore{
    
public:
    
//    static double ** initStore( int iterations,int datasize);
//    static void putDataAtIteration (double ** store,int iteration,double * data,int datasize);
//    static double * getDataForIteration(double ** store,int iter);
//    static double * getDataForAllIterations(double ** store,int iterationssize,int inputindex);
    

    
    static double ** initStore( int iterations,int datasize){
        
        double** store=new double*[iterations];
        
        int i=0;
        
        while(i<iterations){
            
            store[i]=new double[datasize];
            i++;
        }
        
        return store;
        
    }
    
    static void putDataAtIteration (double ** store,int iteration,double * data,int datasize){
        
        int i=0;
        
        while(i<datasize){
            
            store[iteration][i]=data[i];
            
            i++;
        }
        
        
    }
    
    
    static double * getDataForIteration(double ** store,int iter){
        
        return store[iter];
        
    }
    
    
    static double * getDataForAllIterations(double ** store,int iterationssize,int inputindex){
        
        double* data=new double[iterationssize];
        
        int i=0;
        
        
        while(i<iterationssize){
            
            data[i]=store[i][inputindex];
            i++;
        }
        
        return data;
    }
    
    
};


#endif /* TwoDimStore_hpp */



