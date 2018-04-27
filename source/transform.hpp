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

struct StructWorldTriplet
{
  int xx;
  int yy;
  int nn;
};

struct StructWorldData
{
  StructWorldTriplet main;
  StructWorldTriplet pos;  //for position or offset
  StructWorldTriplet sub;
};

struct StructIDpair
{
  int ownID;
  int parentID;
};
using idmap = map<int, unsigned long>;

using idmap = map<int, unsigned long>;

vector<string> getOffset(const string& content);
json proccessMultiDish(json mDish, vector<string> &names);
idmap makeIDdict(const string &fileData, unsigned long &startID, string &fileHeader);

#endif /* transform_hpp */
