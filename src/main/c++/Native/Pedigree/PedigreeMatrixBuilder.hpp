/**---------------------------   
 * PROJECT: ParallelLCA
 * Auth:
 *   Francois Saab
 * Mail: saab.francois@gmail.com, francois.saab.1@ens.etsmtl.ca
 * Date: 1/1/2017
 *
 * Copyright © 2017 Francois Saab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. 
 *
 *--------------------------*/
#ifndef PedigreeMatrixBuilder_H
#define PedigreeMatrixBuilder_H


using namespace std;

#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include "includes.h"

using namespace std;
class PedigreeMatrixBuilder
{

public:
  PedigreeMatrixBuilder()
  {
  }

  vector<string> pedigreeSet = {"1", "2", "3", "4", "5"};
  ReliabilityObj reliabilityObj = ReliabilityObj();
  CompletenessObj completenessObj = CompletenessObj();
  TemporalCorrelationObj temporalCorrelationObj = TemporalCorrelationObj();
  GeographicalCorrelationObj geographicalCorrelationObj = GeographicalCorrelationObj();
  TechnologicalCorrelationObj technologicalCorrelationObj = TechnologicalCorrelationObj();
  SampleSizeObj sampleSizeObj = SampleSizeObj();
  
  Reliability reliability = reliabilityObj.NA;
  Completeness completeness = completenessObj.NA;
  TemporalCorrelation temporalCorrelation = temporalCorrelationObj.NA;
  GeographicalCorrelation geographicalCorrelation = geographicalCorrelationObj.NA;
  TechnologicalCorrelation technologicalCorrelation = technologicalCorrelationObj.NA;
  SampleSize sampleSize = sampleSizeObj.NA;

  double basicUncertainty = 1.05;

  void setBasicUncertainty(double v)
  {
    basicUncertainty = v;
  }

  string getPedigreeCommentString()
  {
    string matrix;

    matrix += "(";
    matrix += reliability.getKey();
    matrix += ";";
    matrix += completeness.getKey();
    matrix += ";";
    matrix += temporalCorrelation.getKey();
    matrix += ";";
    matrix += geographicalCorrelation.getKey();
    matrix += ";";
    matrix += technologicalCorrelation.getKey();
    matrix += ";";
    matrix += sampleSize.getKey();
    matrix += ")";

    return matrix;
  }

  vector<double> getUncertaintyLogNormal(double mu)
  {

    double u1 = reliability.getVariance();              /*Math.pow(Math.log(reliability.getIndicator), 2)*/
    double u2 = completeness.getVariance();             /*Math.pow(Math.log(completeness.getIndicator), 2)*/
    double u3 = temporalCorrelation.getVariance();      /*Math.pow(Math.log(temporalCorrelation.getIndicator), 2)*/
    double u4 = geographicalCorrelation.getVariance();  /*Math.pow(Math.log(geographicalCorrelation.getIndicator), 2)*/
    double u5 = technologicalCorrelation.getVariance(); /*Math.pow(Math.log(technologicalCorrelation.getIndicator), 2)*/
    double u6 = sampleSize.getVariance();               /*Math.pow(Math.log(sampleSize.getIndicator), 2)*/
    double ub = pow(log(basicUncertainty), 2);
    double uncert = exp(sqrt(u1 + u2 + u3 + u4 + u5 + u6 + ub));
    vector<double> unc = {mu, uncert};
    return unc;
  }

  vector<double> getUncertaintyNormal(double mu)
  {

    double u1 = reliability.getVariance();              /*Math.pow(Math.log(reliability.getIndicator), 2)*/
    double u2 = completeness.getVariance();             /*Math.pow(Math.log(completeness.getIndicator), 2)*/
    double u3 = temporalCorrelation.getVariance();      /*Math.pow(Math.log(temporalCorrelation.getIndicator), 2)*/
    double u4 = geographicalCorrelation.getVariance();  /*Math.pow(Math.log(geographicalCorrelation.getIndicator), 2)*/
    double u5 = technologicalCorrelation.getVariance(); /*Math.pow(Math.log(technologicalCorrelation.getIndicator), 2)*/
    double u6 = sampleSize.getVariance();               /*Math.pow(Math.log(sampleSize.getIndicator), 2)*/
    double ub = pow(log(basicUncertainty), 2);
    double uncert = (sqrt(u1 + u2 + u3 + u4 + u5 + u6 + ub));
    vector<double> unc = {mu, uncert};
    return unc;
  }

