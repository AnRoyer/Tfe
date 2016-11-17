#include <vector>
#include <map>
#include <iostream>

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
  std::map<int, std::vector<int> > elementsNodesMap;

  //Loop over regions
  for(GModel::riter it = gModel->firstRegion(); it != gModel->lastRegion(); ++it)
  {
    GRegion *r = *it;
    
    //Tetrahedra
    for(unsigned int i = 0; i < r->tetrahedra.size(); i++)
    {
      const int tag = r->tetrahedra[i]->getNum()-1;

      for(unsigned int j = 0; j < r->tetrahedra[i]->getNumVertices(); j++)
      {
	elementsNodesMap[tag].push_back(r->tetrahedra[i]->getVertex(j)->getNum()-1);
      }
    }

    //Hexahedra
    for(unsigned int i = 0; i < r->hexahedra.size(); i++)
    {
      const int tag = r->hexahedra[i]->getNum()-1;

      for(unsigned int j = 0; j < r->hexahedra[i]->getNumVertices(); j++)
      {
	elementsNodesMap[tag].push_back(r->hexahedra[i]->getVertex(j)->getNum()-1);
      }
    }

    //Prisms
    for(unsigned int i = 0; i < r->prisms.size(); i++)
    {
      const int tag = r->prisms[i]->getNum()-1;

      for(unsigned int j = 0; j < r->prisms[i]->getNumVertices(); j++)
      {
	elementsNodesMap[tag].push_back(r->prisms[i]->getVertex(j)->getNum()-1);
      }
    }

    //Pyramids
    for(unsigned int i = 0; i < r->pyramids.size(); i++)
    {
      const int tag = r->pyramids[i]->getNum()-1;

      for(unsigned int j = 0; j < r->pyramids[i]->getNumVertices(); j++)
      {
	elementsNodesMap[tag].push_back(r->pyramids[i]->getVertex(j)->getNum()-1);
      }
    }

    //Trihedra
    for(unsigned int i = 0; i < r->trihedra.size(); i++)
    {
      const int tag = r->trihedra[i]->getNum()-1;

      for(unsigned int j = 0; j < r->trihedra[i]->getNumVertices(); j++)
      {
	elementsNodesMap[tag].push_back(r->trihedra[i]->getVertex(j)->getNum()-1);
      }
    }
    
    //Polyhedra ????????
  }

  //Loop over faces
  for(GModel::fiter it = gModel->firstFace(); it != gModel->lastFace(); ++it)
  {
    GFace *f = *it;

    //Triangles
    for(unsigned int i = 0; i < f->triangles.size(); i++)
    {
      const int tag = f->triangles[i]->getNum()-1;

      for(unsigned int j = 0; j < f->triangles[i]->getNumVertices(); j++)
      {
	elementsNodesMap[tag].push_back(f->triangles[i]->getVertex(j)->getNum()-1);
      }
    }

    //Quadrangles
    for(unsigned int i = 0; i < f->quadrangles.size(); i++)
    {
      const int tag = f->quadrangles[i]->getNum()-1;

      for(unsigned int j = 0; j < f->quadrangles[i]->getNumVertices(); j++)
      {
	elementsNodesMap[tag].push_back(f->quadrangles[i]->getVertex(j)->getNum()-1);
      }
    }

    //Polygons ????????
  }

  //Loop over edges
  for(GModel::eiter it = gModel->firstEdge(); it != gModel->lastEdge(); ++it)
  {
    GEdge *e = *it;

    //Lines
    for(unsigned int i = 0; i < e->lines.size(); i++)
    {
      const int tag = e->lines[i]->getNum()-1;

      for(unsigned int j = 0; j < e->lines[i]->getNumVertices(); j++)
      {
          elementsNodesMap[tag].push_back(e->lines[i]->getVertex(j)->getNum()-1);
      }
    }
  }

  //Loop over vertices
  for(GModel::viter it = gModel->firstVertex(); it != gModel->lastVertex(); ++it)
  {
    GVertex *v = *it;

    for(unsigned int i = 0; i < v->points.size(); i++)
    {
      const int tag = v->points[i]->getNum()-1;

      //Points
      for(unsigned int j = 0; j < v->points[i]->getNumVertices(); j++)
      {
          elementsNodesMap[tag].push_back(v->points[i]->getVertex(j)->getNum()-1);
      }
    }
  }

  //create mesh format for METIS
  int position = 0;
  eptr[0] = 0;
  int i = 0;
  for(std::map<int, std::vector<int> >::iterator it = elementsNodesMap.begin(); it != elementsNodesMap.end(); ++it)
  {
    metisToGmshIndex[i] = it->first + 1;
    position += it->second.size();
    eptr[i+1] = position;
    i++;
  }

  int *eindTemp = new int[position];
  int positionInd = 0;
  for(std::map<int, std::vector<int> >::iterator it = elementsNodesMap.begin(); it != elementsNodesMap.end(); ++it)
  {
    for(unsigned int j = 0; j < it->second.size(); j++)
    {
      eindTemp[positionInd] = it->second.at(j);
      positionInd++;
    }
  }
  *eind = eindTemp;
}
