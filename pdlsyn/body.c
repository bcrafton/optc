
#include "body.h"

using namespace std;
using namespace boost;

// body

// cud use a void pointer or inheritance, but fuck it
body_t::body_t(body_type_t type, port_list_t* ports, value_map_t* map, fsm_t* behavior, packet_t* packet)
{

  // printf("%d %d\n", type, BODY_TYPE_PORTS);
  this->type = type;

  this->ports = NULL;
  this->map = NULL;
  this->behavior = NULL;

  switch (type) {

    case BODY_TYPE_PORTS:
      this->ports = ports;
      break;

    case BODY_TYPE_MAP:
      this->map = map;
      break;

    case BODY_TYPE_BEHAVIOR:
      this->behavior = behavior;
      break;

    case BODY_TYPE_PACKET:
      this->packet = packet;
      break;

    default:
      fprintf(stderr, "Invalid body type: %d\n", type);
      assert(0);
  }
  
}
