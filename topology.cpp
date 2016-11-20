#include <iostream>
#include <set>
#include <map>
#include <string>

#include "MElement.h"

#include "MTetrahedron.h"
#include "MHexahedron.h"
#include "MPrism.h"
#include "MPyramid.h"
#include "MTrihedron.h"

#include "MTriangle.h"
#include "MQuadrangle.h"

#include "MPoint.h"

#include "MElementCut.h"

#include "discreteRegion.h"
#include "discreteFace.h"
#include "discreteEdge.h"
#include "discreteVertex.h"

#include "partitionFace.h"
#include "partitionEdge.h"
#include "partitionVertex.h"

#include "topology.h"

int createPartitionBoundaries(GModel *model, bool createGhostCells)
{
    unsigned int numElem[6];
    const int meshDim = model->getNumMeshElements(numElem);
    
    std::set<partitionFace*, Less_partitionFace> pfaces;
    std::set<partitionEdge*, Less_partitionEdge> pedges;
    std::set<partitionVertex*, Less_partitionVertex> pvertices;
    
    std::multimap<MFace, MElement*, Less_Face> faceToElement;
    std::multimap<MEdge, MElement*, Less_Edge> edgeToElement;
    std::multimap<MVertex*, MElement*> vertexToElement;
    
    //Create partition faces
    std::cout << "\tCreate partition faces... " << std::flush;
    if (meshDim == 3)
    {
        for(GModel::riter it = model->firstRegion(); it != model->lastRegion(); ++it)
        {
            fillit_(faceToElement, (*it)->tetrahedra.begin(), (*it)->tetrahedra.end());
            fillit_(faceToElement, (*it)->hexahedra.begin(), (*it)->hexahedra.end());
            fillit_(faceToElement, (*it)->prisms.begin(), (*it)->prisms.end());
            fillit_(faceToElement, (*it)->pyramids.begin(), (*it)->pyramids.end());
            fillit_(faceToElement, (*it)->trihedra.begin(), (*it)->trihedra.end());
            fillit_(faceToElement, (*it)->polyhedra.begin(), (*it)->polyhedra.end());
        }
        
        std::multimap<MFace, MElement*, Less_Face>::iterator it = faceToElement.begin();
        Equal_Face oper;
        
        while (it != faceToElement.end())
        {
            MFace f = it->first;
            std::vector<MElement*> voe;
            
            //Take all elements having f as face.
            do {
                voe.push_back(it->second);
                ++it;
                if (it ==  faceToElement.end())
                {
                    break;
                }
            }while (oper (f,it->first));
            
            assignPartitionBoundary(model, f, pfaces, voe);
        }
    }
    std::cout << "Done!" << std::endl;
    
    //Create partition edges
    std::cout << "\tCreate partition edges... " << std::flush;
    if (meshDim > 1)
    {
        if (meshDim == 2)
        {
            for(GModel::fiter it = model->firstFace(); it != model->lastFace(); ++it)
            {
                fillit_(edgeToElement, (*it)->triangles.begin(), (*it)->triangles.end());
                fillit_(edgeToElement, (*it)->quadrangles.begin(), (*it)->quadrangles.end());
                fillit_(edgeToElement, (*it)->polygons.begin(), (*it)->polygons.end());
            }
        }
        
        if (meshDim == 3)
        {
            for(GModel::riter it = model->firstRegion(); it != model->lastRegion(); ++it)
            {
                fillit_(edgeToElement, (*it)->tetrahedra.begin(), (*it)->tetrahedra.end());
                fillit_(edgeToElement, (*it)->hexahedra.begin(), (*it)->hexahedra.end());
                fillit_(edgeToElement, (*it)->prisms.begin(), (*it)->prisms.end());
                fillit_(edgeToElement, (*it)->pyramids.begin(), (*it)->pyramids.end());
                fillit_(edgeToElement, (*it)->trihedra.begin(), (*it)->trihedra.end());
                fillit_(edgeToElement, (*it)->polyhedra.begin(), (*it)->polyhedra.end());
            }
        }
        
        std::multimap<MEdge, MElement*, Less_Edge>::iterator it = edgeToElement.begin();
        Equal_Edge oper;
        
        //Take all elements having e as edge.
        while (it != edgeToElement.end())
        {
            MEdge e = it->first;
            std::vector<MElement*> voe;
            
            do {
                voe.push_back(it->second);
                ++it;
                if (it ==  edgeToElement.end())
                {
                    break;
                }
            }while (oper (e,it->first));
            
            assignPartitionBoundary(model, e, pedges, voe, pfaces);
        }
    }
    std::cout << "Done!" << std::endl;
    
    //Create partition vertices
    std::cout << "\tCreate partition vertices... "  << std::flush;
    if (meshDim > 1)
    {
        if (meshDim == 2)
        {
            for(GModel::fiter it = model->firstFace(); it != model->lastFace(); ++it)
            {
                fillit_(vertexToElement, (*it)->triangles.begin(), (*it)->triangles.end());
                fillit_(vertexToElement, (*it)->quadrangles.begin(), (*it)->quadrangles.end());
                fillit_(vertexToElement, (*it)->polygons.begin(), (*it)->polygons.end());
            }
        }
        
        if (meshDim == 3)
        {
            for(GModel::riter it = model->firstRegion(); it != model->lastRegion(); ++it)
            {
                fillit_(vertexToElement, (*it)->tetrahedra.begin(), (*it)->tetrahedra.end());
                fillit_(vertexToElement, (*it)->hexahedra.begin(), (*it)->hexahedra.end());
                fillit_(vertexToElement, (*it)->prisms.begin(), (*it)->prisms.end());
                fillit_(vertexToElement, (*it)->pyramids.begin(), (*it)->pyramids.end());
                fillit_(vertexToElement, (*it)->trihedra.begin(), (*it)->trihedra.end());
                fillit_(vertexToElement, (*it)->polyhedra.begin(), (*it)->polyhedra.end());
            }
        }
        
        std::multimap<MVertex*, MElement*>::iterator it = vertexToElement.begin();
        
        //Take all elements having v as vertex.
        while (it != vertexToElement.end())
        {
            MVertex *v = it->first;
            std::vector<MElement*> voe;
            
            do {
                voe.push_back(it->second);
                ++it;
                if (it ==  vertexToElement.end())
                {
                    break;
                }
            }while (v == it->first);
            
            assignPartitionBoundary(model, v, pvertices, voe, pedges, pfaces);
        }
    }
    std::cout << "Done!" << std::endl;
    
    /*
     // create vertex-based ghost cells (i.e., elements that touch the
     // partition boundaries by at least one vertex)
     if(createGhostCells){
     std::multimap<MElement*, short> &ghosts(model->getGhostCells());
     ghosts.clear();
     if(meshDim == 2 || createAllDims)
     for(std::set<partitionEdge*, Less_partitionEdge>::iterator it = pedges.begin();
     it != pedges.end(); it++)
     addGhostCells(*it, vertexToElement, ghosts);
     if(meshDim == 3)
     for(std::set<partitionFace*, Less_partitionFace>::iterator it = pfaces.begin();
     it != pfaces.end(); it++)
     addGhostCells(*it, vertexToElement, ghosts);
     }
     */
    
    return 1;
}

