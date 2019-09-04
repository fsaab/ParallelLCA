
#ifndef TwoDimVectorStore_hpp
#define TwoDimVectorStore_hpp
#include <stdio.h>
#include <vector>
using namespace std;
class TwoDimVectorStore {
public:

    //    static double ** initStore( int iterations,int datasize);
    //    static void putDataAtIteration (double ** store,int iteration,double * data,int datasize);
    //    static double * getDataForIteration(double ** store,int iter);
    //    static double * getDataForAllIterations(double ** store,int iterationssize,int inputindex);

    static void initStore(vector< vector<double > > & store, int iterations, int datasize) {

        (store).resize(iterations);

        //        int i=0;
        //        
        //        while(i<iterations){
        //            
        //             (*store)[i].resize(datasize);
        //            i++;
        //        }

    }

     static void putDataAtIteration(vector< vector<double> >& store, int iteration, double * data, int datasize) {

        int i = 0;

        while (i < datasize) {

            (store)[iteration].push_back(data[i]);

            i++;
        }


    }

    static vector<double> getDataForIteration(vector<vector<double> >& store, int iter) {

        return (store)[iter];

    }

    static double* getDataForAllIterations(vector<vector<double> > & store, int iterationssize, int inputindex) {

        double* data=new double[iterationssize];

        int i = 0;

        while (i < iterationssize) {
            //cout<< (*store)[i][inputindex] << " , ";
            data[i]=((store)[i][inputindex]);
            i++;
        }

        return data;
    }


};


#endif /* TwoDimVectorStore_hpp */



