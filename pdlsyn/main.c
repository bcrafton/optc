
#include "fsm.h"
#include "defines.h"
#include "protocol.h"
#include "config.h"
#include "btimer.h"

using namespace std;
using namespace boost;

extern FILE* yyin; // this is already in the parser and we just need to point it to a file.
extern int yyparse();

protocol_t* p; // our top level protocol
protocol_t* p1;
protocol_t* p2;

// do we inher config lhs = p1 and rhs = p2?
//vector_t< pair<string, string> > connections;
config_t config;
protocol_t* merged_p;

// from these two things we get:
// port_connections_t

int main(int num_args, char** args) {

  FILE* f;

  // get config
  f = fopen("config", "r");
  if(f == NULL) {
    printf("couldn't open %s\n", "config");
    exit(1);
  }
  else {
    printf("parsing: %s\n", "config");
  }
  yyin = f;  // now flex reads from file
  yyparse();
  fclose(f);

  // config.print();

  // get protocol 1
  p = new protocol_t();
  p1 = p;
  f = fopen(config.protocol_file_names.first.c_str(), "r");
  if(f == NULL) {
    printf("couldn't open %s\n", config.protocol_file_names.first.c_str());
    exit(1);
  }
  else {
    printf("parsing: %s\n", config.protocol_file_names.first.c_str());
  }
  yyin = f;  // now flex reads from file
  yyparse();
  fclose(f);

  // get protocol 2
  p = new protocol_t();
  p2 = p;
  f = fopen(config.protocol_file_names.second.c_str(), "r");
  if(f == NULL) {
    printf("couldn't open %s\n", config.protocol_file_names.second.c_str());
    exit(1);
  }
  else {
    printf("parsing: %s\n", config.protocol_file_names.second.c_str());
  }
  yyin = f;  // now flex reads from file
  yyparse();
  fclose(f);

  if (VIZ) {
    string p1_name = "./out/" + p1->name + ".dot";
    p1->to_visualize(p1_name);

    string p2_name = "./out/" + p2->name + ".dot";
    p2->to_visualize(p2_name);
  }


  if (MERGE) {

    btimer_t timer;
    timer.start();

    merged_p = new protocol_t(p1, p2, &config);

    timer.end();
    printf("Time Taken | CPU: %f WALL: %f\n", timer.cpu_time(), timer.wall_time());

    // print # of states.
    printf("# states : %u\n", merged_p->size());
  }

  if (VIZ) {
    string merged_p_name = "./out/" + merged_p->protocol_name() + ".dot";
    merged_p->to_visualize(merged_p_name);
  }

  if (VERILOG) {
    std::ofstream myfile;
    string filename;

    filename = "./out/" + merged_p->protocol_name() + ".v";
    myfile.open (filename);
    myfile << merged_p->to_verilog();
    myfile.close();
  }

  if (TEST_BENCH) {
    std::ofstream myfile;
    string filename;

    filename = "./out/" + merged_p->protocol_name() + "_tb.v";
    myfile.open (filename);
    myfile << merged_p->test_bench();
    myfile.close();

    filename = "./out/input.vec";
    myfile.open (filename);
    myfile << merged_p->test_bench_vec();
    myfile.close();

    filename = "./out/makefile";
    myfile.open (filename);
    myfile << merged_p->makefile();
    myfile.close();
  }


}