template <class ITERATOR>
void fillit_(std::multimap<MFace, MElement*, Less_Face> &faceToElement, ITERATOR it_beg, ITERATOR it_end)
{
    for (ITERATOR IT = it_beg; IT != it_end ; ++IT)
    {
        MElement *el = *IT;
        for(unsigned int j = 0; j < el->getNumFaces(); j++)
        {
            const MFace e = el->getFace(j);
            faceToElement.insert(std::pair<MFace, MElement*>(e, el));
        }
    }
}

template <class ITERATOR>
void fillit_(std::multimap<MEdge, MElement*, Less_Edge> &edgeToElement, ITERATOR it_beg, ITERATOR it_end)
{
    for (ITERATOR IT = it_beg; IT != it_end; ++IT)
    {
        MElement *el = *IT;
        for(unsigned int j = 0; j < el->getNumEdges(); j++)
        {
            const MEdge e = el->getEdge(j);
            edgeToElement.insert(std::pair<MEdge, MElement*>(e, el));
        }
    }
}

template <class ITERATOR>
void fillit_(std::multimap<MVertex*, MElement*> &vertexToElement, ITERATOR it_beg, ITERATOR it_end)
{
    for (ITERATOR IT = it_beg; IT != it_end ; ++IT)
    {
        MElement *el = *IT;
        for(unsigned int j = 0; j < el->getNumVertices(); j++)
        {
            MVertex* e = el->getVertex(j);
            vertexToElement.insert(std::pair<MVertex*, MElement*>(e, el));
        }
    }
}

