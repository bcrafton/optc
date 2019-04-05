%{

#include "defines.h"
#include "common.h"

#include "fsm.h"
#include "protocol.h"
#include "transition.h"
#include "body.h"
#include "parameter.h"

using namespace std;
using namespace boost;

extern int yylex();
extern void yyerror(char*);
void Div0Error(void);
void UnknownVarError(string s);
extern protocol_t* p;
extern config_t config;
fsm_t* behavior;

%}

%union {

  int     int_val;
  double  double_val;
  string* str_val;

  port_list_t* ports_list;
  port_t* port_val;
  port_type_t port_type_val;
  port_direction_t port_dir_val;

  condition_list_t* condition_list;
  condition_t* cond_val;
  value_t* value_val;

  tree_transition_t*   tree_transition;
  forest_transition_t* forest_transition;

  edge_t* edge_val;
  vector<string>* string_list;

  pair<uint32_t, uint32_t>* mapping_val;
  value_map_t*              map_val;
//  map_list_t*             map_list_val;

  body_t*                   body_val;

  // packets
  packet_t*                                                           type_packet;
  // damn we cud replace the pair<contents, subpackets> with a subpacket.
  // exactly what subpacket contains.
  // pair< vector_t<packet_contents_t*>*, vector_t<subpacket_t*>* >*     type_packet_body;
  // vector_t<subpacket_t*>*                                             type_subpackets;
  subpacket_t*                                                        type_subpacket;
  vector_t<packet_contents_t*>*                                       type_packet_contents;
  packet_prim_type_t                                                  type_packet_prim_type;
  field_mapping_t*                                                    type_field_mapping;
  packet_inst_t*                                                      type_packet_inst;
  vector_t<packet_branch_t*>*                                         type_packet_branches;
  packet_branch_t*                                                    type_packet_branch;

  vector_t<packet_condition_t*>*                                      type_packet_conditions;
  packet_condition_t*                                                 type_packet_condition;  

  // vector_t<binding_t*>*                                               type_bindings;
  // binding_t*                                                          type_binding;
  vector_t<parameter_t*>*                                             type_parameters;
  parameter_t*                                                        type_parameter;

  vector_t<argument_t*>*                                              type_named_args;
  argument_t*                                                         type_named_arg;

  vector_t<string>*                                                   type_arguments;

  vector_t<transaction_t*>*                                           type_transactions;
  transaction_t*                                                      type_transaction;

  // config
  config_t*                                                           type_config;
  pair<string, string>*                                               type_protocol_entry;
  connection_t*                                                       type_connection;
  connections_t*                                                      type_connections;

}

%start top

%token <int_val>    PROTOCOL ENDPROTOCOL
%token <int_val>    PORTS ENDPORTS
%token <int_val>    INIT ENDINIT
%token <int_val>    BEHAVIOR ENDBEHAVIOR
%token <int_val>    GENERATE MAP
%token <int_val>    SEND RECEIVE ATSIGN
%token <int_val>    PACKET SUBPACKET

%token <int_val>    IN OUT 
%token <int_val>    DATA CONTROL STATE
%token <int_val>    BEGIN_BLOCK END_BLOCK


%token <int_val>    RARROW RPAREN LPAREN SEMICOLON COLON PLUS ORBAR COMMA ASSIGN LBRACKET RBRACKET

%token <str_val>    VARIABLE
%token <double_val> NUMBER


%type <ports_list> ports_block;
%type <ports_list> ports;
%type <port_val>  port;
%type <port_type_val> port_type;
%type <port_dir_val>  port_dir;

%type <condition_list> conditions_block
%type <condition_list> conditions;
%type <cond_val> condition;
%type <value_val> value;

%type <sequence_val> behavior_block;
%type <sequence_val> behavior;
%type <sequence_val> sequence;

%type <forest_transition> transitions;

%type <forest_transition> transition_block;
%type <forest_transition> transition_list;
%type <forest_transition>   transition_list_item;
%type <tree_transition>   transition;
%type <tree_transition>   generate_transition;

%type <edge_val>   edge;

%type <type_transactions> receive_block;
%type <type_transactions> send_block;
// replaced by transaction
// %type <string_list> names;

%type <forest_transition> generate;

%type <map_val>         maps_block;
// old
//%type <map_list_val>  maps;

%type <map_val>         map;
%type <map_val>         mappings;
%type <mapping_val>     mapping;

%type <body_val>        body;

// packets
%type <type_packet>          packet_block;
%type <type_packet>          packet;
%type <type_subpacket>       packet_body;

