#ifndef GRAPH_INCLUDED
#define GRAPH_INCLUDED

#include "Gmsh.h"
#include "GModel.h"

void GModelToGraph(GModel* gModel, int* eptr, int** eind, int *metisToGmshIndex);
template <class ITERATOR>
void fillElementsNodesMap(std::multimap<int, int> &elementsNodesMap, ITERATOR it_beg, ITERATOR it_end);

#endif //GRAPH_INCLUDED
