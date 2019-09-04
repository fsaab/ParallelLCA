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
#ifndef StringUtils_h
#define StringUtils_h

#include <string>
#include <vector>
#include <iostream>


#include <boost/algorithm/string.hpp>

#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_operator.hpp>


using namespace std;
class StringUtils{
 
public:
    
    static void join(const vector<string>& v, string str, string& s) {
        
        s.clear();
        
        for (vector<string>::const_iterator p = v.begin();
             p != v.end(); ++p) {
            s += *p;
            if (p != v.end() - 1)
                s += str;
        }
    }

    /*Simple Is-Numeric function in C++*/
    // static bool is_digit_decimal(const char value) {
    //      return std::isdigit(value)||
    // value=='.'||
    // value=='e'||
    // value=='-'||
    // value=='+'; }
    // static bool is_number(const std::string &value) { return std::all_of(value.begin(), value.end(), is_digit_decimal); }

    static bool is_number(std::string const& str)
    {
        std::string::const_iterator first(str.begin()), last(str.end());
        return boost::spirit::qi::parse(first, last,
                boost::spirit::double_ >> *boost::spirit::qi::space)
            && first == last;
    }

    
};


#endif /* StringUtils_h */
