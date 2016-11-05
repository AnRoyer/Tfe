#ifndef GRAPH_INCLUDED
#define GRAPH_INCLUDED

#include "Gmsh.h"
#include "GModel.h"

void GModelToGraph(GModel* gModel, int* eptr, int** eind, int *metisToGmshIndex);

#endif //GRAPH_INCLUDED
