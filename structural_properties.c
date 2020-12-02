#include "cgraph_constants.h"
#include "cgraph_datatype.h"
#include "cgraph_error.h"
#include "cgraph_interface.h"
#include "cgraph_queue.h"
#include "cgraph_topology.h"
#include "cgraph_vector.h"

int cgraph_is_dag(const cgraph_t *graph, bool *res) {
  *res = true;

  if(!cgraph_is_directed(graph)) {
    *res = false;
    return 0;
  }

  CGRAPH_INTEGER no_of_vertices = cgraph_vcount(graph);

  for(int i = 0; i < no_of_vertices; i++) {
    /* use bfs to verify if there is a path from one vertex to itself */
    cgraph_iqueue_t source = cgraph_iqueue_create();
    CGRAPH_INTEGER passed[no_of_vertices];
    CGRAPH_INTEGER pop;
    
    memset(passed, 0, no_of_vertices * sizeof(CGRAPH_INTEGER));

    cgraph_iqueue_enqueue(source, i);
    passed[i] = 1;

    while(!cgraph_iqueue_empty(source)) {
      cgraph_iqueue_poll(source, &pop);
      for(int j = graph->os[pop]; j < graph->os[pop + 1]; j++) {
        int node = graph->to[ graph->oi[j] ];

        if(!passed[node]) {
          passed[node] = 1;
          cgraph_iqueue_enqueue(source, node);
        }
        else if(passed[node] && node == i) {
          *res = false;
          cgraph_iqueue_free(&source);
          return 0;
        }

      }
    }

    cgraph_iqueue_free(&source);
  }
  return 0;
}