// packet branch
%type <type_packet_branches> packet_branch_block;
%type <type_packet_branches> packet_branches;
// this will need to be a different type because it needs to contain the condition
%type <type_packet_branch>   packet_branch;

%type <type_packet_conditions>   packet_conditions_block;
%type <type_packet_conditions>   packet_conditions;
%type <type_packet_condition>    packet_condition;

// packet contents
%type <type_packet_contents> packet_contents;

// parameter / binding
//%type <type_bindings>        bindings;
//%type <type_binding>         binding;
%type <type_parameters>       parameters;
%type <type_parameter>       parameter;

%type <type_arguments>       arguments_block;
%type <type_arguments>       arguments;

%type <type_named_args>      named_args_block;
%type <type_named_args>      named_args;
%type <type_named_arg>       named_arg;

// transactions
%type <type_transactions>    transactions;
%type <type_transaction>     transaction;

// packet contents items
%type <type_packet_prim_type> packet_prim_type;
%type <type_field_mapping>    field_mapping;
%type <type_packet_inst>      packet_inst;

// config
%type <type_config>         config;
%type <type_protocol_entry> protocol_entry;
%type <type_connections>    connections;
%type <type_connection>     connection;

%%

// protocol / config.
top           : config 
                {
                  config = *( $1 );
                }
              | protocol 
                { 
                } ;

// protocol
protocol      : PROTOCOL VARIABLE protocol_body ENDPROTOCOL 
                { 
                  p->set_name( *($2) );
                  // printf("calling set name: %s\n", p->name.c_str());
                } ;

protocol_body : protocol_body body
                {
                  p->add_body($2);
                }
              | body
                {
                  p->add_body($1);
                } ; 

body          : ports_block     { $$ = new body_t(BODY_TYPE_PORTS,    $1, NULL, NULL, NULL);   }
              | maps_block      { $$ = new body_t(BODY_TYPE_MAP,      NULL, $1, NULL, NULL);   }
              | behavior_block  { 
                                  p->behavior.push_back(behavior);
                                  behavior = new fsm_t();

                                  $$ = new body_t(BODY_TYPE_BEHAVIOR, NULL, NULL, NULL, NULL); 
                                }
              | packet_block    { 
                                  packet_t* packet = $1;
                                  $$ = new body_t(BODY_TYPE_PACKET, NULL, NULL, NULL, packet); 

                                  int i;
                                  for(i=0; i<p->ports->size(); i++)
                                  {
                                    port_t* port = p->ports->at(i);
                                    if ( (port->type == PORT_TYPE_PACKET) && (port->packet_type == packet->type) ) {
                                      behavior = new fsm_t(port, packet);
                                      p->behavior.push_back(behavior);
                                    }
                                  }
                                  behavior = new fsm_t();
                                } ;


// ports
ports_block   : PORTS ports ENDPORTS 
                {
                  p->ports = $2;
                  $$ = $2;
                } ; 

ports         : ports port
                {
                  $1->push_back($2);
                  $$ = $1;
                }
              | port 
                { 
                  port_list_t* ports = new port_list_t();
                  ports->push_back($1);
                  $$ = ports;
                } ; 

port          : port_type port_dir NUMBER VARIABLE 
                { 
                  assert($1 != PORT_TYPE_PACKET);
                  port_t* port = new port_t($1, $2, $3, *($4)); 
                  $$ = port;
                } 
              | port_type port_dir NUMBER VARIABLE VARIABLE
                { 
                  assert($1 == PORT_TYPE_PACKET);
                  port_t* port = new port_t($1, $2, $3, *($4), *($5)); 
                  $$ = port;
                } ;

port_dir      : IN      { $$ = PORT_DIR_IN; } 
              | OUT     { $$ = PORT_DIR_OUT; } ;

port_type     : DATA    { $$ = PORT_TYPE_DATA;    }
              | CONTROL { $$ = PORT_TYPE_CONTROL; }
              | PACKET  { $$ = PORT_TYPE_PACKET;  } ;

conditions_block  : ATSIGN LPAREN conditions RPAREN
                    {  
                      $$ = $3;
                    } ;

conditions        : conditions COMMA condition
                    {  
                      condition_t* new_cond = $3;
                      $1->push_back(new_cond);
                      $$ = $1;
                    }
                  | condition        
                    {  
                      condition_list_t* cond_list = new condition_list_t(); 
                      condition_t* new_cond = $1;
                      cond_list->push_back(new_cond);
                      $$ = cond_list;
                    } ;

condition         : VARIABLE LPAREN value RPAREN 
                    { 
                      string portname = *($1);
                      value_t val = *($3);
        
                      port_t* port = p->ports->find(portname);
                      port_direction_t dir = port->get_dir();
                      uint32_t width = port->width;

                      condition_t* cond = new control_condition_t(portname, dir, width, val);
                      $$ = cond;
                    } ;

