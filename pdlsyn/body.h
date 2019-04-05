
#ifndef BODY_H
#define BODY_H

#include "defines.h"
#include "common.h"

#include "port.h"
#include "value.h"
#include "fsm.h"
#include "packet.h"

/**
 * Body is a class used in our parser to allow us to parse PDL syntax in any particular order. 
 */

typedef enum body_type {
  BODY_TYPE_PORTS,
  BODY_TYPE_MAP,
  BODY_TYPE_BEHAVIOR,
  BODY_TYPE_PACKET
} body_type_t;

class body_t {
  public:
  body_type_t type;

  port_list_t* ports;
  value_map_t* map;
  fsm_t* behavior;
  packet_t* packet;

  // cud use a void pointer or inheritance, but fuck it
  body_t(body_type_t type, port_list_t* ports, value_map_t* map, fsm_t* behavior, packet_t* packet);
};

#endif
