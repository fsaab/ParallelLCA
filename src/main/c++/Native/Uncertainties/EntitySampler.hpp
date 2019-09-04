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
#ifndef EntitySampler_hpp
#define EntitySampler_hpp

#include "../LCAModels/ExchangeItem.hpp"
#include <stdio.h>
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "Lognormal.hpp"
#include "Normal.hpp"
#include "Triangular.hpp"
#include "Uniform.hpp"
#include <iostream>
#include "omp.h"
#include <functional>
#include <boost/range/algorithm.hpp>
#include "../includes/exprtk.hpp"

typedef exprtk::expression<double> expression_t;
typedef exprtk::symbol_table<double> symbol_table_t;

template <typename T>
class EntitySampler
{
  public:
    std::function<
        double(std::default_random_engine &, std::normal_distribution<float>)>
        f1 = Lognormal::next;

    std::function<
        double(std::default_random_engine &, std::normal_distribution<float>)>
        f2 = Normal::next;

    vector<
        std::function<
            double(std::default_random_engine &, std::normal_distribution<float>)>>
        v_funcs{f1, f2, f1, f1};

    EntitySampler()
    {
    }

    double eval(std::string formulae, expression_t &expressiont)
    {
        typedef exprtk::parser<T> parser_t;
        parser_t parser;
        parser.compile(formulae, expressiont);
        T y = expressiont.value();

        return y;
    }

    double getExpressionValue(string expr, double value, expression_t expressionsTable)
    {
        double res = (expr == "" ? value : eval(expr, expressionsTable));
        return res;
    }

    double getCellValue(Exchange &c)
    {
        return (c.amount * c.conversionFactor * (c.input ? -1 : 1));
    }

    double getCellValue(Exchange &c, expression_t &expressiontbl)
    {
        double value = getExpressionValue(c.amountFormula, c.amount, expressiontbl);
        int sign = (c.input ? -1 : 1);
        return value * c.conversionFactor * sign;
    }

    double getNextSimulatedCellValue(ExchangeItem *c, std::default_random_engine &gen)
    {
        int sign = ((*c).exch.input ? -1 : 1);

        switch ((*c).exch.uncertaintyType)
        {
        case 1:

            return exp((*c).distribution(gen)) * (*c).exch.conversionFactor * sign;

            break;

        case 2:

            return (*c).distribution(gen) * (*c).exch.conversionFactor * sign;

            break;

        case 3:

            return Triangular::next((*c).exch.parameter1, (*c).exch.parameter2, (*c).exch.parameter3) * (*c).exch.conversionFactor * sign;

            break;

        case 4:

            return Uniform::next((*c).exch.parameter1, (*c).exch.parameter2) * (*c).exch.conversionFactor * sign;

            break;

        default:

            return (*c).exch.amount * (*c).exch.conversionFactor * sign;

            break;
        }
    }

    double getNextSimulatedCellValue(ExchangeItem *c,
                                     std::default_random_engine &gen,
                                     expression_t expressiont)
    {
        int sign = ((*c).exch.input ? -1 : 1);

        if ((*c).exch.amountFormula != "")
        {
            return getCellValue((*c).exch, expressiont);
        }
        else
        {
            switch ((*c).exch.uncertaintyType)
            {
            case 1:

                return exp((*c).distribution(gen)) * (*c).exch.conversionFactor * sign;

                break;

            case 2:

                return (*c).distribution(gen) * (*c).exch.conversionFactor * sign;

                break;

            case 3:

                return Triangular::next(getExpressionValue((*c).exch.parameter1Formula, (*c).exch.parameter1, expressiont),
                                        getExpressionValue((*c).exch.parameter2Formula, (*c).exch.parameter2, expressiont),
                                        getExpressionValue((*c).exch.parameter3Formula, (*c).exch.parameter3, expressiont)) *
                       (*c).exch.conversionFactor * sign;

                break;

            case 4:

                return Uniform::next(
                           getExpressionValue((*c).exch.parameter1Formula, (*c).exch.parameter1, expressiont),
                           getExpressionValue((*c).exch.parameter2Formula, (*c).exch.parameter2, expressiont)) *
                       (*c).exch.conversionFactor * sign;

                break;

            default:

                return getCellValue((*c).exch, expressiont);

                break;
            }
        }
    }

    double getImpactCellValue(CalcImpactFactor c)
    {

        return c.amount * c.conversionFactor;
    }

    double getImpactCellValue(CalcImpactFactor c, expression_t expressiont)
    {

        string expr = c.amountFormula;
        double value = getExpressionValue(expr, c.amount, expressiont);
        return value * c.conversionFactor;
    }

    // parameters contains formulaes
    double getParameterCellValue(CalcParameter p, expression_t expressionsTable)
    {

        string expr = p.formula;
        double value = getExpressionValue(expr, p.value, expressionsTable);
        int sign = (p.input ? -1 : 1);

        //the below commented functionality is suspended
        return value /** p.conversionFactor * sign*/;
    }

    double getParameterCellValue(CalcParameter &p)
    {
        //the below commented functionality is suspended
        return p.value /**p.conversionFactor * (p.input ? -1 : 1)*/;
    }

    // parameters co`ntains formulaes
    double getNextSimulatedParameterCellValue(CalcParameter p, std::default_random_engine gen, expression_t expressiont)
    {
        if (p.formula != "")
        {
            return getParameterCellValue(p, expressiont);
        }
        else
        {
            int sign = 1;

            switch (p.uncertaintyType)
            {

            case 1:
                return exp(p.distribution(gen)) /** p.conversionFactor * sign*/;
                break;

            case 2:
                return p.distribution(gen) /** p.conversionFactor * sign*/;
                break;

            case 3:
                return Triangular::next(
                    getExpressionValue(p.parameter1Formula, p.parameter1, expressiont),
                    getExpressionValue(p.parameter2Formula, p.parameter2, expressiont),
                    getExpressionValue(p.parameter3Formula, p.parameter3, expressiont)) /** p.conversionFactor * sign*/;
                break;

            case 4:
                return Uniform::next(getExpressionValue(p.parameter1Formula, p.parameter1, expressiont),
                                     getExpressionValue(p.parameter2Formula, p.parameter2, expressiont)) /** p.conversionFactor * sign*/;
                break;

                // default:
                //     return getParameterCellValue(p, expressiont);

                break;
            }
        }
    }

    double getNextSimulatedParameterCellValue(CalcParameter p, std::default_random_engine gen)
    {
        int sign = 1;

        switch (p.uncertaintyType)
        {

        case 0:
            return getParameterCellValue(p);
            break;

        case 1:

            return exp(p.distribution(gen)) * p.conversionFactor * sign;

            break;

        case 2:

            return p.distribution(gen) * p.conversionFactor * sign;

            break;

        case 3:
            return Triangular::next(p.parameter1, p.parameter2, p.parameter3) * p.conversionFactor * sign;
            break;

        case 4:
            return Uniform::next(p.parameter1, p.parameter2) * p.conversionFactor * sign;
            break;

        default:
            return getParameterCellValue(p);

            break;
        }
    }
};

#endif /* EntitySampler_hpp */
