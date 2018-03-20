//
//  main.cpp
//  readMultiDish
//
//  Created by Diane Blackwood on 3/13/18.
//  Copyright © 2018 Diane Blackwood. All rights reserved.
//
// use the the website http://jsonviewer.stack.hu/ to look at the output from .dump() of a json object
//
// information about json.hpp is at the websites below:
// https://github.com/nlohmann/json#design-goals
// https://nlohmann.github.io/json/
//


#include <iostream>
#include "buildMultiDish.hpp"
#include "transform.hpp"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

int main(int argc, const char * argv[]) {

  json multiDish;
  
  multiDish = makeJson("m0");
  cout << "---------------------------------------------------------------\n";
  cout << multiDish.dump();
  cout << "\n---------------------------------------------------------------\n";
  
  string out = proccessMultiDish(multiDish);


//Example way to split a string with a character
//https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c/14266139
//std::string s = "scott\ntiger\nmushroom";
//std::string delimiter = "\n";
//
//size_t pos = 0;
//std::string token;
//while ((pos = s.find(delimiter)) != std::string::npos) {
//    token = s.substr(0, pos);
//    std::cout << token << std::endl;
//    s.erase(0, pos + delimiter.length());
//}
//std::cout << s << std::endl;

  return 0;
}

