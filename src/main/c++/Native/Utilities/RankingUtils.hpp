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
#ifndef RankingUtils_hpp
#define RankingUtils_hpp

#include <iostream>
using namespace std;

#include <gsl/gsl_vector.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_sort_vector.h>
#include <vector>
#include <cmath>

class RankingUtils {
public:


    // Function returns the rank vector
    // of the set of observations

    static double * rankify(double* X, int N) {




        // Rank Vector
        double* Rank_X = new double[N];

        for (int i = 0; i < N; i++) {
            int r = 1, s = 1;

            // Count no of smaller elements
            // in 0 to i-1
            for (int j = 0; j < i; j++) {
                if (X[j] < X[i]) r++;
                if (X[j] == X[i]) s++;
            }

            // Count no of smaller elements
            // in i+1 to N-1
            for (int j = i + 1; j < N; j++) {
                if (X[j] < X[i]) r++;
                if (X[j] == X[i]) s++;
            }

            // Use Fractional Rank formula
            // fractional_rank = r + (n-1)/2
            Rank_X[i] = r + (s - 1) * 0.5;
        }

        // Return Rank Vector
        return Rank_X;
    }


    //https://coral.ise.lehigh.edu/jild13/2016/07/11/hello/

    static double spearman_corr(double* in, double* out, int n) {



        int stridea = 1, strideb = 1;


        double * groupa = new double[stridea * n];
        double * groupb = new double[stridea * n ];



        //    double * groupa = (double *) malloc (stridea * na * sizeof(double));
        // double * groupb = (double *) malloc (strideb * nb * sizeof(double));


        for (int i = 0; i < n; i++)
            groupa[i * stridea] = (double) in[i];
        for (int i = 0; i < n; i++)
            groupb[i * strideb] = (double) out[i];


        // check with current test for Pearson coef
        // double r = gsl_stats_correlation(groupa, stridea, groupb, strideb, nb);


        //     // get answer from R:
        //     // groupa <- c(.0421, .0941, .1064, .0242, .1331, .0773, .0243, .0815, .1186, .0356, .0728, .0999, .0614, .0479)
        //     // groupb <- c(.1081, .0986, .1566, .1961, .1125, .1942, .1079, .1021, .1583, .1673, .1675, .1856, .1688, .1512)
        //     // rs <- cor(groupa, groupb, method="spearman")
        //     // format(rs, digits=18)  #-0.160439560439560425

        double * w = new double[2 * n];

        double rs = gsl_stats_spearman(groupa, stridea, groupb, strideb, n, w);

        //gsl_stats_spearman_free (w);

        delete w;

        delete groupa;
        delete groupb;



        return rs;

    }

    /*
spearman_resolve_ties()
  Resolve a sequence of ties.

The input ranks array is expected to take the same value for all indices in
tiesTrace. The common value is recoded with the average of the indices. For
example, if ranks = <5,8,2,6,2,7,1,2> and tiesTrace = <2,4,7>, the result
will be <5,8,3,6,3,7,1,3>.

Input: nties - number of ties
       ranks - vector of ranks
       ties_trace - vector with the indices of the ties

Return: none

Note: this routine is adapted from The Apache Commons Mathematics Library
     */
    static inline void
    spearman_resolve_ties(const size_t nties, gsl_vector *ranks, size_t *ties_trace) {
        size_t i;

        // constant value of ranks over ties_trace
        double c = gsl_vector_get(ranks, ties_trace[0]);

        // new rank (ie. the average of the current indices)
        double avg = (2 * c + nties - 1) / 2;

        for (i = 0; i < nties; ++i)
            gsl_vector_set(ranks, ties_trace[i], avg);
    } /* spearman_resolve_ties() */

    /*
spearman_rank()
 Rank data using the natural ordering on doubles, ties being resolved by 
taking their average.

Input: data - vector of observations
       ranks - vector of ranks
       d - vector of sorted observations
       p - vector of indices of observations as if they were sorted

Return: none

Note: this routine is adapted from The Apache Commons Mathematics Library
     */
    static inline int
    spearman_rank(const gsl_vector *data,
            gsl_vector *ranks,
            gsl_vector *d,
            gsl_permutation *p) {
        size_t i;

        // copy the input data and sort them
        gsl_vector_memcpy(d, data);
        gsl_sort_vector(d);

        // get the index of the input data as if they were sorted
        gsl_sort_vector_index(p, data);

        // walk the sorted array, filling output array using sorted positions,
        // resolving ties as we go
        double pos = 1;
        gsl_vector_set(ranks, gsl_permutation_get(p, 0), pos);
        size_t nties = 1;

        size_t * ties_trace = (size_t*) calloc(1, sizeof (size_t));
        if (ties_trace == 0) {
            GSL_ERROR_NULL("failed to allocate space for ties_trace vector",
                    GSL_ENOMEM);
        }
        ties_trace[0] = gsl_permutation_get(p, 0);

        for (i = 1; i < data->size; ++i) {
            if (gsl_vector_get(d, i) - gsl_vector_get(d, i - 1) > 0) {
                pos = i + 1;
                if (nties > 1)
                    spearman_resolve_ties(nties, ranks, ties_trace);
                ties_trace = (size_t*) realloc(ties_trace, sizeof (size_t));
                if (ties_trace == 0) {
                    GSL_ERROR_NULL("failed to allocate space for ties_trace vector",
                            GSL_ENOMEM);
                }
                nties = 1;
                ties_trace[0] = gsl_permutation_get(p, i);
            } else {
                ++nties;
                ties_trace = (size_t*) realloc(ties_trace, nties * sizeof (size_t));
                if (ties_trace == 0) {
                    GSL_ERROR_NULL("failed to allocate space for ties_trace vector",
                            GSL_ENOMEM);
                }
                ties_trace[nties - 1] = gsl_permutation_get(p, i);
            }
            gsl_vector_set(ranks, gsl_permutation_get(p, i), pos);
        }
        if (nties > 1)
            spearman_resolve_ties(nties, ranks, ties_trace);

        free(ties_trace);

        return GSL_SUCCESS;
    } /* spearman_rank() */

static void spearmanrank(double * data, int n,double*  ranks_) {



        gsl_vector * v = gsl_vector_alloc(n);

        for (int i = 0; i < n; i++) {
            gsl_vector_set(v, i, data[i]);
        }

        gsl_vector *ranks=gsl_vector_alloc(n);
        gsl_vector *d=gsl_vector_alloc(n);
        gsl_permutation *p=gsl_permutation_alloc(n);

        spearman_rank(v, ranks, d, p);

        for (int i = 0; i < n; i++) /* OUT OF RANGE ERROR */ {
            ranks_[i] = gsl_vector_get(ranks, i);
        }

         gsl_vector_free (v);
         gsl_vector_free (ranks);
         gsl_vector_free (d);
         gsl_permutation_free (p);
       
    }

static double spearman_corr_formula(double* in, double in_avg, double* out, double out_avg, int n) {

        double sum = 0, sum_sq_in = 0,sum_sq_out=0;
        
       

        for (int i = 0; i < n; i++) {
            
            double in_diff=0,out_diff=0;
            in_diff=in[i] - in_avg;
            out_diff=out[i] - out_avg;

            sum += (in_diff) * (out_diff);
            sum_sq_in += pow(in_diff,2);
            sum_sq_out += pow(out_diff,2);
        }

       

        return sum / sqrt(sum_sq_in*sum_sq_out);

    }


};
#include <stdio.h>

#endif /* RankingUtils_hpp */
