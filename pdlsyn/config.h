
#ifndef CONFIG_H
#define CONFIG_H

#include "defines.h"
#include "common.h"

#include "connection.h"

/**
 * config is the abstract class representation of the config file that we parse
 */

class config_t {
  public:
  std::pair<std::string, std::string> protocol_file_names;
  std::pair<std::string, std::string> protocol_names;
  connections_t* connections;

  config_t();
  config_t(std::pair<std::string, std::string> protocol_file_names, 
           std::pair<std::string, std::string> protocol_names, 
           connections_t* connections);

  void print();
  std::string to_string();
};

#endif
