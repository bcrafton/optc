

#include "config.h"

using namespace std;
using namespace boost;

// config
/*
class config_t {
  public:
  pair<string, string> protocol_file_names;
  pair<string, string> protocol_names;
  connections_t* connections;

  config_t();
  config_t(pair<string, string> protocol_file_names, 
           pair<string, string> protocol_names, 
           vector_t< connection_t > connections);

  void print();
  string to_string();
};
*/

config_t::config_t::config_t()
{
}

config_t::config_t(pair<string, string> protocol_file_names, pair<string, string> protocol_names, connections_t* connections)
{
  this->protocol_file_names = protocol_file_names;
  this->protocol_names = protocol_names;
  this->connections = connections;
}

void config_t::print()
{
  printf("%s", this->to_string().c_str());
}

string config_t::to_string()
{
  string config_string;

  config_string += this->protocol_file_names.first;
  config_string += " ";
  config_string += this->protocol_file_names.second;

  config_string += "\n";

  return config_string;
}