value         : NUMBER   
                { 
                  int num = $1;
                  $$ = new value_t(VALUE_TYPE_NUMBER, num); 
                }
              | VARIABLE 
                { 
                  string var = *($1);
                  $$ = new value_t(VALUE_TYPE_VARIABLE, var);  
                } ; 

behavior_block : BEHAVIOR behavior ENDBEHAVIOR 
                 {  
                   // do nothing
                 } ;

behavior       : sequence 
                 {
                 } ;

sequence       : transitions
                 {
                   int i, j;

                   // add ROOT to graph
                   int root = behavior->add_root();
                   for(i=0; i<$1->head.size(); i++) {
                     behavior->add_edge(root, $1->head[i], $1->edge[i]);
                   }

                   // we are going to make all the end vertices sinks
                   for(i=0; i<$1->tail.size(); i++) {
                     behavior->set_sink($1->tail[i]);
                   }
                 } ;

// need to combine transition_block and transition into same type to make this smaller.
transitions      : transitions transition_block
                   {
                     int i, j;
                     for(i=0; i<$1->tail.size(); i++) {
                       for(j=0; j<$2->head.size(); j++) {
                         // big mistake here. edge[i]. shud be the same as head.
                         behavior->add_edge($1->tail[i], $2->head[j], $2->edge[j]); 
                       }
                     }
                     $1->set_tail($2->tail);
                     $$ = $1;
                   } 
                 | transitions transition
                   {
                     int i;
                     for(i=0; i<$1->tail.size(); i++) {
                       behavior->add_edge($1->tail[i], $2->head, $2->edge);
                     }
                     $1->set_tail($2->tail);
                     $$ = $1;
                   } 
                 | transitions generate
                   {
                     int i, j;
                     for(i=0; i<$1->tail.size(); i++) {
                       for(j=0; j<$2->head.size(); j++) {
                         // big mistake here. edge[i]. shud be the same as head.
                         behavior->add_edge($1->tail[i], $2->head[j], $2->edge[j]); 
                       }
                     }
                     $1->set_tail($2->tail);
                     $$ = $1;
                   }
                 | transition_block
                   {
                     $$ = $1;
                   } 
                 | transition
                   {
                     forest_transition_t* t = new forest_transition_t();
                     t->add_head($1->head);
                     t->add_edge($1->edge);
                     t->set_tail($1->tail);
                     $$ = t;
                   }
                 | generate 
                   {
                     $$ = $1;
                   } ;

transition_block : BEGIN_BLOCK transition_list END_BLOCK
                   {
                     $$ = $2;
                   } ;

// this is not gonna work, this could return a connection list
transition_list  : transition_list ORBAR transition_list_item
                   {
                     $1->add_head($3->head);
                     $1->add_edge($3->edge);
                     $1->add_tail($3->tail);
                   } 
                 | ORBAR transition_list_item
                   { 
                     $$ = $2;
                   } ;

transition_list_item : transition_list_item transitions
                       {
                         int i, j;
                         for(i=0; i<$1->tail.size(); i++) {
                           for(j=0; j<$2->head.size(); j++) {
                             behavior->add_edge($1->tail[i], $2->head[j], $2->edge[j]);
                           }
                         }
                         $1->set_tail($2->tail);
                         $$ = $1;
                       }
                     | generate
                       {
                         $$ = $1;
                       }
                     | transition
                       {
                         forest_transition_t* t = new forest_transition_t();
                         t->add_head($1->head);
                         t->add_edge($1->edge);
                         t->set_tail($1->tail);
                         $$ = t;
                       } ;

transition       : PLUS edge
                   {
                     if (behavior == NULL) behavior = new fsm_t();

                     int v = behavior->add_vertex();

                     tree_transition_t* t = new tree_transition_t();
                     t->add_head(v);
                     t->add_edge( $2 );
                     t->add_tail(v);
                     $$ = t;
                   } ;

generate_transition : PLUS edge
                      {
                        if (behavior == NULL) behavior = new fsm_t();

                        // int v = behavior->add_vertex();

                        tree_transition_t* t = new tree_transition_t();
                        // t->add_head(v);
                        t->add_edge( $2 );
                        // t->add_tail(v);
                        $$ = t;
                      } ;