void assignPartitionBoundary(GModel *model, MFace &me, std::set<partitionFace*, Less_partitionFace> &pfaces, std::vector<MElement*> &v)
{
    std::vector<int> v2;
    v2.push_back(v[0]->getPartition());
    
    for (unsigned int i = 1; i < v.size(); i++)
    {
        bool found = false;
        for (unsigned int j = 0; j < v2.size(); j++)
        {
            if (v[i]->getPartition() == v2[j])
            {
                found = true;
                break;
            }
        }
        
        if (!found)
        {
            v2.push_back(v[i]->getPartition());
        }
    }
    
    //If there is less than two partitions touching the face: stop
    if (v2.size() < 2)
    {
        return;
    }
    
    partitionFace pf(model, 1, v2);
    std::set<partitionFace*, Less_partitionFace>::iterator it = pfaces.find(&pf);
    partitionFace *ppf;
    
    if (it == pfaces.end())
    {
        ppf = new  partitionFace(model, -(int)pfaces.size()-1, v2);
        pfaces.insert(ppf);
        model->add(ppf);
    }
    else
    {
        ppf = *it;
    }
    // to finish !!
    if (me.getNumVertices() == 3)
    {
        ppf->triangles.push_back(new MTriangle(me.getVertex(0), me.getVertex(1), me.getVertex(2)));
    }
    else
    {
        ppf->quadrangles.push_back(new MQuadrangle(me.getVertex(0), me.getVertex(1), me.getVertex(2), me.getVertex(3)));
    }
}

void assignPartitionBoundary(GModel *model, MEdge &me, std::set<partitionEdge*, Less_partitionEdge> &pedges, std::vector<MElement*> &v, std::set<partitionFace*, Less_partitionFace> &pfaces)
{
    std::vector<int> v2;
    v2.push_back(v[0]->getPartition());
    
    for (unsigned int i = 1; i < v.size(); i++)
    {
        bool found = false;
        for (unsigned int j = 0; j < v2.size(); j++)
        {
            if (v[i]->getPartition() == v2[j])
            {
                found = true;
                break;
            }
        }
        
        if (!found)
        {
            v2.push_back(v[i]->getPartition());
        }
    }
    
    //If there is less than two partitions touching the edge: stop
    if (v2.size() < 2)
    {
        return;
    }
    
    partitionFace pf(model, 1, v2);
    std::set<partitionFace*, Less_partitionFace>::iterator itf = pfaces.find(&pf);
    
    //If the edge is on a partitionFace
    if (itf != pfaces.end())
    {
        return;
    }
    
    partitionEdge pe  (model, 1, 0, 0, v2);
    std::set<partitionEdge*, Less_partitionEdge>::iterator it = pedges.find(&pe);
    
    partitionEdge *ppe;
    if (it == pedges.end())
    {
        ppe = new  partitionEdge(model, -(int)pedges.size()-1, 0, 0, v2);
        pedges.insert(ppe);
        model->add(ppe);
    }
    else
    {
        ppe = *it;
    }
    
    ppe->lines.push_back(new MLine(me.getVertex(0),me.getVertex(1)));
}

