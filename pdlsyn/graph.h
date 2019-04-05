
#ifndef GRAPH_H
#define GRAPH_H

#include "defines.h"
#include "common.h"

class vertex_t;
class edge_t;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, vertex_t*, edge_t*, boost::no_property> graph_t;

typedef graph_t::vertex_descriptor vid_t;
typedef graph_t::edge_descriptor eid_t;

#endif