// this is silly and it shud be fixed.
edge             : conditions_block LBRACKET send_block receive_block RBRACKET
                   {
                     condition_list_t conditions = *($1);
                     vector_t<transaction_t*> sends = *($3);
                     vector_t<transaction_t*> receives = *($4);
                     edge_t* edge = new edge_t( conditions, sends, receives );
                     $$ = edge;
                   }
                 | conditions_block LBRACKET receive_block send_block RBRACKET
                   {
                     condition_list_t conditions = *($1);
                     vector_t<transaction_t*> sends = *($4);
                     vector_t<transaction_t*> receives = *($3);
                     edge_t* edge = new edge_t( conditions, sends, receives );
                     $$ = edge;
                   }
                 | conditions_block LBRACKET send_block RBRACKET
                   {
                     condition_list_t conditions = *($1);
                     vector_t<transaction_t*> sends = *($3);
                     vector_t<transaction_t*> receives;
                     edge_t* edge = new edge_t( conditions, sends, receives );
                     $$ = edge;
                   } 
                 | conditions_block LBRACKET receive_block RBRACKET
                   {
                     condition_list_t conditions = *($1);
                     vector_t<transaction_t*> sends;
                     vector_t<transaction_t*> receives = *($3);
                     edge_t* edge = new edge_t( conditions, sends, receives );
                     $$ = edge;
                   } 
                 | conditions_block
                   {
                     condition_list_t conditions = *($1);
                     vector_t<transaction_t*> sends;
                     vector_t<transaction_t*> receives;
                     edge_t* edge = new edge_t( conditions, sends, receives );
                     $$ = edge;
                   } ; 

receive_block    : RECEIVE LPAREN transactions RPAREN
                   {
                     vector_t<transaction_t*>* transactions = $3;
                     int i;
                     for(i=0; i<transactions->size(); i++) {
                       transaction_t* transaction = transactions->at(i);
                       transaction->set_dir(PORT_DIR_IN);
                     }
                     $$ = transactions;
                   } ;

send_block       : SEND LPAREN transactions RPAREN
                   {
                     vector_t<transaction_t*>* transactions = $3;
                     int i;
                     for(i=0; i<transactions->size(); i++) {
                       transaction_t* transaction = transactions->at(i);
                       transaction->set_dir(PORT_DIR_OUT);
                     }
                     $$ = transactions;
                   } ;

/*
names            : names COMMA VARIABLE
                   {
                     $1->push_back( *($3) );
                     $$ = $1;
                   }

                 | VARIABLE
                   {
                     vector<string>* list = new vector<string>();
                     list->push_back( *($1) );
                     $$ = list;
                   } ;
*/

transactions     : transactions COMMA transaction
                   {
                     $1->push_back( $3 );
                     $$ = $1;
                   }
                 | transaction
                   {
                     vector_t<transaction_t*>* transactions = new vector_t<transaction_t*>();
                     transactions->push_back( $1 );
                     $$ = transactions;
                   } ;

transaction      : VARIABLE LPAREN parameters RPAREN
                   {
                     string name = *( $1 );
                     // about width : 
                     // for with I am not sure if this is right thing to do 
                     // if there was a name that was the same on both sides this would break
                     // and we would need to wait to lookup until we had the direction
                     port_t* port = p->ports->find(name);
                     uint32_t width = port->width;
                     // get the params they are specifying 
                     vector_t<parameter_t*> params = *( $3 );
                     // create our transaction
                     transaction_t* transaction = new transaction_t(name, width, params);
                     // return it
                     $$ = transaction;
                   }
                 | VARIABLE
                   {
                     string name = *( $1 );
                     // about width : 
                     // for with I am not sure if this is right thing to do 
                     // if there was a name that was the same on both sides this would break
                     // and we would need to wait to lookup until we had the direction
                     port_t* port = p->ports->find(name);
                     uint32_t width = port->width;
                     // create our transaction
                     transaction_t* transaction = new transaction_t(name, width);
                     // return it
                     $$ = transaction;
                   } ;

// just going to do a single transition for now
// also only doing for a fixed number of transactions
// need to use the map for that later.

// dont need to do the divergence
// can infer from the map
// so smart

/*
for the generate
u are gonna generate N transitions again and attach them all to eachother
there is no 

generate condition begin transation end

generate N name begin transitions end

thats it
*/

