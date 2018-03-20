//
//  transform.hpp
//  readMultiDish
//
//  Created by Diane Blackwood on 3/13/18.
//  Copyright © 2018 Diane Blackwood. All rights reserved.
//

#ifndef transform_hpp
#define transform_hpp

#include <stdio.h>
#include <regex>
#include <stdio.h>
#include <string>
#include <map>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

struct StructAvida_cfg
{
  int xx;
  int yy;
};

struct StructWorldDim
{
  int xx;
  int yy;
};

using idmap = map<int, unsigned long>;

vector<string> getOffset(const string& content);
StructAvida_cfg getAvidaConfig(const string &data);
string proccessMultiDish(json mDish);

#endif /* transform_hpp */
