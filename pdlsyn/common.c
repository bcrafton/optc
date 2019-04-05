
#include "common.h"

using namespace std;
using namespace boost;

int log2number(int num) {
    return (int)(ceil(log2(num)));
}

std::string num2binary(int count, int width) {
  int n = count;
  std::string output;
  for (int i = 0; i < width; i++) {
  if (n) {
     int bit = n%2;
     output += boost::lexical_cast<std::string>(bit);
  } else {
     output += "0";
  }
  n = n/2;
  }
  std::reverse(output.begin(), output.end());
  return output;
}

vector_t<string> split(const string& str, const string& delim)
{
  vector_t<string> tokens;
  size_t prev = 0, pos = 0;
  do
  {
    pos = str.find(delim, prev);
    if (pos == string::npos) pos = str.length();
    string token = str.substr(prev, pos-prev);
    if (!token.empty()) tokens.push_back(token);
    prev = pos + delim.length();
  }
  while (pos < str.length() && prev < str.length());
  return tokens;
}
