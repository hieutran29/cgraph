#include <stdio.h>

#include "gb.h"
#include "ut.h"

int main() {
  cgraph_t g;
  test_create_g1(&g);
  cgraph_ivec_t vertices = cgraph_ivec_create();
  cgraph_ivec_t edges = cgraph_ivec_create();
  double weight[] = {5, 10, 3, 4, 2, 7, 20, 5, 8};
  cgraph_get_shortest_path_dijkstra(&g, &vertices, &edges, 0, 5, weight, CGRAPH_OUT);
  for(int i = 0; i < cgraph_ivec_size(vertices); i++) {
      printf("%d ", vertices[i]);
  }
  printf("\n");
  for(int i = 0; i < cgraph_ivec_size(edges); i++) {
      printf("%d ", edges[i]);
  }
  return 0;
}