
#include "defines.h"

using namespace std;
using namespace boost;

extern FILE* yyin; 
extern int yyparse();

int main(int num_args, char** args) {

  FILE* f;

  f = fopen("src.optimo", "r");
  if(f == NULL) {
    printf("couldn't open %s\n", "src.optimo");
    exit(1);
  }
  else {
    printf("parsing: %s\n", "src.optimo");
  }
  yyin = f;
  yyparse();
  fclose(f);

}




