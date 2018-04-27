//
//  buildMultiDish.cpp
//  readMultiDish
//
//  Created by Diane Blackwood on 3/13/18.
//  Copyright © 2018 Diane Blackwood. All rights reserved.
//

#include "buildMultiDish.hpp"
#include "json.hpp"
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <iterator>
#include <fstream>

//https://stackoverflow.com/questions/306533/how-do-i-get-a-list-of-files-in-a-directory-in-c
#include <sys/stat.h>
#include <dirent.h>

using json = nlohmann::json;
using namespace std;

//https://stackoverflow.com/questions/306533/how-do-i-get-a-list-of-files-in-a-directory-in-c
/* Returns a list of files in a directory (except the ones that begin with a dot) */
StructDirListing GetFilesInDirectory(const string &directory)
{
  StructDirListing out;
  #ifdef WINDOWS
    HANDLE dir;
    WIN32_FIND_DATA file_data;

    if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
        return; /* No files found */

    do {
        const string file_name = file_data.cFileName;
        const string full_file_name = directory + "/" + file_name;
        const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        if (file_name[0] == '.')
            continue;

        //if (is_directory) continue;

        out.name.push_back(file_name);
        out.isdir.push_back(is_directory);
    } while (FindNextFile(dir, &file_data));

    FindClose(dir);
  #else
    DIR *dir;
    class dirent *ent;
    class stat st;
    dir = opendir(directory.c_str());
    //dir = opendir(directory);
    while ((ent = readdir(dir)) != NULL) {
        const string file_name = ent->d_name;
        //const string full_file_name = directory + "/" + file_name;
        const string full_file_name = directory +  file_name;

        if (file_name[0] == '.')
            continue;

        if (stat(full_file_name.c_str(), &st) == -1)
            continue;

        const bool is_directory = (st.st_mode & S_IFDIR) != 0;

        //if (is_directory) continue;
        out.name.push_back(file_name);
        out.isdir.push_back(is_directory);

    }
    closedir(dir);
  #endif
  return out;
} // GetFilesInDirectory

bool clearDirStuct(StructDirListing dirName){
  dirName.name.clear();
  dirName.isdir.clear();
  return true;
}

json makeJson(const string& dir) {
  string filePath, fileName, line;
  unsigned long len, lngth;
  StructDirListing mainDir, subDir;
  json subj, mainj, superDishFiles, subDishFiles;
  vector<json> subDishes;
  vector<string> offsetPair;
  string fname, fdata;
  stringstream buffer;

  if (true) {    //should be test to see that it is a file. Skipping I need this working. then put in Avida
    //filePathName = dir + "/entryname";
    filePath = dir + "/";
    mainDir = GetFilesInDirectory(filePath);
    //cout << "main; filePath = " << filePath << endl;
    mainj["amend"] = "false";
    mainj["name"] = "importMultiDish";
    mainj["type"] = "addEvent";

    len = mainDir.name.size();
    for (int ii=0; ii<len; ii++) {
      cout << "Main: " << ii << "  " << mainDir.isdir[ii] << "   " << mainDir.name[ii]  << "\n";    //debug
      if (mainDir.isdir[ii]) {
        //process subfoler
        filePath = dir + "/" + mainDir.name[ii] + "/";
        cout << "Main isdir=1; filePath = " << filePath << endl;
        clearDirStuct(subDir);
        subDishFiles.clear();
        subj.clear();
        subDir = GetFilesInDirectory(filePath);
        lngth = subDir.name.size();
        cout << "subDir length is " << lngth << endl;
        for (int jj=0; jj<lngth; jj++) {
          cout << "Sub:  " << jj << "  " << subDir.isdir[jj] << "   " << subDir.name[jj]  << "\n";    //debug
          if (!subDir.isdir[jj]) {
            fileName = filePath + subDir.name[jj];
            if ("offset.txt" == subDir.name[jj]) {
              //cout << "Process offset.txt file\n";
              offsetPair = getOffset(fileName);
              cout << "Offset x=" << offsetPair[0] << "; y=" << offsetPair[1];
            }
            else {
              //https://stackoverflow.com/questions/2912520/read-file-contents-into-a-string-in-c
              //https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
              ifstream ifs(fileName);
              string content( (istreambuf_iterator<char>(ifs) ),
                             (istreambuf_iterator<char>()    ) );
              
              //jfile = { {"name",subDir.name[jj]}, {"data", content} };
              subDishFiles[subDir.name[jj]] = content;
            }
          }
          else cout << "isdir = true";
        }  //end subDish;
        
        subj["xpos"] = offsetPair[0];
        subj["ypos"] = offsetPair[1];
        subj["files"] = subDishFiles;
        subDishes.push_back(subj);
      }
      else     //its a superdish file
      {
        fileName = dir + "/" + mainDir.name[ii];
        cout << "file"  << ii << " is " << fileName << endl;
        
        ifstream ifs(fileName);
        string content( (istreambuf_iterator<char>(ifs) ),
                        (istreambuf_iterator<char>()    ) );
        //cout << "content is\n" << content << "\n";
        superDishFiles[mainDir.name[ii]] = content;
      }
    }  // end multidish
    mainj["superDishFiles"] = superDishFiles;
    mainj["subDishes"] = subDishes;
//    cout << "\n---------------------------------------------------------------\n";
//    cout << mainj.dump();
//    cout << "\n---------------------------------------------------------------\n";

  }
  
 return mainj;
}

//---------------------------- stuff looked up, but not using ------------------------------------
        //https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
//        ifstream myfile(fileName.c_str());
//        buffer << myfile.rdbuf();
//        fdata = buffer.str();

