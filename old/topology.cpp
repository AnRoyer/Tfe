#include <iostream>
#include <string>
#include <algorithm>

#include "MElement.h"
#include "MVertex.h"

#include "MTetrahedron.h"
#include "MHexahedron.h"
#include "MPrism.h"
#include "MPyramid.h"
#include "MTrihedron.h"

#include "MTriangle.h"
#include "MQuadrangle.h"

#include "MLine.h"

#include "MPoint.h"

#include "topology.h"

std::vector<GModel *> *splitMesh(GModel* gModel, int nparts)
{
    std::vector<GModel *> *models = new std::vector<GModel *>();
    
    for(unsigned int i = 0; i < nparts; i++)
    {
        std::string str = "Partition ";
        str += i+1;
        
        models->push_back(new GModel(str.c_str()));
    }
    
    std::map<int, std::vector<int> > borderNodes = getBorderNodes(gModel);
    
    std::cout << "\tAssign vertices...";
    assignGVertex(models, gModel);
    std::cout << "Done!" << std::endl;
    
    std::cout << "\tCreate new vertices...";
    createNewGVertex(models, gModel, borderNodes);
    std::cout << "Done!" << std::endl;
    
    std::cout << "\tAssign edges...";
    assignGEdge(models, gModel, nparts);
    std::cout << "Done!" << std::endl;
    
    //createNewGEdge(models, gModel, borderNodes);
    
    return models;
}

void assignGVertex(std::vector<GModel *> *models, GModel* gModel)
{
    //Loop over vertices
    for(GModel::viter it = gModel->firstVertex(); it != gModel->lastVertex(); ++it)
    {
        GVertex *v = *it;
        
        for(unsigned int i = 0; i < v->points.size(); i++)
        {
            (*models)[v->points[i]->getPartition()]->setAsCurrent();
            MVertex *vertex = v->points[i]->getVertex(0);
            GVertex *entity = (*models)[v->points[i]->getPartition()]->addVertex(vertex->x(), vertex->y(), vertex->z(), 1.0);
            MVertex *newVertex = new MVertex(vertex->x(), vertex->y(), vertex->z(), entity);
            MPoint *point = new MPoint(newVertex);
            entity->addPoint(point);
            entity->addMeshVertex(newVertex);
        }
    }
}

