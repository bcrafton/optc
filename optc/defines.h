
#ifndef DEFINES_H
#define DEFINES_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <stdlib.h>
#include <vector>
#include <set>
#include <string>
#include <stdio.h>
#include <utility>
#include <math.h>
#include <tuple>
#include <sys/time.h>
#include <time.h>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/graphviz.hpp>
#include <pthread.h>

#define NUM_THREADS 4

#define uint64_t unsigned long
#define uint32_t unsigned int
#define uint8_t unsigned char

#define RANGE_SCALE 8
#define OFFSET_SCALE 1

// if there are too many differences from ubuntu to gatech ece redhat, will use this
#define GATECH_ECE_SERVER 0
#define DYNAMIC_ALGORITHM 1
#define PRINT_VERTEX      1
#define PRINT_EDGE        1
#define VIZ               1
#define MERGE             1
#define VERILOG           1
#define TEST_BENCH        1

#endif