generate         : GENERATE VARIABLE ASSIGN NUMBER COLON NUMBER BEGIN_BLOCK generate_transition END_BLOCK
                   {
                     forest_transition_t* top;

                     int i, j;
                     int max = (int) $4;
                     int min = (int) $6;

                     for(i=max; i>=min; i--) {

                       // want to copy the edge out of tree_transition_t
                       edge_t* edge_copy = $8->edge->copy();

                       // change me to max
                       if(i==$4) {
                         int v = behavior->add_vertex();

                         top = new forest_transition_t();
                         top->add_head(v);
                         top->add_edge(edge_copy);
                         top->add_tail(v);
                       }
                       else {
                         int v = behavior->add_vertex();

                         tree_transition_t* next = new tree_transition_t();
                         next->add_head(v);
                         next->add_edge(edge_copy);
                         next->add_tail(v);

                         for(j=0; j<top->tail.size(); j++) {
                           behavior->add_edge(top->tail[j], next->head, next->edge);
                         }

                         top->set_tail(next->tail);
                       }

                     }

                     $$ = top;
                   } 
                 | GENERATE VARIABLE ASSIGN VARIABLE COLON NUMBER BEGIN_BLOCK generate_transition END_BLOCK
                   {
                     // this is in danger of turning into really shitty code
                     forest_transition_t* top;

                     int i, j;

                     string itr_var = *($2);
                     string portname = *($4);
                     port_t* port = p->ports->find(portname);
                     port_direction_t dir = port->get_dir();
                     uint32_t width = port->width;

                     int max = (1 << width) - 1;
                     int min = (int) $6;

                     value_map_t* map_match = NULL;
                     if (p->maps->map_exists(portname)) {
                       map_match = p->maps->get_map(portname);
                       max = map_match->get_max();
                     };

                     for(i=max; i>=min; i--) {

                       // argument namespace
                       // should probably have more generic method for this.
                       // reminds me of compilers class.
                       argument_int_t* arg = new argument_int_t(itr_var, i);
                       vector_t<argument_t*> args;
                       args.push_back(arg);

                       // want to copy the edge out of tree_transition_t
                       edge_t* edge_copy = $8->edge->copy();

                       // change the parameters
                       for(j=0; j<edge_copy->sends.size(); j++) {
                         transaction_t* send = edge_copy->sends.at(j);
                         send->set(args);
                       }
                       for(j=0; j<edge_copy->receives.size(); j++) {
                         transaction_t* rec = edge_copy->receives.at(j);
                         rec->set(args);
                       }

                       if(i==max) {
                         int v = behavior->add_vertex();

                         top = new forest_transition_t();

                         // OFFICIALLY GARBAGE CODE - LOL
                         if( map_match == NULL || (map_match != NULL && map_match->value_exists(i)) ) {
                           uint32_t num;
                           if (map_match != NULL && map_match->value_exists(i)) {
                             num = map_match->get_key(i);
                           }
                           else {
                             num = i;
                           }

                           condition_t* new_cond = new control_condition_t( portname, dir, width, value_t(VALUE_TYPE_NUMBER, num) );
                           edge_copy->conditions.push_back(new_cond);
                         }

                         top->add_head(v);
                         top->add_edge(edge_copy);
                         top->add_tail(v);
                       }
                       else {
                         int v = behavior->add_vertex();

                         tree_transition_t* next = new tree_transition_t();
                         next->add_head(v);
                         next->add_edge(edge_copy);
                         next->add_tail(v);

                         // this really is just 1 for now.
                         for(j=0; j<top->tail.size(); j++) {
                           behavior->add_edge(top->tail[j], next->head, next->edge);
                         }

                         top->set_tail(next->tail);

                         // OFFICIALLY GARBAGE CODE - LOL
                         // this is in danger of turning into really shitty code
                         // fix me: i should not copy and waste the transition
                         // to connect root to this one.
                         if( map_match == NULL || (map_match != NULL && map_match->value_exists(i)) ) {
                           edge_t* edge_copy1 = $8->edge->copy();

                           // change the parameters
                           for(j=0; j<edge_copy1->sends.size(); j++) {
                             transaction_t* send = edge_copy1->sends.at(j);
                             send->set(args);
                           }
                           for(j=0; j<edge_copy1->receives.size(); j++) {
                             transaction_t* rec = edge_copy1->receives.at(j);
                             rec->set(args);
                           }

                           uint32_t num;
                           if (map_match != NULL && map_match->value_exists(i)) {
                             num = map_match->get_key(i);
                           }
                           else {
                             num = i;
                           }

                           condition_t* new_cond = new control_condition_t( portname, dir, width, value_t(VALUE_TYPE_NUMBER, num) );
                           edge_copy1->conditions.push_back(new_cond);

                           top->add_head(v);
                           top->add_edge(edge_copy1);
                         }
                       }
                     }

                     $$ = top;
                   };

maps_block       : map
                   {
                     // p->maps = $1;
                     $$ = $1;
                   } ;