void assignGEdge(std::vector<GModel *> *models, GModel* gModel, int nparts)
{
    //Loop over edges
    for(GModel::eiter it = gModel->firstEdge(); it != gModel->lastEdge(); ++it)
    {
        GEdge *e = *it;
        
        std::vector<MVertex*> vectorVertex;
        std::vector<MLine*> vectorLine;
        
        int currentPartition = e->lines[0]->getPartition();
        
        MVertex *first = e->lines[0]->getVertex(0);
        
        //Lines
        for(unsigned int i = 1; i < e->lines.size(); i++)
        {
            (*models)[currentPartition]->setAsCurrent();
                
            if(e->lines[i]->getPartition() != currentPartition)
            {
                MVertex *last = e->lines[i]->getVertex(0);
                
                SPoint3 p0 = first->point();
                SPoint3 p1 = last->point();
                
                //Already exists
                MVertex *newFirstVertex = (*models)[currentPartition]->getMeshElementByCoord(p0,0,false)->getVertex(0);
                MVertex *newLastVertex = (*models)[currentPartition]->getMeshElementByCoord(p1,0,false)->getVertex(0);
                
                GVertex *GV0 = static_cast<GVertex*>(newFirstVertex->onWhat());
                GVertex *GV1 = static_cast<GVertex*>(newLastVertex->onWhat());
                    
                //Create the new GEdge
                GEdge *entity = (*models)[currentPartition]->addLine(GV0, GV1);
                    
                //Assign new MVertex
                for(unsigned int j = 0; j < vectorVertex.size(); j++)
                {
                    vectorVertex[j]->setEntity(entity);
                    entity->addMeshVertex(vectorVertex[j]);
                }
                
                MLine *firstLine = new MLine(newFirstVertex,vectorVertex.front());
                MLine *lastLine = new MLine(vectorVertex.back(), newLastVertex);
                    
                //Assign new MLine
                entity->addLine(firstLine);
                for(unsigned int j = 0; j < vectorLine.size(); j++)
                {
                    entity->addLine(vectorLine[j]);
                }
                entity->addLine(lastLine);
                    
                vectorVertex.clear();
                vectorLine.clear();
                currentPartition = e->lines[i]->getPartition();
                first = last;
            }
            else
            {
                MVertex *v0 = e->lines[i]->getVertex(0);
                MVertex *newVertex0 = new MVertex(v0->x(), v0->y(), v0->z(), nullptr);
                
                if(vectorVertex.size() > 0)
                {
                    MLine *line = new MLine(vectorVertex.back(),newVertex0);
                    vectorLine.push_back(line);
                }
                vectorVertex.push_back(newVertex0);
                
            }
        }
        
        MVertex *last = e->lines[e->lines.size()-1]->getVertex(1);
        
        SPoint3 p0 = first->point();
        SPoint3 p1 = last->point();
        
        //Already exists
        MVertex *newFirstVertex = (*models)[currentPartition]->getMeshElementByCoord(p0,0,false)->getVertex(0);
        MVertex *newLastVertex = (*models)[currentPartition]->getMeshElementByCoord(p1,0,false)->getVertex(0);
        
        GVertex *GV0 = static_cast<GVertex*>(newFirstVertex->onWhat());
        GVertex *GV1 = static_cast<GVertex*>(newLastVertex->onWhat());
        
        //Create the new GEdge
        GEdge *entity = (*models)[currentPartition]->addLine(GV0, GV1);
        
        //Assign new MVertex
        for(unsigned int j = 0; j < vectorVertex.size(); j++)
        {
            vectorVertex[j]->setEntity(entity);
            entity->addMeshVertex(vectorVertex[j]);
        }
        
        MLine *firstLine = new MLine(newFirstVertex,vectorVertex.front());
        MLine *lastLine = new MLine(vectorVertex.back(), newLastVertex);
        
        //Assign new MLine
        entity->addLine(firstLine);
        for(unsigned int j = 0; j < vectorLine.size(); j++)
        {
            entity->addLine(vectorLine[j]);
        }
        entity->addLine(lastLine);
        
        vectorVertex.clear();
        vectorLine.clear();
    }
}

void createNewGVertex(std::vector<GModel *> *models, GModel* gModel, std::map<int, std::vector<int> > borderNodes)
{
    //Create new GVertex when a node has two or more partition
    for(std::map<int, std::vector<int> >::iterator it = borderNodes.begin(); it != borderNodes.end(); ++it)
    {
        MVertex *vertex = gModel->getMeshVertexByTag(it->first);
        for(unsigned int i = 0; i < it->second.size(); i++)
        {
            (*models)[it->second[i]]->setAsCurrent();
            GVertex *entity = (*models)[it->second[i]]->addVertex(vertex->x(), vertex->y(), vertex->z(), 1.0);
            MVertex *newVertex = new MVertex(vertex->x(), vertex->y(), vertex->z(), entity);
            MPoint *point = new MPoint(newVertex);
            entity->addPoint(point);
            entity->addMeshVertex(newVertex);
        }
    }
}