void assignPartitionBoundary(GModel *model, MVertex *ve, std::set<partitionVertex*, Less_partitionVertex> &pvertices, std::vector<MElement*> &v, std::set<partitionEdge*, Less_partitionEdge> &pedges, std::set<partitionFace*, Less_partitionFace> &pfaces)
{
    std::vector<int> v2;
    v2.push_back(v[0]->getPartition());
    
    for (unsigned int i = 1; i < v.size(); i++)
    {
        bool found = false;
        for (unsigned int j = 0; j < v2.size(); j++)
        {
            if (v[i]->getPartition() == v2[j])
            {
                found = true;
                break;
            }
        }
        
        if (!found)
        {
            v2.push_back(v[i]->getPartition());
        }
    }
    
    //If there is less than two partitions touching the vertex: stop
    if (v2.size() < 2)
    {
        return;
    }
    
    partitionFace pf(model, 1, v2);
    std::set<partitionFace*, Less_partitionFace>::iterator itf = pfaces.find(&pf);
    
    //If the vertex is on a partitionFace
    if (itf != pfaces.end())
    {
        return;
    }
    
    partitionEdge pe(model, 1, 0, 0, v2);
    std::set<partitionEdge*, Less_partitionEdge>::iterator ite = pedges.find(&pe);
    
    //If the vertex is on a partitionFace
    if (ite != pedges.end())
    {
        return;
    }
    
    partitionVertex pv(model, 1, v2);
    std::set<partitionVertex*, Less_partitionVertex>::iterator it = pvertices.find(&pv);
    
    partitionVertex *ppv;
    if (it == pvertices.end())
    {
        ppv = new  partitionVertex(model, -(int)pvertices.size()-1,v2);
        pvertices.insert (ppv);
        model->add(ppv);
    }
    else
    {
        ppv = *it;
    }
    
    ppv->points.push_back(new MPoint(ve));
}

