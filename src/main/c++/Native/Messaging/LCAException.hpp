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
#ifndef LCAException_h
#define LCAException_h
#include <stdio.h>
#include <exception>
#include <string>
using namespace std;

class LCAException : public std::exception
{

public:
  string message_;
  int code_;
  
  LCAException(int code, string message)
  {

    message_ = message;
    code_ = code;
  }

  string toString()
  {
    return "{\"code\":\"" + to_string(code_) + "\",\"message\":\"" + message_ + "\",\"data\":\"-1\"}";
  }

  virtual const char *what() const throw()
  {

    string msg = "{\"code\":\"" + to_string(code_) + "\",\"message\":\"" + message_ + "\",\"data\":\"-1\"}";

    return msg.c_str();
  }
};

#endif /* CalcParameter_h */