void createNewGEdge(std::vector<GModel *> *models, GModel* gModel, std::map<int, std::vector<int> > borderNodes)
{
    //Loop over regions
    for(GModel::riter it = gModel->firstRegion(); it != gModel->lastRegion(); ++it)
    {
        GRegion *r = *it;
        
        //Tetrahedra
        for(unsigned int i = 0; i < r->tetrahedra.size(); i++)
        {
            for(unsigned int j = 0; j < r->tetrahedra[i]->getNumVertices(); j++)
            {
                if(borderNodes.count(r->tetrahedra[i]->getVertex(j)->getNum()) > 0)
                {
                    for(unsigned int k = j+1; k < r->tetrahedra[i]->getNumVertices(); k++)
                    {
                        if(borderNodes.count(r->tetrahedra[i]->getVertex(k)->getNum()) > 0)
                        {
                            std::vector<int> partitions;
                            
                            for(unsigned int l = 0; l < borderNodes[r->tetrahedra[i]->getVertex(j)->getNum()].size(); l++)
                            {
                                for(unsigned int m = 0; m < borderNodes[r->tetrahedra[i]->getVertex(k)->getNum()].size(); k++)
                                {
                                    if(borderNodes[r->tetrahedra[i]->getVertex(j)->getNum()][l] == borderNodes[r->tetrahedra[i]->getVertex(k)->getNum()][m])
                                    {
                                        partitions.push_back(borderNodes[r->tetrahedra[i]->getVertex(j)->getNum()][l]);
                                        break;
                                    }
                                }
                            }
                            
                            createEdge(models, gModel, r->tetrahedra[i]->getVertex(j)->getNum(), r->tetrahedra[i]->getVertex(k)->getNum(), partitions);
                        }
                    }
                }
            }
        }
        
        //Hexahedra
        for(unsigned int i = 0; i < r->hexahedra.size(); i++)
        {
            for(unsigned int j = 0; j < r->hexahedra[i]->getNumVertices(); j++)
            {
            }
        }
        
        //Prisms
        for(unsigned int i = 0; i < r->prisms.size(); i++)
        {
            for(unsigned int j = 0; j < r->prisms[i]->getNumVertices(); j++)
            {
            }
        }
        
        //Pyramids
        for(unsigned int i = 0; i < r->pyramids.size(); i++)
        {
            for(unsigned int j = 0; j < r->pyramids[i]->getNumVertices(); j++)
            {
            }
        }
        
        //Trihedra
        for(unsigned int i = 0; i < r->trihedra.size(); i++)
        {
            for(unsigned int j = 0; j < r->trihedra[i]->getNumVertices(); j++)
            {
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
            for(unsigned int j = 0; j < f->triangles[i]->getNumVertices(); j++)
            {
                if(borderNodes.count(f->triangles[i]->getVertex(j)->getNum()) > 0)
                {
                    for(unsigned int k = j+1; k < f->triangles[i]->getNumVertices(); k++)
                    {
                        if(borderNodes.count(f->triangles[i]->getVertex(k)->getNum()) > 0)
                        {
                            std::vector<int> partitions;
                            
                            for(unsigned int l = 0; l < borderNodes[f->triangles[i]->getVertex(j)->getNum()].size(); l++)
                            {
                                for(unsigned int m = 0; m < borderNodes[f->triangles[i]->getVertex(k)->getNum()].size(); k++)
                                {
                                    if(borderNodes[f->triangles[i]->getVertex(j)->getNum()][l] == borderNodes[f->triangles[i]->getVertex(k)->getNum()][m])
                                    {
                                        partitions.push_back(borderNodes[f->triangles[i]->getVertex(j)->getNum()][l]);
                                        break;
                                    }
                                }
                            }
                            
                            createEdge(models, gModel, f->triangles[i]->getVertex(j)->getNum(), f->triangles[i]->getVertex(k)->getNum(), partitions);
                        }
                    }
                }
            }
        }
        
        //Quadrangles
        for(unsigned int i = 0; i < f->quadrangles.size(); i++)
        {
            for(unsigned int j = 0; j < f->quadrangles[i]->getNumVertices(); j++)
            {
            }
        }
        
        //Polygons ????????
    }
}

std::map<int, std::vector<int> > getBorderNodes(GModel* gModel)
{
    std::map<int, std::vector<int> > partitionByNode;
    
    //Loop over regions
    for(GModel::riter it = gModel->firstRegion(); it != gModel->lastRegion(); ++it)
    {
        GRegion *r = *it;
        
        //Tetrahedra
        for(unsigned int i = 0; i < r->tetrahedra.size(); i++)
        {
            for(unsigned int j = 0; j < r->tetrahedra[i]->getNumVertices(); j++)
            {
                partitionByNode[r->tetrahedra[i]->getVertex(j)->getNum()].push_back(r->tetrahedra[i]->getPartition());
            }
        }
        
        //Hexahedra
        for(unsigned int i = 0; i < r->hexahedra.size(); i++)
        {
            for(unsigned int j = 0; j < r->hexahedra[i]->getNumVertices(); j++)
            {
                partitionByNode[r->hexahedra[i]->getVertex(j)->getNum()].push_back(r->hexahedra[i]->getPartition());
            }
        }
        
        //Prisms
        for(unsigned int i = 0; i < r->prisms.size(); i++)
        {
            for(unsigned int j = 0; j < r->prisms[i]->getNumVertices(); j++)
            {
                partitionByNode[r->prisms[i]->getVertex(j)->getNum()].push_back(r->prisms[i]->getPartition());
            }
        }
        
        //Pyramids
        for(unsigned int i = 0; i < r->pyramids.size(); i++)
        {
            for(unsigned int j = 0; j < r->pyramids[i]->getNumVertices(); j++)
            {
                partitionByNode[r->pyramids[i]->getVertex(j)->getNum()].push_back(r->pyramids[i]->getPartition());
            }
        }
        
        //Trihedra
        for(unsigned int i = 0; i < r->trihedra.size(); i++)
        {
            for(unsigned int j = 0; j < r->trihedra[i]->getNumVertices(); j++)
            {
                partitionByNode[r->trihedra[i]->getVertex(j)->getNum()].push_back(r->trihedra[i]->getPartition());
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
            for(unsigned int j = 0; j < f->triangles[i]->getNumVertices(); j++)
            {
                partitionByNode[f->triangles[i]->getVertex(j)->getNum()].push_back(f->triangles[i]->getPartition());
            }
        }
        
        //Quadrangles
        for(unsigned int i = 0; i < f->quadrangles.size(); i++)
        {
            for(unsigned int j = 0; j < f->quadrangles[i]->getNumVertices(); j++)
            {
                partitionByNode[f->quadrangles[i]->getVertex(j)->getNum()].push_back(f->quadrangles[i]->getPartition());
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
            for(unsigned int j = 0; j < e->lines[i]->getNumVertices(); j++)
            {
                partitionByNode[e->lines[i]->getVertex(j)->getNum()].push_back(e->lines[i]->getPartition());
            }
        }
    }
    
    //Looking for nodes that have two or more partition
    std::map<int, std::vector<int> > borderNodes;
    
    for(std::map<int, std::vector<int> >::iterator it = partitionByNode.begin(); it != partitionByNode.end(); ++it)
    {
        int part = it->second[0];
        for(unsigned int i = 1; i < it->second.size(); i++)
        {
            if(part != it->second[i])
            {
                borderNodes[it->first].push_back(it->second[0]);
                std::vector<int>::iterator vectorIt;
                
                for(unsigned int k = 1; k < it->second.size(); k++)
                {
                    vectorIt = std::find(borderNodes[it->first].begin(), borderNodes[it->first].end(), it->second[k]);
                    
                    if (vectorIt == borderNodes[it->first].end())
                    {
                        borderNodes[it->first].push_back(it->second[k]);
                    }
                }
            }
        }
    }
    partitionByNode.clear();
    
    return borderNodes;
}

void createEdge(std::vector<GModel *> *models, GModel *gModel, int num0, int num1, std::vector<int> partitions)
{
    MVertex *vertex0 = gModel->getMeshVertexByTag(num0);
    MVertex *vertex1 = gModel->getMeshVertexByTag(num1);
    
    SPoint3 p0 = vertex0->point();
    SPoint3 p1 = vertex1->point();
    
    std::cout << num0 << " = (" << p0.x() << "," << p0.y() << "," << p0.z() << ")" << std::endl;
    std::cout << num1 << " = (" << p1.x() << "," << p1.y() << "," << p1.z() << ")" << std::endl;
    
    for(unsigned int i = 0; i < partitions.size(); i++)
    {
        
        MVertex *firstVertex = (*models)[partitions[i]]->getMeshElementByCoord(p0,0,false)->getVertex(0);
        MVertex *lastVertex = (*models)[partitions[i]]->getMeshElementByCoord(p1,0,false)->getVertex(0);
        std::cout << "ok" << std::endl;
        
        GVertex *GV0 = static_cast<GVertex*>(firstVertex->onWhat());
        GVertex *GV1 = static_cast<GVertex*>(lastVertex->onWhat());
        
        //Create the new GEdge
        GEdge *entity = (*models)[partitions[i]]->addLine(GV0, GV1);
        
        //Create the new MLine
        
        MLine *mLine = new MLine(firstVertex,lastVertex);
        
        //Assign new MLine
        entity->addLine(mLine);
    }
}

void assignOmegaTopology(GModel *gModel)
{
    
}
