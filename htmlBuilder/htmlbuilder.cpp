#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class Builder
{

  ofstream* _out;
  ifstream* _in;
  string _str;
public:
    stringstream ans;
  Builder(string str);
  void build();
};

Builder::Builder(string str): _str(str)
{
    string test="../trie/archive/"+str+".txt";
  _in = new ifstream("../trie/archive/"+str+".txt");
  if(!_in->is_open())
  {
      std::cout << "file not opened" << "\n";
       std::cout << "cant open "<<test  << "\n";
      ans << "ERROR" << endl;
    return;
  }
  std::cout << " opened "<<test  << "\n";
  _in = new ifstream("../trie/archive/"+str+".txt");
}

void Builder::build()
{
  _out = new ofstream("../htmlBuilder/html/ret.html");
  ifstream stream("../htmlBuilder/html/start.txt");
  stream.seekg(0, std::ios::end);
  size_t size = stream.tellg();
  string buffer(size, ' ');
  stream.seekg(0);
  stream.read(&buffer[0], size);

  (*_out) << buffer << endl;
  ans<<buffer << endl;
  (*_out) << _str << endl;
  ans<< _str << endl;
  ifstream stream2("../htmlBuilder/html/cont1.txt");

  stream2.seekg(0, std::ios::end);
  size = stream2.tellg();
  stream2.seekg(0);
  string buffer2(size, ' ');
  stream2.read(&buffer2[0], size);

  (*_out) << buffer2 << endl;
  ans << buffer2 << endl;
  string astart2="<a href =\"";
  string astart = "<a href =\"http://";
  string acont = "\">";
  string aend = "</a>";

  ifstream name(_str);
  (*_out) << "<h1>Showing results for \"" << _str << "\":<br><br><br></h1>" << endl;
  ans << "<h1>Showing results for \"" << _str << "\":<br><br><br></h1>" << endl;
  string line;
  int i = 1;
  while(getline(*_in,line))
  {
    if(line.substr(0,7).compare("http://")==0)
    {
        line=line.substr(7,line.length()-1);
    }
    (*_out) << astart << line << acont << "Result number " << i
    << "  is:                        " << line << "<br><br>" << aend << endl;
    ans << astart << line << acont << "Result number " << i++
    << "  is:                        " << line << "<br><br>" << aend << endl;
  }

  (*_out) << "</body> </html>" << endl;
   ans << "</body> </html>" << endl;
  delete _out;
  delete _in;
}
