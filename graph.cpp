#include <map>

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
    std::multimap<int, int> nodesToElements;

    //Loop over regions
    for(GModel::riter it = gModel->firstRegion(); it != gModel->lastRegion(); ++it)
    {
        GRegion *r = *it;
    
        fillNodesToElements(nodesToElements, r->tetrahedra.begin(), r->tetrahedra.end());
        fillNodesToElements(nodesToElements, r->hexahedra.begin(), r->hexahedra.end());
        fillNodesToElements(nodesToElements, r->prisms.begin(), r->prisms.end());
        fillNodesToElements(nodesToElements, r->pyramids.begin(), r->pyramids.end());
        fillNodesToElements(nodesToElements, r->trihedra.begin(), r->trihedra.end());
        fillNodesToElements(nodesToElements, r->polyhedra.begin(), r->polyhedra.end());
    }

    //Loop over faces
    for(GModel::fiter it = gModel->firstFace(); it != gModel->lastFace(); ++it)
    {
        GFace *f = *it;

        fillNodesToElements(nodesToElements, f->triangles.begin(), f->triangles.end());
        fillNodesToElements(nodesToElements, f->quadrangles.begin(), f->quadrangles.end());
        fillNodesToElements(nodesToElements, f->polygons.begin(), f->polygons.end());
    }

    //Loop over edges
    for(GModel::eiter it = gModel->firstEdge(); it != gModel->lastEdge(); ++it)
    {
        GEdge *e = *it;

        fillNodesToElements(nodesToElements, e->lines.begin(), e->lines.end());
    }

    //Loop over vertices
    for(GModel::viter it = gModel->firstVertex(); it != gModel->lastVertex(); ++it)
    {
        GVertex *v = *it;

        fillNodesToElements(nodesToElements, v->points.begin(), v->points.end());
    }

    //create mesh format for METIS
    unsigned int position = 0;
    eptr[0] = 0;
    unsigned int i = 0;
    int itFirstLast = 0;
    
    for(std::multimap<int, int>::const_iterator it = nodesToElements.begin(); it != nodesToElements.end(); ++it)
    {
        if(itFirstLast != it->first+1)
        {
            metisToGmshIndex[i] = it->first + 1;
            position += nodesToElements.count(it->first);
            eptr[i+1] = position;
            i++;
            itFirstLast = it->first+1;
        }
    }

    int *eindTemp = new int[position];
    unsigned int positionInd = 0;
    for(std::multimap<int, int>::const_iterator it = nodesToElements.begin(); it != nodesToElements.end(); ++it)
    {
        eindTemp[positionInd] = it->second;
        positionInd++;
    }
    *eind = eindTemp;
}

template <class ITERATOR>
void fillNodesToElements(std::multimap<int, int> &nodesToElements, ITERATOR it_beg, ITERATOR it_end)
{
    for(ITERATOR it = it_beg; it != it_end; ++it)
    {
        const int tag = (*it)->getNum()-1;
        
        for(unsigned int j = 0; j < (*it)->getNumVertices(); j++)
        {
            nodesToElements.insert(std::pair<int, int>(tag,(*it)->getVertex(j)->getNum()-1));
        }
    }
}
