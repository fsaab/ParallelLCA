
#ifndef TwoDimStreamedVectorStore_hpp
#define TwoDimStreamedVectorStore_hpp
#include <stdio.h>
#include <vector>
#include "../../Utilities/FileUtils.hpp"
using namespace std;

#include <boost/algorithm/string/join.hpp>

class TwoDimStreamedVectorStore
{
  public:

  TwoDimStreamedVectorStore(){

  }

    std::vector<std::string> split(std::string input, std::string sep)
    {

        vector<string> strs;
        boost::split(strs, input, boost::is_any_of(sep));
        return strs;
    }

     void initStore(vector<vector<double>> &store, int iterations, int datasize)
    {

        (store).resize(iterations);
    }

     void splitIterationsDataInFiles(
        int iterationsSize,
        vector<vector<double>> &data,
        int dataSize,
        string path, long splitLength)
    {
        long splitCount = dataSize / splitLength;

       // cout << dataSize << " " << splitLength << " " << splitCount << " " << iterationsSize << endl;

        if (splitCount * splitLength < dataSize)
            splitCount++;

        vector<string> s;
        s.resize(splitCount);

        for (int i = 0; i < iterationsSize; i++)
        {
            for (int j = 0; j < splitCount; j++)
            {
                for (int k = j * splitLength; k < ((j * splitLength) + splitLength); k++)
                {
                    if (k == dataSize)
                        break;

                    s[j] += to_string(data[i][k]) + ",";
                }

                s[j] += "\n";
            }
        }


        for (int j = 0; j < splitCount; j++)
        {
            //save to file
            FileUtils fileUtils;
            fileUtils.writeTextToFile(s[j], path + "/" + to_string(j + 1));
        }
    }

   

    // static vector<vector<double>> getDataForAllIterationsAndDataSlot(int iterationssize,
    //                                                                  int datasize,
    //                                                                  int dataSlotNum,
    //                                                                  string dataSlotPath,
    //                                                                  vector<vector<double>> &store)
    // {

    //     FileUtils fileUtils;

    //     int i = 0;

    //     vector<string> paths;

    //     for (auto &&filepath : paths)
    //     {
    //         vector<string> lines = fileUtils.readTextFromFile(filepath);
    //         while (i < iterationssize)
    //         {
    //             vector<string> v = split(lines[i], ",");
    //             int j = 0;
    //             for (string s : v)
    //             {
    //                 store[i][j] = stod(s);
    //                 j++;
    //             }

    //             i++;
    //         }
    //     }

    //     return store;
    // }
};

#endif /* TwoDimStreamedVectorStore_hpp */
