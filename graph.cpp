#include <vector>
#include <map>
#include <iostream>

#include "MElementCut.h"

#include "MTetrahedron.h"
#include "MHexahedron.h"
#include "MPrism.h"
#include "MPyramid.h"
#include "MTrihedron.h"

#include "MTriangle.h"
#include "MQuadrangle.h"

#include "MLine.h"

#include "MPoint.h"

#include "graph.h"

void GModelToGraph(GModel* gModel, int* eptr, int** eind, int *metisToGmshIndex)
{
    std::multimap<int, int> elementsNodesMap;

    //Loop over regions
    for(GModel::riter it = gModel->firstRegion(); it != gModel->lastRegion(); ++it)
    {
        GRegion *r = *it;
    
        fillElementsNodesMap(elementsNodesMap, r->tetrahedra.begin(), r->tetrahedra.end());
        fillElementsNodesMap(elementsNodesMap, r->hexahedra.begin(), r->hexahedra.end());
        fillElementsNodesMap(elementsNodesMap, r->prisms.begin(), r->prisms.end());
        fillElementsNodesMap(elementsNodesMap, r->pyramids.begin(), r->pyramids.end());
        fillElementsNodesMap(elementsNodesMap, r->trihedra.begin(), r->trihedra.end());
        fillElementsNodesMap(elementsNodesMap, r->polyhedra.begin(), r->polyhedra.end());
    }

    //Loop over faces
    for(GModel::fiter it = gModel->firstFace(); it != gModel->lastFace(); ++it)
    {
        GFace *f = *it;

        fillElementsNodesMap(elementsNodesMap, f->triangles.begin(), f->triangles.end());
        fillElementsNodesMap(elementsNodesMap, f->quadrangles.begin(), f->quadrangles.end());
        fillElementsNodesMap(elementsNodesMap, f->polygons.begin(), f->polygons.end());
    }

    //Loop over edges
    for(GModel::eiter it = gModel->firstEdge(); it != gModel->lastEdge(); ++it)
    {
        GEdge *e = *it;

        fillElementsNodesMap(elementsNodesMap, e->lines.begin(), e->lines.end());
    }

    //Loop over vertices
    for(GModel::viter it = gModel->firstVertex(); it != gModel->lastVertex(); ++it)
    {
        GVertex *v = *it;

        fillElementsNodesMap(elementsNodesMap, v->points.begin(), v->points.end());
    }

    //create mesh format for METIS
    int position = 0;
    eptr[0] = 0;
    int i = 0;
    int itFirstLast = 0;
    
    for(std::multimap<int, int>::iterator it = elementsNodesMap.begin(); it != elementsNodesMap.end(); ++it)
    {
        if(itFirstLast != it->first+1)
        {
            metisToGmshIndex[i] = it->first + 1;
            position += elementsNodesMap.count(it->first);
            eptr[i+1] = position;
            i++;
            itFirstLast = it->first+1;
        }
    }

    int *eindTemp = new int[position];
    int positionInd = 0;
    for(std::multimap<int, int>::iterator it = elementsNodesMap.begin(); it != elementsNodesMap.end(); ++it)
    {
        eindTemp[positionInd] = it->second;
        positionInd++;
    }
    *eind = eindTemp;
}

template <class ITERATOR>
void fillElementsNodesMap(std::multimap<int, int> &elementsNodesMap, ITERATOR it_beg, ITERATOR it_end)
{
    for(ITERATOR it = it_beg; it != it_end; ++it)
    {
        const int tag = (*it)->getNum()-1;
        
        for(unsigned int j = 0; j < (*it)->getNumVertices(); j++)
        {
            elementsNodesMap.insert(std::pair<int, int>(tag,(*it)->getVertex(j)->getNum()-1));
        }
    }
}
