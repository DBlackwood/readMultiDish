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

StructWorldTriplet getWorldSize(const string &fileData) {
  cout << "\nIN getWorldSize ---------------------------\n";
  StructWorldTriplet worldxy;
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
      if (1 < found) {
        worldxy.nn = worldxy.xx * worldxy.yy;
        return worldxy;
      }
    }  //if not # comment line
  }
  return worldxy;
}

int GetID (const string& aline)
{
  smatch matcher;
  auto rexpr = regex("[\\S]+");  //We want to search for all non-white space
  regex_search(aline, matcher, rexpr);
  //cout << matcher[0] << "~ ";
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
      //cout << line << endl;
    }
  }//end for:  we now have a list of IDs in the file
  //cout << endl;
  
  //cout << "\n\nBefore Sort: ";
  //for (auto wrd : ownIDv) { cout << wrd << " ";}
  
  //Sort the list of IDs
  sort(ownIDv.begin(), ownIDv.end());

  unsigned long cnt = ownIDv.size();
  startID = start + cnt;                 //update the startID for the next file that is processed.
  unsigned long ii=start;
  for (auto wrd : ownIDv)  {IDdict[wrd] = ii++;}
  
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

//------------------------------------------------------------------------------- getShiftedIndex --
int getShiftedIndex(string numstr, StructWorldData worldInfo){
  int pren, prex, prey, postn, postx, posty;

    cout << "numstr=" << numstr;
    if ( String2Int(numstr, pren) ) {
      prex = pren % worldInfo.sub.xx;
      prey = pren / worldInfo.sub.xx;
      postx = prex + worldInfo.pos.xx;
      posty = prey + worldInfo.pos.yy;
      postn = posty * worldInfo.main.xx + postx;
      cout << "_" << pren << "-->" << postn << ";  ";
      //need some error checking
      //cout << "; postn=" << postn << "; world.main.nn=" << worldInfo.main.nn << " ";
      if (postn < 0 || worldInfo.main.nn <= postn) postn = -1;
    }
    else {
      postn = -1;
    }
    return postn;
}

//------------------------------------------------------------------------------- getNewPosition --
string getNewPosition(string places, StructWorldData worldInfo){
  //cout << "\nIN getNewPosition: ";
  string rline = "";           //replacement line
  smatch matcher;
  vector<int> linePos;
  int postn;
  string subplaces = places;
  //cout << "subplaces=" << subplaces << "; ";
  auto rexpr = regex("[^,]+");  //We want to search for all non-white space
  while(regex_search(subplaces, matcher, rexpr))
  {
    string numstr = matcher[0];
    postn = getShiftedIndex(numstr, worldInfo);
    if (0 <= postn) {
      linePos.push_back(postn);
    }
    else {
      cout << "getShiftedID failed: pre_index=" << postn << ";\n";
      return "ERROR";
    }
    subplaces = matcher.suffix().str();
  }
  //cout << endl;
  //for loop to create rline;
  // auto actually resolves to the type std::vector<string>::iterator, which is a lot to type
   for (auto it = linePos.begin(); it != linePos.end(); ++it)
   {
      rline += to_string(*it); //Iterators are derefenced like pointers with the * operator
      if (it+1 != linePos.end()){  // Here I'm actually using random access ;-) to peek ahead
         rline += ",";             // Only put in a comma if there is another number
      }
   }
  //cout << "rline=" << rline << ";\n";
  return rline;
}