std::vector<GModel*> createNewModels(GModel *gModel, int nparts)
{
    std::vector<GModel*> newModels(nparts, nullptr);
    
    for (unsigned int i = 0; i < nparts; i++)
    {
        newModels[i] = new GModel();
    }
    
    //Loop over regions
    for(GModel::riter it = gModel->firstRegion(); it != gModel->lastRegion(); ++it)
    {
        GRegion *r = *it;
        
        std::vector<GRegion *> modelsHaveRegion(nparts, nullptr);
        
        //Tetrahedra
        for(unsigned int i = 0; i < r->tetrahedra.size(); i++)
        {
            const int partition = r->tetrahedra[i]->getPartition()-1;
            if(!modelsHaveRegion[partition])
            {
                discreteRegion *dr = new discreteRegion(newModels[partition], newModels[partition]->getNumRegions()+1);
                newModels[partition]->add(dr);
                modelsHaveRegion[partition] = dr;
                addPhysical(newModels[partition], dr, gModel, r, partition+1);
            }
            
            modelsHaveRegion[r->tetrahedra[i]->getPartition()-1]->tetrahedra.push_back(r->tetrahedra[i]);
        }
        

        //Hexahedra
        for(unsigned int i = 0; i < r->hexahedra.size(); i++)
        {
            const int partition = r->hexahedra[i]->getPartition()-1;
            if(!modelsHaveRegion[partition])
            {
                discreteRegion *dr = new discreteRegion(newModels[partition], newModels[partition]->getNumRegions()+1);
                newModels[partition]->add(dr);
                modelsHaveRegion[partition] = dr;
                addPhysical(newModels[partition], dr, gModel, r, partition+1);
            }
            
            modelsHaveRegion[r->hexahedra[i]->getPartition()-1]->hexahedra.push_back(r->hexahedra[i]);
        }
        
        //Prisms
        for(unsigned int i = 0; i < r->prisms.size(); i++)
        {
            const int partition = r->prisms[i]->getPartition()-1;
            if(!modelsHaveRegion[partition])
            {
                discreteRegion *dr = new discreteRegion(newModels[partition], newModels[partition]->getNumRegions()+1);
                newModels[partition]->add(dr);
                modelsHaveRegion[partition] = dr;
                addPhysical(newModels[partition], dr, gModel, r, partition+1);
            }
            
            modelsHaveRegion[r->prisms[i]->getPartition()-1]->prisms.push_back(r->prisms[i]);
        }
        
        //Pyramids
        for(unsigned int i = 0; i < r->pyramids.size(); i++)
        {
            const int partition = r->pyramids[i]->getPartition()-1;
            if(!modelsHaveRegion[partition])
            {
                discreteRegion *dr = new discreteRegion(newModels[partition], newModels[partition]->getNumRegions()+1);
                newModels[partition]->add(dr);
                modelsHaveRegion[partition] = dr;
                addPhysical(newModels[partition], dr, gModel, r, partition+1);
            }
            
            modelsHaveRegion[r->pyramids[i]->getPartition()-1]->pyramids.push_back(r->pyramids[i]);
        }
        
        //Trihedra
        for(unsigned int i = 0; i < r->trihedra.size(); i++)
        {
            const int partition = r->trihedra[i]->getPartition()-1;
            if(!modelsHaveRegion[partition])
            {
                discreteRegion *dr = new discreteRegion(newModels[partition], newModels[partition]->getNumRegions()+1);
                newModels[partition]->add(dr);
                modelsHaveRegion[partition] = dr;
                addPhysical(newModels[partition], dr, gModel, r, partition+1);
            }
            
            modelsHaveRegion[r->trihedra[i]->getPartition()-1]->trihedra.push_back(r->trihedra[i]);
        }
        
        //Polyhedra ????????
    }
    
    //Loop over faces
    for(GModel::fiter it = gModel->firstFace(); it != gModel->lastFace(); ++it)
    {
        GFace *f = *it;
        
        std::vector<GFace *> modelsHaveFace(nparts, nullptr);
        
        //Triangles
        for(unsigned int i = 0; i < f->triangles.size(); i++)
        {
            const int partition = f->triangles[i]->getPartition()-1;
            if(!modelsHaveFace[partition])
            {
                discreteFace *df = new discreteFace(newModels[partition], newModels[partition]->getNumFaces()+1);
                newModels[partition]->add(df);
                modelsHaveFace[partition] = df;
                addPhysical(newModels[partition], df, gModel, f, partition+1);
            }
            
            modelsHaveFace[f->triangles[i]->getPartition()-1]->triangles.push_back(f->triangles[i]);
        }
        
        //Quadrangles
        for(unsigned int i = 0; i < f->quadrangles.size(); i++)
        {
            const int partition = f->quadrangles[i]->getPartition()-1;
            if(!modelsHaveFace[partition])
            {
                discreteFace *df = new discreteFace(newModels[partition], newModels[partition]->getNumFaces()+1);
                newModels[partition]->add(df);
                modelsHaveFace[partition] = df;
                addPhysical(newModels[partition], df, gModel, f, partition+1);
            }
            
            modelsHaveFace[f->quadrangles[i]->getPartition()-1]->quadrangles.push_back(f->quadrangles[i]);
        }
        
        //Polygons ????????
    }
    
    //Loop over edges
    for(GModel::eiter it = gModel->firstEdge(); it != gModel->lastEdge(); ++it)
    {
        GEdge *e = *it;
        
        std::vector<GEdge *> modelsHaveEdge(nparts, nullptr);
        
        //Lines
        for(unsigned int i = 0; i < e->lines.size(); i++)
        {
            const int partition = e->lines[i]->getPartition()-1;
            if(!modelsHaveEdge[partition])
            {
                discreteEdge *de = new discreteEdge(newModels[partition], newModels[partition]->getNumEdges()+1, nullptr, nullptr);
                newModels[partition]->add(de);
                modelsHaveEdge[partition] = de;
                addPhysical(newModels[partition], de, gModel, e, partition+1);
            }
            
            modelsHaveEdge[e->lines[i]->getPartition()-1]->lines.push_back(e->lines[i]);
        }
    }
    
    //Loop over vertices
    for(GModel::viter it = gModel->firstVertex(); it != gModel->lastVertex(); ++it)
    {
        GVertex *v = *it;
        
        std::vector<GVertex *> modelsHaveVertex(nparts, nullptr);
        
        for(unsigned int i = 0; i < v->points.size(); i++)
        {
            const int partition = v->points[i]->getPartition()-1;
            if(!modelsHaveVertex[partition])
            {
                discreteVertex *dv = new discreteVertex(newModels[partition], newModels[partition]->getNumVertices()+1);
                newModels[partition]->add(dv);
                modelsHaveVertex[partition] = dv;
                addPhysical(newModels[partition], dv, gModel, v, partition+1);
            }
            
            modelsHaveVertex[v->points[i]->getPartition()-1]->points.push_back(v->points[i]);
        }
    }
    
    return newModels;
}