/*
maps             : maps map
                   {
                     // check to make sure that we are not adding duplicate names
                     $1->push_back($2);
                     $$ = $1;
                   }
                 | map
                   {
                     map_list_t* new_map_list = new map_list_t();
                     new_map_list->push_back($1);
                     $$ = new_map_list;
                   } ;
*/
map              : MAP VARIABLE BEGIN_BLOCK mappings END_BLOCK
                   {
                     $4->set_name($2);
                     $$ = $4;
                   } ;

mappings         : mappings mapping
                   {
                     $1->put( *($2) );
                     $$ = $1;
                   }
                 | mapping
                   {
                     value_map_t* new_map = new value_map_t();
                     new_map->put( *($1) );
                     $$ = new_map;
                   } ;

mapping          : NUMBER COLON NUMBER
                   {
                     $$ = new pair<uint32_t, uint32_t>($1, $3);
                   } ;

//// PACKETS ////

packet_block     : packet
                   {
                     $$ = $1;
                   } ;

packet           : PACKET VARIABLE BEGIN_BLOCK packet_body END_BLOCK
                   {
                     subpacket_t* subpacket = $4; 

                     // get the type of our packet
                     string type = *( $2 );

                     // get the contents/header  
                     vector_t<packet_contents_t*> contents = subpacket->packet_contents;

                     // we can use $empty as a param to fix this trash
                     // get the branches
                     vector_t<packet_branch_t*> branches = subpacket->branches;

                     // create the new packet.
                     packet_t* new_packet = new packet_t(type, contents, branches);

                     // return
                     $$ = new_packet;

                   }
                 | PACKET VARIABLE arguments_block BEGIN_BLOCK packet_body END_BLOCK
                   {
                     subpacket_t* subpacket = $5; 

                     // get the type of our packet
                     string type = *( $2 );

                     // get the arguments our packet takes
                     vector_t<string> args = *( $3 );

                     // get the contents/header  
                     vector_t<packet_contents_t*> contents = subpacket->packet_contents;

                     // we can use $empty as a param to fix this trash
                     // get the branches
                     vector_t<packet_branch_t*> branches = subpacket->branches;

                     // create the new packet.
                     packet_t* new_packet = new packet_t(type, args, contents, branches);

                     // return
                     $$ = new_packet;

                   } ;

packet_body      : packet_contents
                   {
                     subpacket_t* subpacket = new subpacket_t();

                     // we are returning a pair of header and branches.
                     vector_t<packet_contents_t*> contents = *( $1 );

                     // set the contents in our pair.
                     subpacket->packet_contents = contents;

                     // return
                     $$ = subpacket;
                   }
                 | packet_body packet_branch_block
                   {
                     subpacket_t* subpacket = $1;

                     // we are returning a pair of header and branches.
                     vector_t<packet_branch_t*> branches = *( $2 );

                     // set the contents in our pair.
                     subpacket->branches = branches;

                     // return
                     $$ = subpacket;
                   } ;

/*
  WE CHANGED PUSH FRONT TO PUSH BACK
  NOT REALLY SURE WHAT ORDER THE BITS COME IN
  BUT IT SHUD BE REVERSED OR SOMETHING IDK
*/

                   // field mapping
packet_contents  : packet_contents field_mapping
                   {
                     //$1->push_front( $2 );
                     $1->push_back( $2 );
                     $$ = $1;
                   }
                   // packet inst
                 | packet_contents packet_inst
                   {
                     //$1->push_front( $2 );
                     $1->push_back( $2 );
                     $$ = $1;
                   }
                   // field mapping
                 | field_mapping
                   {
                     vector_t<packet_contents_t*>* contents = new vector_t<packet_contents_t*>();
                     //contents->push_front( $1 );
                     contents->push_back( $1 );
                     $$ = contents;
                   }
                   // packet inst
                 | packet_inst
                   {
                     vector_t<packet_contents_t*>* contents = new vector_t<packet_contents_t*>();
                     //contents->push_front( $1 );
                     contents->push_back( $1 );
                     $$ = contents;
                   } ;

packet_inst      : SUBPACKET VARIABLE VARIABLE
                   {
                     // get the packet type
                     string type = *( $2 );

                     // get a reference to the packet
                     assert(p->packets.contains(type));
                     packet_t* packet = p->packets.get(type);

                     // get the name
                     string name = *( $3 );

                     // build the packet inst
                     packet_inst_t* packet_inst = new packet_inst_t(name, packet);

                     $$ = packet_inst;
                   }
                 | SUBPACKET VARIABLE VARIABLE named_args_block
                   {
                     // get the packet type
                     string type = *( $2 );

                     // get a reference to the packet
                     assert(p->packets.contains(type));
                     packet_t* packet = p->packets.get(type);

                     // get the name
                     string name = *( $3 );

                     // get the named args list
                     vector_t<argument_t*> args = *($4);

                     // build the packet inst
                     packet_inst_t* packet_inst = new packet_inst_t(name, packet, args);

                     $$ = packet_inst;
                   } ;