  void setCompleteness(string pedigree)
  {

    if (std::find(pedigreeSet.begin(), pedigreeSet.end(), pedigree) != pedigreeSet.end())
    {

      switch (std::stoi(pedigree))
      {
      case 1:
        completeness = completenessObj.ONE;
        break;
      case 2:
        completeness = completenessObj.TWO;
        break;
      case 3:
        completeness = completenessObj.THREE;
        break;
      case 4:
        completeness = completenessObj.FOUR;
        break;
      case 5:
        completeness = completenessObj.FIVE;
        break;
      }
    }
    else
    {
      completeness = completenessObj.NA;
    }
  }

  void setTechnologicalCorrelation(string pedigree)
  {
    if (std::find(pedigreeSet.begin(), pedigreeSet.end(), pedigree) != pedigreeSet.end())
    {

      switch (std::stoi(pedigree))
      {
      case 1:
        technologicalCorrelation = technologicalCorrelationObj.ONE;
        break;
      case 2:
        technologicalCorrelation = technologicalCorrelationObj.TWO;
        break;
      case 3:
        technologicalCorrelation = technologicalCorrelationObj.THREE;
        break;
      case 4:
        technologicalCorrelation = technologicalCorrelationObj.FOUR;
        break;
      case 5:
        technologicalCorrelation = technologicalCorrelationObj.FIVE;
        break;
      }
    }
    else
    {
      technologicalCorrelation = technologicalCorrelationObj.NA;
    }
  }

  void setReliability(string pedigree)
  {
    if (std::find(pedigreeSet.begin(), pedigreeSet.end(), pedigree) != pedigreeSet.end())
    {

      switch (std::stoi(pedigree))
      {
      case 1:
        reliability = reliabilityObj.ONE;
        break;
      case 2:
        reliability = reliabilityObj.TWO;
        break;
      case 3:
        reliability = reliabilityObj.THREE;
        break;
      case 4:
        reliability = reliabilityObj.FOUR;
        break;
      case 5:
        reliability = reliabilityObj.FIVE;
        break;
      }
    }
    else
    {
      reliability = reliabilityObj.NA;
    }
  }

  void setSampleSize(string pedigree)
  {
    if (std::find(pedigreeSet.begin(), pedigreeSet.end(), pedigree) != pedigreeSet.end())
    {

      switch (std::stoi(pedigree))
      {
      case 1:
        sampleSize = sampleSizeObj.ONE;
        break;
      case 2:
        sampleSize = sampleSizeObj.TWO;
        break;
      case 3:
        sampleSize = sampleSizeObj.THREE;
        break;
      case 4:
        sampleSize = sampleSizeObj.FOUR;
        break;
      case 5:
        sampleSize = sampleSizeObj.FIVE;
        break;
      }
    }
    else
    {
      sampleSize = sampleSizeObj.NA;
    }
  }
  void setTemporalCorrelation(string pedigree)
  {
    if (std::find(pedigreeSet.begin(), pedigreeSet.end(), pedigree) != pedigreeSet.end())
    {

      switch (std::stoi(pedigree))
      {
      case 1:
        temporalCorrelation = temporalCorrelationObj.ONE;
        break;
      case 2:
        temporalCorrelation = temporalCorrelationObj.TWO;
        break;
      case 3:
        temporalCorrelation = temporalCorrelationObj.THREE;
        break;
      case 4:
        temporalCorrelation = temporalCorrelationObj.FOUR;
        break;
      case 5:
        temporalCorrelation = temporalCorrelationObj.FIVE;
        break;
      }
    }
    else
    {
      temporalCorrelation = temporalCorrelationObj.NA;
    }
  }

  void setGeographicalCorrelation(string pedigree)
  {
    if (std::find(pedigreeSet.begin(), pedigreeSet.end(), pedigree) != pedigreeSet.end())
    {

      switch (std::stoi(pedigree))
      {
      case 1:
        geographicalCorrelation = geographicalCorrelationObj.ONE;
        break;
      case 2:
        geographicalCorrelation = geographicalCorrelationObj.TWO;
        break;
      case 3:
        geographicalCorrelation = geographicalCorrelationObj.THREE;
        break;
      case 4:
        geographicalCorrelation = geographicalCorrelationObj.FOUR;
        break;
      case 5:
        geographicalCorrelation = geographicalCorrelationObj.FIVE;
        break;
      }
    }
    else
    {
      geographicalCorrelation = geographicalCorrelationObj.NA;
    }
  }
};

#endif /* PedigreeUtils_H */
