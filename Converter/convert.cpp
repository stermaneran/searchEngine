//This program takes spider files and puts it in the archive
//"WORDS.RECORD.URL: "

#include <iostream>
#include <fstream>

#include <string>
#include <sstream>
#include "../trie/trie.cpp"

#pragma once

#define PREF "WORDS.RECORD.URL: "

extern pthread_mutex_t _rawPagesOfsMutex;

using namespace std;

class Convert {
  ifstream file;
  string pref = PREF;
  string _fname;
  size_t pref_len = pref.length();
  Trie t;

public:
  Convert(string fname);
  ~Convert();
  void write();
};

Convert::Convert(string fname)
{
  _fname=fname;
  file.open(_fname);

}

Convert::~Convert()
{
  file.close();
}


void Convert::write()
{
  if(!file.is_open())
  {
    file.open(_fname);
  }

  string line;
  string url;

  getline(file,line);
  if((line.substr(0,pref_len)).compare(pref) == 0) {
    url = line.substr(pref_len,line.length()-1);
    //cout << "\nThe obtained url is: " << url << "\n" << endl;
  }
  else
  {
    cout << PREF << " not found!" << endl;
    file.close();
    return;
  }
  while(line != "")
  {
    getline(file,line);
    if((line.substr(0,pref_len)).compare(pref) == 0) {
      url = line.substr(pref_len,line.length()-1);
      //cout << "\nThe obtained url is: " << url << "\n" << endl;
      continue;
    }
    else if(line == "") break;
    std::locale loc;
    stringstream lower;
    for (std::string::size_type i=0; i<line.length(); ++i)
      lower << std::tolower(line[i],loc);
    //std::cout << lower.str() << "\n";
    t.go(WRITE,lower.str(),url);
    //cout << "adding " << url << " --> " << line << ".txt" << endl;
    }
    file.close();
}
