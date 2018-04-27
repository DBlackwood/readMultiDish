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

//http://www.kumobius.com/2013/08/c-string-to-int/
bool String2Int(const std::string& str, int& result)
{
    try
    {
        std::size_t lastChar;
        result = std::stoi(str, &lastChar, 10);
        return lastChar == str.size();
    }
    catch (std::invalid_argument&)
    {
        return false;
    }
    catch (std::out_of_range&)
    {
        return false;
    }
}

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

StructXYpair getWorldSize(const string &fileData) {
  cout << "\nIN getWorldSize ---------------------------\n";
  StructXYpair worldxy;
  smatch matcher;
  vector<string> words;
  string line;
  int found = 0;
  vector<string> lines = getLines(fileData);
  unsigned long len = lines.size();
  for (int ii=0; ii<len; ii++) {
    
    line = lines[ii];
    //cout << ii << ":" << lines[ii] << endl;
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

int GetID (const string& aline)
{
  smatch matcher;
  auto rexpr = regex("[\\S]+");  //We want to search for all non-white space
  regex_search(aline, matcher, rexpr);
  cout << matcher[0] << "~ ";
  int ownID = stoi(matcher[0]);
  /*  if we wanted all the words
   vector<string> words;
   string subline = aline;
   while(regex_search(subline, matcher, rexpr))
   {
   words.push_back(matcher[0]);
   subline = matcher.suffix().str();
   }
   */
  return ownID;
}

idmap makeIDdict(string &fileData, unsigned long &startID, string &fileHeader){
  cout << "In MakeIDdict: startID=" << startID << endl;
  idmap IDdict;
  string line;
  unsigned long start = startID;
  vector<int> ownIDv;
  
  // Get a list of IDs in the original file;
  vector<string> lines = getLines(fileData);
  for (auto line : lines) {
    auto size = line.size();
    //cout << line.substr(0, 1) << ": size=" << size << ": " << line << "\n";
    if ("#" != line.substr(0, 1) && 5 < size) {
      int ownID = GetID(line);
      ownIDv.push_back(ownID);
    }
    else if (0 == startID) {
      fileHeader += line + "\n";
      cout << line << endl;
    }
  }//end for:  we now have a list of IDs in the file
  cout << endl;
  
  //Sort the list of IDs
  cout << "\n\nBefore Sort: ";
  for (auto wrd : ownIDv)
  { cout << wrd << " ";}
  
    sort(ownIDv.begin(), ownIDv.end());
    cout << "\n\nSorted: ";
    unsigned long cnt = ownIDv.size();
    startID = start + cnt;                 //update the startID for the next file that is processed.
    unsigned long ii=start;
    for (auto wrd : ownIDv)
    {
      cout << wrd << "; ";
      IDdict[wrd] = ii++;
    }
    cout << endl;

  // just do display the IDdict
  cout << "\n\nPairs: ";
  for (auto wrd : ownIDv) { cout << wrd << ":" << IDdict[wrd] << "; ";}
  cout << endl;
  
  /* another way to build the dictionary
   int ii = 0;
   for (auto word : ownIDv)
   {
   IDdict[word] = ii++;
   }
   */
  
  return IDdict;
}

//-------------------------------------------------------------------------------------- GetNewID --
unsigned long GetNewID(const int& key, const idmap& IDmap)
{
  unsigned long tmpNum;
  auto search = IDmap.find(key);
  if(search != IDmap.end()) {
    tmpNum = search->second;
  }
  else {
    std::cout << "Not found\n";
    tmpNum = 0;
  }
  return tmpNum;
}

//------------------------------------------------------------------------------- getNewPosition --
string getNewPosition(string locations, StructPosSize worldInfo){
  cout << "\nIN getNewPosition:  ";
  string rline = "";           //replacement line
  smatch matcher;
  vector<string> locateList;
  string places = locations;
  auto rexpr = regex("[^,]+");  //We want to search for all non-white space
  while(regex_search(places, matcher, rexpr))
  {
    cout << matcher[0] << "  ";
    locateList.push_back(matcher[0]);
    places = matcher.suffix().str();
  }
  cout << endl;
  return rline;
}

//--------------------------------------------------------------------------- makeReplacementLine --
//  makes a replacement line for details.spop
string makeReplacementLine (const string& aline, const idmap& IDmap, StructPosSize worldInfo)
{
  //cout << "In GetNewLine: ";
  StructIDpair idPair;
  string newline = "";
  smatch matcher;
  vector<string> words;
  string subline = aline;
  auto rexpr = regex("[\\S]+");  //We want to search for all non-white space
  while(regex_search(subline, matcher, rexpr))
  {
    words.push_back(matcher[0]);
    subline = matcher.suffix().str();
  }
  //cout << "There are " << words.size() << " words stored in the vector." << endl;
  int tmpResult = 0;
  unsigned long tmpNum = 0;
  if ( 18 < words.size() )
  {
    if (String2Int(words[0], tmpResult) ){
      idPair.ownID = tmpResult;
      tmpNum = GetNewID(idPair.ownID, IDmap);
      words[0] = to_string(tmpNum);
    }
    else {cout << "intConvert failed: word[0] = " << words[0] << "\n";}
    
    if (String2Int(words[3], tmpResult) ){
      idPair.parentID = tmpResult;
      tmpNum = GetNewID(idPair.parentID, IDmap);
      words[3] = to_string(tmpNum);
    }
    else {
      cout << "intConvert failed: word[3] = " << words[3] << "\n";
    }
    // words[17] contains the position of the organism camma deliminted
    string newLocations = getNewPosition(words[17], worldInfo);

    for (auto it = words.begin(); it != words.end(); ++it)
    {
      newline += *it;            //Iterators are derefenced like pointers with the * operator
      if (it+1 != words.end()){  //Here I'm actually using random access ;-) to peek ahead
        newline += " ";
      } else{
        //cout << newline << endl;
      }
    }
  }  // end of test for organism in the grid that is more than 18 words in string.
  else newline = "dead";
  //cout << newline << "\n";
  return newline;
}

//-------------------------------------------------------------------------------- string MakeNewSpop --
string MakeNewSpop (const idmap& dict, const string& fileData, string fileHeader, StructPosSize worldInfo)
{
  cout << "\nin MakeNewSpop \n";
  string line, newline;
  string newfile = fileHeader;
  vector<string> ownIDv, parentIDv;
  
  vector<string> lines = getLines(fileData);
  for (auto line : lines) {
    auto size = line.size();
    //cout << line.substr(0, 1) << ": size=" << size << ": " << line << "\n";
    if ("#" != line.substr(0, 1) && 5 < size) {
      newline = makeReplacementLine(line, dict, worldInfo);
      if ("dead" != newline)
      {
        newfile += newline + "\n";
      }
    }
  }
  
  return newfile;
}

// Returns a new json structure with the composit files:
// details.spop;
json proccessMultiDish(json mDish) {
  cout << "\nIN proccessMultiDish--------------------------------------\n";
  StructPosSize worldInfo;
  string new_spop_file;
  unsigned long startID= 0;  //the first organims ID in a file.

  json mainFiles = mDish["superDishFiles"];
  worldInfo.main = getWorldSize(mainFiles["avida.cfg"]);

  
  cout << "Main Size: x=" << worldInfo.main.xx << ";  y=" << worldInfo.main.yy << endl;
  
  vector<json> subDishes = mDish["subDishes"];
  
  unsigned long Number_of_subDishes = subDishes.size();
  if (0 < Number_of_subDishes) {
  
  }
  for (unsigned long ii = 0; ii < Number_of_subDishes; ii++) {
    worldInfo.pos.xx = stoi(subDishes[ii]["xpos"].get<string>());
    worldInfo.pos.yy = stoi(subDishes[ii]["ypos"].get<string>());
    cout << "subDish position x=" << worldInfo.pos.xx << "; y="  << worldInfo.pos.yy << endl;
    worldInfo.sub = getWorldSize(mainFiles["avida.cfg"]);
    cout << "subDish    size: x=" << worldInfo.sub.xx << "; y=" << worldInfo.sub.yy << endl;
    //if
    
    // make make part of the new detail.spop file
    string fileData = subDishes[ii]["files"]["detail.spop"];
    string fileHeader = "";
    idmap idDict = makeIDdict(fileData, startID, fileHeader);
    
    cout << "startID = " << startID << ";  fileHeader = \n" << fileHeader << endl;
    
    
    new_spop_file += MakeNewSpop (idDict, fileData, fileHeader, worldInfo);
    cout << "The next startID is " << startID << ". The first part of the new file is below: \n\n";
    
    //cout << new_spop_file;

  }
  return "dummy";
}


// ---------------------------------------------------------Not in Use---------------------------------------------------------------
// example on spliting a string with a single character
//https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
//std::vector<std::string> split(const std::string& s, char delimiter)
//{
//   std::vector<std::string> tokens;
//   std::string token;
//   std::istringstream tokenStream(s);
//   while (std::getline(tokenStream, token, delimiter))
//   {
//      tokens.push_back(token);
//   }
//   return tokens;
//}