void assignMeshVerticesToModel(GModel *gModel)
{
    std::map<MVertex*, GEntity*> vertexToEntity;
    
    //Loop over regions
    for(GModel::riter it = gModel->firstRegion(); it != gModel->lastRegion(); ++it)
    {
        GRegion *r = *it;
        
        //Tetrahedra
        for(unsigned int i = 0; i < r->tetrahedra.size(); i++)
        {
            for(unsigned int j = 0; j < r->tetrahedra[i]->getNumVertices(); j++)
            {
                if(vertexToEntity.count(r->tetrahedra[i]->getVertex(j)) > 0)
                {
                    if(vertexToEntity[r->tetrahedra[i]->getVertex(j)]->dim() > r->dim())
                    {
                        vertexToEntity[r->tetrahedra[i]->getVertex(j)] = r;
                    }
                }
                else
                {
                    vertexToEntity[r->tetrahedra[i]->getVertex(j)] = r;
                }
            }
        }
        
        //Hexahedra
        for(unsigned int i = 0; i < r->hexahedra.size(); i++)
        {
            for(unsigned int j = 0; j < r->hexahedra[i]->getNumVertices(); j++)
            {
                if(vertexToEntity.count(r->hexahedra[i]->getVertex(j)) > 0)
                {
                    if(vertexToEntity[r->hexahedra[i]->getVertex(j)]->dim() > r->dim())
                    {
                        vertexToEntity[r->hexahedra[i]->getVertex(j)] = r;
                    }
                }
                else
                {
                    vertexToEntity[r->hexahedra[i]->getVertex(j)] = r;
                }
            }
        }
         
        //Prisms
        for(unsigned int i = 0; i < r->prisms.size(); i++)
        {
            for(unsigned int j = 0; j < r->prisms[i]->getNumVertices(); j++)
            {
                if(vertexToEntity.count(r->prisms[i]->getVertex(j)) > 0)
                {
                    if(vertexToEntity[r->prisms[i]->getVertex(j)]->dim() > r->dim())
                    {
                        vertexToEntity[r->prisms[i]->getVertex(j)] = r;
                    }
                }
                else
                {
                    vertexToEntity[r->prisms[i]->getVertex(j)] = r;
                }
            }
        }
         
        //Pyramids
        for(unsigned int i = 0; i < r->pyramids.size(); i++)
        {
            for(unsigned int j = 0; j < r->pyramids[i]->getNumVertices(); j++)
            {
                if(vertexToEntity.count(r->pyramids[i]->getVertex(j)) > 0)
                {
                    if(vertexToEntity[r->pyramids[i]->getVertex(j)]->dim() > r->dim())
                    {
                        vertexToEntity[r->pyramids[i]->getVertex(j)] = r;
                    }
                }
                else
                {
                    vertexToEntity[r->pyramids[i]->getVertex(j)] = r;
                }
            }
        }
         
        //Trihedra
        for(unsigned int i = 0; i < r->trihedra.size(); i++)
        {
            for(unsigned int j = 0; j < r->trihedra[i]->getNumVertices(); j++)
            {
                if(vertexToEntity.count(r->trihedra[i]->getVertex(j)) > 0)
                {
                    if(vertexToEntity[r->trihedra[i]->getVertex(j)]->dim() > r->dim())
                    {
                        vertexToEntity[r->trihedra[i]->getVertex(j)] = r;
                    }
                }
                else
                {
                    vertexToEntity[r->trihedra[i]->getVertex(j)] = r;
                }
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
                if(vertexToEntity.count(f->triangles[i]->getVertex(j)) > 0)
                {
                    if(vertexToEntity[f->triangles[i]->getVertex(j)]->dim() > f->dim())
                    {
                        vertexToEntity[f->triangles[i]->getVertex(j)] = f;
                    }
                }
                else
                {
                    vertexToEntity[f->triangles[i]->getVertex(j)] = f;
                }
            }
        }
    
        //Quadrangles
        for(unsigned int i = 0; i < f->quadrangles.size(); i++)
        {
            for(unsigned int j = 0; j < f->quadrangles[i]->getNumVertices(); j++)
            {
                if(vertexToEntity.count(f->quadrangles[i]->getVertex(j)) > 0)
                {
                    if(vertexToEntity[f->quadrangles[i]->getVertex(j)]->dim() > f->dim())
                    {
                        vertexToEntity[f->quadrangles[i]->getVertex(j)] = f;
                    }
                }
                else
                {
                    vertexToEntity[f->quadrangles[i]->getVertex(j)] = f;
                }
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
                if(vertexToEntity.count(e->lines[i]->getVertex(j)) > 0)
                {
                    if(vertexToEntity[e->lines[i]->getVertex(j)]->dim() > e->dim())
                    {
                        vertexToEntity[e->lines[i]->getVertex(j)] = e;
                    }
                }
                else
                {
                    vertexToEntity[e->lines[i]->getVertex(j)] = e;
                }
            }
        }
    }
    
    //Loop over vertices
    for(GModel::viter it = gModel->firstVertex(); it != gModel->lastVertex(); ++it)
    {
        GVertex *v = *it;
        
        for(unsigned int i = 0; i < v->points.size(); i++)
        {
            for(unsigned int j = 0; j < v->points[i]->getNumVertices(); j++)
            {
                if(vertexToEntity.count(v->points[i]->getVertex(j)) > 0)
                {
                    if(vertexToEntity[v->points[i]->getVertex(j)]->dim() > v->dim())
                    {
                        vertexToEntity[v->points[i]->getVertex(j)] = v;
                    }
                }
                else
                {
                    vertexToEntity[v->points[i]->getVertex(j)] = v;
                }
            }
        }
    }
    
    //Fill the entities
    for(std::map<MVertex*, GEntity*>::iterator it = vertexToEntity.begin(); it != vertexToEntity.end(); ++it)
    {
        it->second->addMeshVertex(it->first);
    }
}

