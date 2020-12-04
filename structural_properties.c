#include "cgraph_constants.h"
#include "cgraph_datatype.h"
#include "cgraph_error.h"
#include "cgraph_interface.h"
#include "cgraph_iqueue.h"
#include "cgraph_topology.h"
#include "cgraph_ivec.h"

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

int cgraph_topological_sorting(const cgraph_t *graph,
                               cgraph_ivec_t *res,
                               cgraph_neimode_t mode)
{
  cgraph_ivec_init(res, 0);
  cgraph_neimode_t mode2;
  if (mode == CGRAPH_OUT)
    mode2 = CGRAPH_IN;
  if (mode == CGRAPH_IN)
    mode2 = CGRAPH_OUT;
  cgraph_ivec_t dgree = cgraph_ivec_create();
  cgraph_degree_all(graph, &dgree, mode2, true);
  CGRAPH_INTEGER ressize = 0;
  bool check = true;
  while (check)
  {
    CGRAPH_INTEGER indx = 0;
    cgraph_ivec_t nei = cgraph_ivec_create();
    while (dgree[indx] != 0 && indx < graph->n)
      indx++;
    if (indx == graph->n)
      check = false;
    else
    {
      dgree[indx]--;
      cgraph_neighbors(graph, &nei, indx, mode);
      for (CGRAPH_INTEGER j = 0; j < cgraph_ivec_size(nei); j++)
        dgree[nei[j]]--;
      cgraph_ivec_push_back(res, indx);
    }
  }
  return 0;
}

/**
 * \function cgraph_topological_sorting
 * \brief Calculate a possible topological sorting of the graph.
 *
 * </para><para>
 * A topological sorting of a directed acyclic graph is a linear ordering
 * of its nodes where each node comes before all nodes to which it has
 * edges. Every DAG has at least one topological sort, and may have many.
 * This function returns a possible topological sort among them. If the
 * graph is not acyclic (it has at least one cycle), a partial topological
 * sort is returned and a warning is issued.
 *
 * \param graph The input graph.
 * \param res Pointer to a vector, the result will be stored here.
 *   It will be resized if needed.
 * \param mode Specifies how to use the direction of the edges.
 *   For \c IGRAPH_OUT, the sorting order ensures that each node comes
 *   before all nodes to which it has edges, so nodes with no incoming
 *   edges go first. For \c IGRAPH_IN, it is quite the opposite: each
 *   node comes before all nodes from which it receives edges. Nodes
 *   with no outgoing edges go first.
 * \return Error code.
 *
 * Time complexity: O(|V|+|E|), where |V| and |E| are the number of
 * vertices and edges in the original input graph.
 *
 * \sa \ref igraph_is_dag() if you are only interested in whether a given
 *     graph is a DAG or not, or \ref igraph_feedback_arc_set() to find a
 *     set of edges whose removal makes the graph a DAG.
 *
 * \example examples/simple/igraph_topological_sorting.c
 */
int cgraph_topological_sorting(const cgraph_t *graph,
                               cgraph_ivec_t *res,
                               cgraph_neimode_t mode) {
  /* TODO: Provide an implementation and pass tests */
  return 0;
}