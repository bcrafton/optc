
#include "connection.h"

using namespace std;
using namespace boost;

// connection

/*
class connection_t {
  public:
  port_ref_t send_ref;
  vector_t<std::string> send_heir;

  port_ref_t rec_ref;
  vector_t<std::string> rec_heir;

  // i like it best this way.
  // idc about 6 params.
  connection_t(std::string prot_name1, std::string name1, port_direction dir1, std::string prot_name2, std::string name2, port_direction dir2);

  void print();
  std::string to_string();
};
*/

connection_t::connection_t(string prot_name1, string name1, port_direction dir1, string prot_name2, string name2, port_direction dir2) 
{
  bool flag1 = (dir1 == PORT_DIR_IN && dir2 == PORT_DIR_OUT);
  bool flag2 = (dir1 == PORT_DIR_OUT && dir2 == PORT_DIR_IN);
  assert(flag1 || flag2);

  if (dir1 == PORT_DIR_OUT) {
    this->send_ref = var_ref_t(prot_name1, name1, dir1);
    this->rec_ref = var_ref_t(prot_name2, name2, dir2);
  }
  else {
    this->send_ref = var_ref_t(prot_name2, name2, dir2);
    this->rec_ref = var_ref_t(prot_name1, name1, dir1);
  }
}

string connection_t::to_string()
{
  string connection_string;
  
  connection_string += this->send_ref.to_string();
  connection_string += this->rec_ref.to_string();

  return connection_string;
}

void connection_t::print()
{
  printf("%s", this->to_string().c_str());
}

// connections

/*
class connections_t : public vector_t<connection_t> {
  public:
  connections_t();

  // class functions
  bool contains(transaction_t t1, transaction_t t2);
  bool contains(port_ref_t* ref1, port_ref_t* ref2);

  vector_t <connection_t> connections(transaction_t t1);
  bool buffer(transaction_t t1, transaction_t t2);

  void print();
  std::string to_string();
};
*/

connections_t::connections_t()
{
}

/*
bool connections_t::contains(transaction_t t1, transaction_t t2)
{
  assert(0);
}
*/

bool connections_t::contains(port_ref_t* ref1, port_ref_t* ref2)
{
  int i;
  for(i=0; i<this->size(); i++) {
    // get this itr's connection
    connection_t connection = this->at(i);

    // if in/out
    if (ref1->get_dir() == PORT_DIR_OUT) {
      // this asssertion is not true at all
      // they shud just return false if the case.
      // we are gonna have ins/outs on both sides.
      // assert(ref2->get_dir() == PORT_DIR_IN);

      if (ref2->get_dir() == PORT_DIR_OUT) {
        return false;
      } 

      bool flag1 = ref1->compare(&connection.send_ref) == 0;
      bool flag2 = ref2->compare(&connection.rec_ref) == 0;
      if (flag1 && flag2) return true;
    }

    // if out/in
    else {
      // if its not out then it has to be in
      assert(ref1->get_dir() == PORT_DIR_IN);

      // these assertions are not true at all
      // they shud just return false if the case.
      // we are gonna have ins/outs on both sides.
      // assert(ref2->get_dir() == PORT_DIR_OUT);

      if (ref2->get_dir() == PORT_DIR_IN) {
        return false;
      } 

      bool flag1 = ref1->compare(&connection.rec_ref) == 0;
      bool flag2 = ref2->compare(&connection.send_ref) == 0;
      if (flag1 && flag2) return true;
    }
  }

  // if never found, then return false
  return false;
}

vector_t<var_ref_t> connections_t::connections(var_ref_t* ref)
{
  // we had this before:   vector_t<port_ref_t*> ret;
  // and for whatever reason it would not work.
  vector_t<var_ref_t> ret;

  int i;
  for(i=0; i<this->size(); i++) {
    connection_t connection = this->at(i);
    if (connection.send_ref.compare(ref) == 0) {
      ret.push_back( connection.rec_ref );
      //connection.send_ref.print();
      //connection.rec_ref.print();
      //printf("\n");
    }
    else if (connection.rec_ref.compare(ref) == 0) {
      ret.push_back( connection.send_ref );
      //connection.rec_ref.print();
      //connection.send_ref.print();
      //printf("\n");
    }
  }

  return ret;
}

bool connections_t::buffer(var_ref_t* t1, var_ref_t* t2)
{
  assert(false);
  // return this->contains(t1, t2);
}

void connections_t::print()
{
  printf("%s", this->to_string().c_str());
}

string connections_t::to_string()
{
  string connections_string;

  int i;
  for(i=0; i<this->size(); i++) {
    connections_string += this->at(i).to_string();
    connections_string += "\n";
  }  

  return connections_string;
}