void assignPartitionBoundariesToModels(GModel *gModel, std::vector<GModel*> &models)
{
    int maxNumPhysical = gModel->numPhysicalNames();
    
    //Loop over faces
    for(GModel::fiter it = gModel->firstFace(); it != gModel->lastFace(); ++it)
    {
        GFace *f = *it;
        
        if(f->geomType() == GEntity::PartitionSurface)
        {
            maxNumPhysical++;
            f->addPhysicalEntity(maxNumPhysical);
            
            std::string name = "_sigma{";
            for(unsigned int j = 0; j < static_cast<partitionFace*>(f)->_partitions.size(); j++)
            {
                if(j > 0)
                {
                    name += ",";
                }
                name += std::to_string(static_cast<partitionFace*>(f)->_partitions[j]);
            }
            name += "}";
            
            for(unsigned int j = 0; j < static_cast<partitionFace*>(f)->_partitions.size(); j++)
            {
                models[static_cast<partitionFace*>(f)->_partitions[j]-1]->add(f);
                models[static_cast<partitionFace*>(f)->_partitions[j]-1]->setPhysicalName(name, f->dim(), maxNumPhysical);
            }
        }
    }
    
    //Loop over edges
    for(GModel::eiter it = gModel->firstEdge(); it != gModel->lastEdge(); ++it)
    {
        GEdge *e = *it;
        
        if(e->geomType() == GEntity::PartitionCurve)
        {
            maxNumPhysical++;
            e->addPhysicalEntity(maxNumPhysical);
            
            std::string name = "_sigma{";
            for(unsigned int j = 0; j < static_cast<partitionEdge*>(e)->_partitions.size(); j++)
            {
                if(j > 0)
                {
                    name += ",";
                }
                name += std::to_string(static_cast<partitionEdge*>(e)->_partitions[j]);
            }
            name += "}";
            
            for(unsigned int j = 0; j < static_cast<partitionEdge*>(e)->_partitions.size(); j++)
            {
                models[static_cast<partitionEdge*>(e)->_partitions[j]-1]->add(e);
                models[static_cast<partitionEdge*>(e)->_partitions[j]-1]->setPhysicalName(name, e->dim(), maxNumPhysical);
            }
        }
    }
    
    //Loop over vertices
    for(GModel::viter it = gModel->firstVertex(); it != gModel->lastVertex(); ++it)
    {
        GVertex *v = *it;
        
        if(v->geomType() == GEntity::PartitionVertex)
        {
            maxNumPhysical++;
            v->addPhysicalEntity(maxNumPhysical);
            
            std::string name = "_sigma{";
            for(unsigned int j = 0; j < static_cast<partitionVertex*>(v)->_partitions.size(); j++)
            {
                if(j > 0)
                {
                    name += ",";
                }
                name += std::to_string(static_cast<partitionVertex*>(v)->_partitions[j]);
            }
            name += "}";
            
            for(unsigned int j = 0; j < static_cast<partitionVertex*>(v)->_partitions.size(); j++)
            {
                models[static_cast<partitionVertex*>(v)->_partitions[j]-1]->add(v);
                models[static_cast<partitionVertex*>(v)->_partitions[j]-1]->setPhysicalName(name, v->dim(), maxNumPhysical);
            }
        }
    }
}

