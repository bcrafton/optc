
#include "vec_file.h"

int compare (gconstpointer a, gconstpointer b)
{
  GString* str1 = (GString*)a;
  GString* str2 = (GString*)b;
  return strcmp(str1->str, str2->str);
}

gboolean traverse(void* key, void* value, void* data)
{
  GString* key1 = (GString*) key;
  printf("%s\n", key1->str);
  return FALSE;
}

GTree* load_table() {

  FILE* f;
  char buffer1[100];
  char buffer2[100];
  uint64_t val;

  GArray* port_names = g_array_sized_new(FALSE, FALSE, sizeof(GString*), 50);
  GTree* ports = g_tree_new( &compare );

  // open the vec file
  f = fopen("input.vec", "rb");
  if (f==NULL) {
    fprintf(stderr, "File not found\n");
  }

  // confirm first strings are "vec begin"
  fscanf(f, "%s %s", buffer1, buffer2);
  assert( strcmp(buffer1, "vec") == 0 );
  assert( strcmp(buffer2, "begin") == 0 );

  // while we do not get end, fill vector.
  do {
    fscanf(f, "%s", buffer1);

    if ( strcmp(buffer1, "end") != 0 ) {
      GString* str = g_string_new_len(buffer1, sizeof(char) * 50);
      port_names = g_array_append_val( port_names, str );
    }

  } while( strcmp(buffer1, "end") != 0 );

  int index = 0;
  while(fscanf(f, "%lx", &val) != EOF)
  {
    // not sure how this array thing works.
    GString* key = ( (GString*) g_array_index(port_names, GString*, index) );

    if ( g_tree_lookup(ports, key) != NULL ) {
      GArray* vals = g_tree_lookup(ports, key);
      vals = g_array_append_val(vals, val);
      g_tree_insert(ports, key, vals);
    }
    else {
      GArray* vals = g_array_sized_new(FALSE, FALSE, sizeof(uint64_t), 50);
      vals = g_array_append_val(vals, val);
      g_tree_insert(ports, key, vals);
    }

    if (index+1 == port_names->len) {
      index = 0;
    }
    else {
      index++;
    }
  }

  printf("number elements in tree: %d\n", g_tree_nnodes(ports));
  g_tree_foreach(ports, &traverse, NULL);
  return ports;
}



