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

#ifndef ContributionTreeNode_hpp
#define ContributionTreeNode_hpp

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

struct ContributionTreeNode {
    long NodeId;
    long ParentNodeId;
    long ParentProcessId;
    long ProcessId;
    double Share;

    ContributionTreeNode(
            long _NodeId,
            long _ParentNodeId,
            long _ParentProcessId,
            long _ProcessId,
            double _Share) {

        NodeId = _NodeId;
        ParentNodeId = _ParentNodeId;
        ParentProcessId = _ParentProcessId;
        ProcessId = _ProcessId;
        Share = _Share;
    }


};



#endif /* ContributionTreeNode_hpp */