void freeModels(std::vector<GModel*> *models)
{
    for(unsigned int i = 0; i < models->size(); i++)
    {
        std::vector<GEntity*> entities;
        (*models)[i]->getEntities(entities);
        
        for(unsigned int j = 0; j < entities.size(); j++)
        {
            entities[j]->mesh_vertices.clear();
            
            switch(entities[j]->dim())
            {
                case 3:
                    (*models)[i]->remove(static_cast<GRegion*>(entities[j]));
                    break;
                case 2:
                    (*models)[i]->remove(static_cast<GFace*>(entities[j]));
                    break;
                case 1:
                    (*models)[i]->remove(static_cast<GEdge*>(entities[j]));
                    break;
                case 0:
                    (*models)[i]->remove(static_cast<GVertex*>(entities[j]));
                    break;
                default:
                    break;
            }
        }
    }
}

void addPhysical(GModel *newModel, GEntity *newEntity, GModel *oldModel, GEntity *oldEntity, int partition)
{
    std::vector<int> oldPhysical = oldEntity->getPhysicalEntities();
    
    std::string name = "_omega{";
    name += std::to_string(partition);
    name += "}";
        
    const int number = newModel->setPhysicalName(name, newEntity->dim(), 0);
    newEntity->addPhysicalEntity(number);
    
    for(unsigned int i = 0; i < oldPhysical.size(); i++)
    {
        name = oldModel->getPhysicalName(oldEntity->dim(), oldPhysical[i]);
            
        const int number = newModel->setPhysicalName(name, newEntity->dim(), 0);
        newEntity->addPhysicalEntity(number);
    }
}







