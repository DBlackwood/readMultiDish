//
//  transform.cpp
//  readMultiDish
//
//  Created by Diane Blackwood on 3/13/18.
//  Copyright © 2018 Diane Blackwood. All rights reserved.
//

#include "transform.hpp"
#include "json.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <vector>
#include <iterator>

#include <fstream>

using namespace std;
using json = nlohmann::json;

vector<string> getOffset(const string& fname){
  vector<string> offSetTuple;
  smatch matcher;
  string line;
  //cout << "fname is " << fname << endl;
  
  ifstream myfile (fname);
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      auto size = line.size();
      //cout <<  "size=" << size << endl << line << "\n";
      if ("#" != line.substr(0, 1) && 5 < size) {
        //cout << "data line~"  << line << endl;
        
        auto rexpr = regex("offsetx=\\s*(\\d+))");
        if (regex_search(line, matcher, rexpr)) {
          offSetTuple.push_back(matcher.str(1));
        }
        rexpr = regex("offsety=\\s*(\\d+))");
        if (regex_search(line, matcher, rexpr)) {
          offSetTuple.push_back(matcher.str(1));
        }
      }  //if not # comment line
    }
    myfile.close();
  };
  return offSetTuple;
}

vector<string> getLines(const string &data){
  //another example
  //https://stackoverflow.com/questions/6126635/break-long-string-into-multiple-c
  vector<string> tokens;
  
  istringstream sTextLine;
  sTextLine.str(data);
  string line;
  while(getline(sTextLine, line)) {
    if(line.empty()) continue;
    if(line[line.size()-1] == '\r') line.resize(line.size()-1);
    if(line.empty()) continue;
    tokens.push_back(line);
  }
  return tokens;
}

string processSubDish(const string findex, json jsubDish) {
  cout << "\nIN processSubDish ---------------------------\n";
  string fn, data, path;
  int col, row;
  StructAvida_cfg avidaCfg;
  
  vector<json> jsubFiles = jsubDish["files"];
  //go thru files
  for (auto it = jsubFiles.begin(); it != jsubFiles.end(); ++it){
    const json& j_file = *it;
    //        if (contains(j_file,"name") && j_file["name"].is_string()){
    if (j_file["name"].is_string() && j_file["data"].is_string() ){
      fn = j_file["name"].get<string>();
      data = j_file["data"].get<string>();
      path =  findex + "/" + fn;
      
      // stuff with files
    }
  }
  return "dummy";
}

bool getFileB(json files, string fname, string& fdata){
  cout << "\nIN getFileB--------------------------------------\n";
  string fn, data, path;
  //go thru files
  for (auto it = files.begin(); it != files.end(); ++it){
    const json& j_file = *it;
    //        if (contains(j_file,"name") && j_file["name"].is_string()){
    if (j_file["name"].is_string() && j_file["data"].is_string() ){
      fn = j_file["name"].get<string>();
      data = j_file["data"].get<string>();
      if (fname == fn) {
        fdata = data;
        cout <<  "file found\n";
        return true;
      }
    }
  }
  cout << "file not found"  << endl;
  return false;
}

StructWorldDim getWorldSize(const string &data) {
  cout << "\nIN getWorldSize ---------------------------\n";
  StructWorldDim worldxy;
  smatch matcher;
  vector<string> words;
  string line;
  int found = 0;
    vector<string> lines = getLines(data);
    unsigned long len = lines.size();
    for (int ii=0; ii<len; ii++) {
      
      line = lines[ii];
      cout << ii << ":" << lines[ii] << endl;
      if ("#" != line.substr(0, 1) && 5 < line.size()) {
        auto rexpr = regex("WORLD_X\\s*(\\d+))");
        if (regex_search(line, matcher, rexpr)) {
          worldxy.xx = atoi(matcher.str(1).c_str());
          found++;
        }
        rexpr = regex("WORLD_Y\\s*(\\d+))");
        if (regex_search(line, matcher, rexpr)) {
          worldxy.yy = atoi(matcher.str(1).c_str());
          found++;
        }
        if (1 < found) return worldxy;
      }  //if not # comment line
    }
  return worldxy;
}

string proccessMultiDish(json mDish) {
  cout << "\nIN proccessMultiDish--------------------------------------\n";
  StructWorldDim superWorldDim;
  string avidaCFG, tmp;

  if (getFileB(mDish["superDishFiles"], "avida.cfg", avidaCFG)) {
    //cout << "B\n" << avidaCFG << endl;
    superWorldDim = getWorldSize(avidaCFG);
    cout << "x=" << superWorldDim.xx << ";  y=" << superWorldDim.yy << endl;

  


  }
  return "dummy";
}


// ---------------------------------------------------------Not in Use---------------------------------------------------------------
// example on spliting a string with a single character
//https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}