packet_prim_type : DATA    { $$ = PACKET_PRIM_DATA; }
                 | CONTROL { $$ = PACKET_PRIM_CONTROL; } ;

field_mapping    : packet_prim_type NUMBER VARIABLE LPAREN parameters RPAREN
                   {
                     packet_prim_type_t type = $1;
                     uint32_t size = $2;
                     string field = *( $3 );
                     vector_t<parameter_t*> params = *( $5 );

                     field_mapping_t* field_mapping = new field_mapping_t( type, field, size, params );
                     $$ = field_mapping;
                   }
                 | packet_prim_type NUMBER VARIABLE 
                   {
                     packet_prim_type_t type = $1;
                     uint32_t size = $2;
                     string field = *( $3 );

                     field_mapping_t* field_mapping = new field_mapping_t( type, field, size );
                     $$ = field_mapping;
                   } ;

parameters       : parameters COMMA parameter
                   {
                     vector_t<parameter_t*>* params;
                     params = $1;

                     parameter_t* param;
                     param = $3;

                     params->push_back( param );
                     $$ = params;
                   }
                 | parameter
                   {
                     vector_t<parameter_t*>* params;
                     params = new vector_t<parameter_t*>();

                     parameter_t* param;
                     param = $1;

                     params->push_back( param );
                     $$ = params;
                   } ;

parameter        : VARIABLE ASSIGN NUMBER
                   {
                     string name = *($1);
                     uint32_t num = $3;

                     parameter_t* param;
                     if      (name == "range")  param = new parameter_range_t(num);
                     else if (name == "offset") param = new parameter_offset_t(num);
                     else if (name == "id")     param = new parameter_id_t(num);
                     else {
                       fprintf(stderr, "Invalid parameter\n");
                       assert(false);
                     }

                     $$ = param;
                   }
                 | VARIABLE ASSIGN VARIABLE
                   {
                     string name = *($1);
                     string var = *($3);

                     parameter_t* param;
                     if      (name == "range")  param = new parameter_range_t(var);
                     else if (name == "offset") param = new parameter_offset_t(var);
                     else if (name == "id")     param = new parameter_id_t(var);
                     else if (name == "dep")    param = new parameter_dep_t(var);
                     else {
                       fprintf(stderr, "Invalid parameter\n");
                       assert(false);
                     }

                     $$ = param;
                   }
                 | VARIABLE ASSIGN VARIABLE LPAREN NUMBER RPAREN
                   {
                     string name = *($1);
                     string pair_name = *($3);
                     uint32_t pair_num = $5;

                     parameter_t* param;
                     if (name == "dep") param = new parameter_dep_t(pair_name, pair_num);
                     else {
                       fprintf(stderr, "Invalid parameter\n");
                       assert(false);
                     }
    
                     $$ = param;
                   }
                 | VARIABLE ASSIGN VARIABLE LPAREN VARIABLE RPAREN
                   {
                     string name = *($1);
                     string pair_name = *($3);
                     string pair_num_var = *($5);

                     parameter_t* param;
                     if (name == "dep") param = new parameter_dep_t(pair_name, pair_num_var);
                     else {
                       fprintf(stderr, "Invalid parameter\n");
                       assert(false);
                     }
    
                     $$ = param;
                   } ;

named_args_block : LPAREN named_args RPAREN
                   {
                     vector_t<argument_t*>* named_args = $2;
                     $$ = named_args;
                   } ;

named_args       : named_args COMMA named_arg
                   {
                     vector_t<argument_t*>* args = $1;
                     argument_t* arg = $3;
                     args->push_back(arg);
                     $$ = args;
                   }
                 | named_arg
                   {
                     vector_t<argument_t*>* args = new vector_t<argument_t*>();
                     argument_t* arg = $1;
                     args->push_back(arg);
                     $$ = args;
                   } ;

named_arg        : VARIABLE ASSIGN NUMBER
                   {
                     string name = *($1);
                     uint32_t num = $3;
                     argument_t* arg = new argument_int_t(name, num);
                     $$ = arg;
                   }
                 | VARIABLE ASSIGN VARIABLE LPAREN NUMBER RPAREN
                   {
                     string name = *($1);
                     string pair_name = *($3);
                     uint32_t pair_val = $5;
                     argument_t* arg = new argument_pair_t(name, pair_name, pair_val);
                     $$ = arg;
                   } ;