//--------------------------------------------------------------------------- makeReplacementLine --
//  makes a replacement line for details.spop
string makeReplacementLine (const string& aline, const idmap& IDmap, StructWorldData worldInfo)
{
  //cout << "In makeReplacementLine: ";
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
    //cout << "words17=" << words[17] << "; ";
    string newLocations = getNewPosition(words[17], worldInfo);
    words[17] = newLocations;

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

//---------------------------------------------------------------------------- string MakeNewSpop --
string MakeNewSpop(const idmap& dict, const string& fileData, StructWorldData worldInfo)
{
  cout << "\n\n\nin MakeNewSpop \n";
  string line, newline;
  string newfile = "";
  vector<string> ownIDv, parentIDv;
  
  vector<string> lines = getLines(fileData);
  for (auto line : lines) {
    auto size = line.size();
    //cout << line.substr(0, 1) << ": size=" << size << ": " << line << "\n";
    if ("#" != line.substr(0, 1) && 5 < size) {
      newline = makeReplacementLine(line, dict, worldInfo);
      //cout << "old line = \n"<< line << "\nnew line is \n" << newline << endl;
      if ("dead" != newline)
      {
        newfile += newline + "\n";
      }
    }
  }
  return newfile;
}

string findCladeName(string name, int number, vector<string> &names){
  int num = number + 1;
  string newName;
  string aName = name + "-" + to_string(num);
  if (find(names.begin(), names.end(), aName) != names.end()) {
    newName = findCladeName(aName, num, names);
  }
  else { newName = aName; }
  return newName;
}

string getCladeName(string aname, vector<string> &names){
  string theName;
  if (find(names.begin(), names.end(), aname) != names.end()){
    theName = findCladeName(aname, 1, names);
  }
  else theName = aname;
  names.push_back(theName);
  return theName;
}

string makeCladeLine(const string& aline, StructWorldData worldInfo, vector<string> &names) {
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
  if (1 < words.size() ) {
    string aname = getCladeName(words[0], names);
    string newLocations = getNewPosition(words[1], worldInfo);
    words[1] = newLocations;
  }
  newline = words[0] + " " + words[1];
  
  return newline;
}

string MakeNewClade(const string& fileData, StructWorldData worldInfo, string &cladeHeader, vector<string> &names) {
  cout << "nin MakeNewClade \n";
  string line="", newline="";
  string newfile = "";
  bool makeHeaderFlag = true;
  if (1 < cladeHeader.size()) makeHeaderFlag = false;
  
  vector<string> lines = getLines(fileData);
  for (auto line : lines) {
    auto size = line.size();
    cout << "size=" << size << "; line is\n" << line << endl;
    //cout << line.substr(0, 1) << ": size=" << size << ": " << line << "\n";
    if ("#" == line.substr(0, 1)) {
      //make header
      if (makeHeaderFlag) {
        cladeHeader += line + "\n";
      }
    }
    else if (2 < size){
      //make new line
      newline = makeCladeLine(line, worldInfo, names);
      cout << "oldline|" << line << "\n" << "newline=" << newline << endl;
      newfile += newline + "\n";
    }
  }
  cout << "The newfile is \n" << newfile;
  return newfile;
}

// Returns a new json structure with the composit files:
// details.spop;
json proccessMultiDish(json mDish, vector<string> &names){
  cout << "\n\n\n------------In proccessMultiDish---------------------------------------------------------------\n\n" << flush;
  StructWorldData worldInfo;
  string new_spop_file="", new_clade_file="";
  string spop_header = "", cladeHeader="";
  json nWorld, nfile;
  vector<json> vfiles;
  unsigned long startID= 0;  //the first organims ID in a file.
  json mainFiles = mDish["superDishFiles"];

  worldInfo.main = getWorldSize(mainFiles["avida.cfg"]);
  cout << "Main Size: x=" << worldInfo.main.xx << ";  y=" << worldInfo.main.yy << ";  n=" << worldInfo.main.nn << endl;

  vector<json> subDishes = mDish["subDishes"];
  
  unsigned long Number_of_subDishes = subDishes.size();
  if (0 < Number_of_subDishes) {
  
  }
  for (unsigned long ii = 0; ii < Number_of_subDishes; ii++) {
    worldInfo.pos.xx = stoi(subDishes[ii]["xpos"].get<string>());
    worldInfo.pos.yy = stoi(subDishes[ii]["ypos"].get<string>());
    cout << "subDish position x=" << worldInfo.pos.xx << "; y="  << worldInfo.pos.yy << endl;
    worldInfo.sub = getWorldSize(subDishes[ii]["files"]["avida.cfg"]);
    cout << "subDish    size: x=" << worldInfo.sub.xx << "; y=" << worldInfo.sub.yy << endl;
    //if
    
    // create  new detail.spop file
    string sPopData = subDishes[ii]["files"]["detail.spop"];
    idmap idDict = makeIDdict(sPopData, startID, spop_header);
    cout << "\n\nbefore call MakeNewSpop\n\n";
    new_spop_file = new_spop_file + MakeNewSpop(idDict, sPopData, worldInfo);
    cout << "\n\n\nstartID=" << startID << endl; //"; new_spop_file=\n" << new_spop_file << endl;
    
    // create new clade.ssg file
    string cladeData = subDishes[ii]["files"]["clade.ssg"];
    new_clade_file = new_clade_file + MakeNewClade(cladeData, worldInfo, cladeHeader, names);
    //cout << "\nClade Header is \n" << cladeHeader << endl;
    cout << "\nnew_clade_file is \n" << new_clade_file << endl;

  }
  new_spop_file = spop_header + new_spop_file;
  nfile["name"] = "detail.spop";
  nfile["data"] = new_spop_file;
  vfiles.push_back(nfile);
  cout << "\n\nNew detail.spop file:\n" << new_spop_file << endl;
  new_clade_file = cladeHeader + new_clade_file;
  nfile["name"] = "clade.ssg";
  nfile["data"] = new_clade_file;
  vfiles.push_back(nfile);
  cout << "\n\nNew clade.ssg file:\n" << new_clade_file << endl;

  
  // ------- build new world (populated dish) json object
  nWorld["amend"] = "false";
  nWorld["name"] = "importExpr";
  nWorld["type"] = "addEvent";
  vector<string> fnames = {"avida.cfg"
    , "clade.ssg"
    , "environment.cfg"
    , "events.cfg"
    , "instset.cfg"};
  for (int ii = 0; ii < fnames.size(); ii++){
    string fname = fnames[ii];
    if (mainFiles.find(fname) != mainFiles.end() ) {
      nfile["name"] = fname;
      nfile["data"] = mainFiles[fname];
      vfiles.push_back(nfile);
    }
  }
  nWorld["files"] = vfiles;

  
  return nWorld;
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

