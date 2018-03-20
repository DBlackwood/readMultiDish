//
//  buildMultiDish.hpp
//  readMultiDish
//
//  Created by Diane Blackwood on 3/13/18.
//  Copyright © 2018 Diane Blackwood. All rights reserved.
//

#ifndef tools_hpp
#define tools_hpp

#include <stdio.h>
#include <regex>
#include <string>
#include <map>
#include "json.hpp"
#include "transform.hpp"

using namespace std;
using json = nlohmann::json;

struct StructDirListing
{
  vector<string> name;
  vector<bool> isdir;
};

bool clearStuct(StructDirListing dirName);
json makeJson(const string& dir);
StructDirListing GetFilesInDirectory(const string &directory);



#endif /* buildMultiDish_hpp */