arguments_block  : LPAREN arguments RPAREN
                   {
                     vector_t<string>* args = $2;
                     $$ = args;
                   } ;

arguments        : arguments COMMA VARIABLE
                   {
                     string new_arg = *($3);

                     vector_t<string>* args = $1;
                     args->push_back(new_arg);

                     $$ = args;
                   }
                 | VARIABLE 
                   {
                     string new_arg = *($1);

                     vector_t<string>* args = new vector_t<string>();
                     args->push_back(new_arg);

                     $$ = args;
                   } ; 

// packet branches.

packet_branch_block : BEGIN_BLOCK packet_branches END_BLOCK
                    {
                      $$ = $2;
                    } ;

packet_branches  : packet_branches packet_branch
                   {
                     $1->push_back( $2 );
                     $$ = $1;
                   } 
                 | packet_branch
                   {
                     vector_t<packet_branch_t*>* branches = new vector_t<packet_branch_t*>();
                     branches->push_back($1);
                     $$ = branches;
                   } ;

packet_branch    : ORBAR PLUS packet_conditions_block COLON VARIABLE packet_body
                   {
                     vector_t<packet_condition_t*>* packet_conds = $3;
                     // condition->print();
                     // printf("\n");

                     // this will need to be a different type because it needs 
                     // to contain the condition
                     subpacket_t* subpacket = $6;

                     // get the namespace id for this branch
                     string namespace_id = *($5);

                     // create the new packet.
                     packet_branch_t* new_packet_branch = new packet_branch_t(packet_conds, subpacket, namespace_id);

                     // return
                     $$ = new_packet_branch;
                   } ;



packet_conditions_block : ATSIGN LPAREN packet_conditions RPAREN
                          {
                            $$ = $3;
                          } ;

packet_conditions       : packet_conditions COMMA packet_condition
                          {  
                            packet_condition_t* packet_cond = $3;
                            $1->push_back(packet_cond);
                            $$ = $1;
                          }
                        | packet_condition        
                          {  
                            vector_t<packet_condition_t*>* packet_conds = new vector_t<packet_condition_t*>(); 
                            packet_condition_t* packet_cond = $1;
                            packet_conds->push_back(packet_cond);
                            $$ = packet_conds;
                          } ;

packet_condition          : VARIABLE LPAREN value RPAREN 
                          { 
                            string portname = *($1);
                            value_t val = *($3);
                            packet_condition_t* packet_cond = new packet_condition_t(portname, val);
                            $$ = packet_cond;
                          } ;


//////////////
// CONFIG file parsing.
//////////////

config           : protocol_entry protocol_entry connections
                   {
                     // get protocol entry 1 
                     pair<string, string> protocol_entry1 = *( $1 );

                     // get protocol entry 1 
                     pair<string, string> protocol_entry2 = *( $2 );

                     pair<string, string> protocol_file_names = pair<string, string>(protocol_entry1.first, protocol_entry2.first);
                     pair<string, string> protocol_names      = pair<string, string>(protocol_entry1.second, protocol_entry2.second);

                     // get the port map
                     connections_t* connections = $3;

                     // create the config.
                     config_t* config = new config_t(protocol_file_names, protocol_names, connections);

                     $$ = config;
                   } ;

protocol_entry   : VARIABLE VARIABLE
                   {
                     string dir = *( $1 );
                     string name = *( $2 );
                     pair<string, string>* protocol_entry = new pair<string, string>(dir, name);
                     $$ = protocol_entry;
                   } ;

connections      : connections connection
                   {
                     // get the next connection
                     connection_t connection = *( $2 );

                     // get our list thus far
                     connections_t* port_connections = $1;

                     // add connection to list
                     port_connections->push_back( connection );

                     // return our list
                     $$ = port_connections;
                   } 
                 | connection
                   {
                     // get the first connection
                     connection_t connection = *( $1 );

                     // create out list
                     connections_t* port_connections = new connections_t();

                     // add connection to list
                     port_connections->push_back( connection );

                     // return our list
                     $$ = port_connections;
                   } ;

connection       : VARIABLE port_dir VARIABLE VARIABLE port_dir VARIABLE
                   {
                     // get protocol 1 stuff
                     string           protocol_name1 = *( $1 );
                     port_direction_t dir1 = $2;
                     string           port_name1 = *( $3 );

                     // get protocol 2 stuff
                     string           protocol_name2 = *( $4 );
                     port_direction_t dir2 = $5;
                     string           port_name2 = *( $6 );

                     connection_t* connection = new connection_t(protocol_name1, port_name1, dir1, protocol_name2, port_name2, dir2);
                     $$ = connection;
                   } ;

%%





























