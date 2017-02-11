#include <iostream>
#include <set>
#include <map>
#include <string>

#include "MElement.h"
#include "MElementCut.h"

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
    int maxDim = -1;
    std::vector<GModel*> newModels(nparts, nullptr);
    
    for (unsigned int i = 0; i < nparts; i++)
    {
        newModels[i] = new GModel();
    }
    
    //Loop over regions
    for(GModel::riter it = gModel->firstRegion(); it != gModel->lastRegion(); ++it)
    {
        GRegion *r = *it;
        
        std::vector<GRegion *> newModelHaveRegion(nparts, nullptr);
        
        //Tetrahedra
        for(unsigned int i = 0; i < r->tetrahedra.size(); i++)
        {
            const int partition = r->tetrahedra[i]->getPartition();
            if(!newModelHaveRegion[partition])
            {
                discreteRegion *dr = new discreteRegion(newModels[partition], newModels[partition]->getNumRegions()+1);
                newModels[partition]->add(dr);
                newModelHaveRegion[partition] = dr;
                maxDim = 3;
                addPhysical(newModels[partition], dr, gModel, r, partition, maxDim);
            }
            
            newModelHaveRegion[r->tetrahedra[i]->getPartition()]->tetrahedra.push_back(r->tetrahedra[i]);
        }
        

        //Hexahedra
        for(unsigned int i = 0; i < r->hexahedra.size(); i++)
        {
            const int partition = r->hexahedra[i]->getPartition();
            if(!newModelHaveRegion[partition])
            {
                discreteRegion *dr = new discreteRegion(newModels[partition], newModels[partition]->getNumRegions()+1);
                newModels[partition]->add(dr);
                newModelHaveRegion[partition] = dr;
                maxDim = 3;
                addPhysical(newModels[partition], dr, gModel, r, partition, maxDim);
            }
            
            newModelHaveRegion[r->hexahedra[i]->getPartition()]->hexahedra.push_back(r->hexahedra[i]);
        }
        
        //Prisms
        for(unsigned int i = 0; i < r->prisms.size(); i++)
        {
            const int partition = r->prisms[i]->getPartition();
            if(!newModelHaveRegion[partition])
            {
                discreteRegion *dr = new discreteRegion(newModels[partition], newModels[partition]->getNumRegions()+1);
                newModels[partition]->add(dr);
                newModelHaveRegion[partition] = dr;
                maxDim = 3;
                addPhysical(newModels[partition], dr, gModel, r, partition, maxDim);
            }
            
            newModelHaveRegion[r->prisms[i]->getPartition()]->prisms.push_back(r->prisms[i]);
        }
        
        //Pyramids
        for(unsigned int i = 0; i < r->pyramids.size(); i++)
        {
            const int partition = r->pyramids[i]->getPartition();
            if(!newModelHaveRegion[partition])
            {
                discreteRegion *dr = new discreteRegion(newModels[partition], newModels[partition]->getNumRegions()+1);
                newModels[partition]->add(dr);
                newModelHaveRegion[partition] = dr;
                maxDim = 3;
                addPhysical(newModels[partition], dr, gModel, r, partition, maxDim);
            }
            
            newModelHaveRegion[r->pyramids[i]->getPartition()]->pyramids.push_back(r->pyramids[i]);
        }
        
        //Trihedra
        for(unsigned int i = 0; i < r->trihedra.size(); i++)
        {
            const int partition = r->trihedra[i]->getPartition();
            if(!newModelHaveRegion[partition])
            {
                discreteRegion *dr = new discreteRegion(newModels[partition], newModels[partition]->getNumRegions()+1);
                newModels[partition]->add(dr);
                newModelHaveRegion[partition] = dr;
                maxDim = 3;
                addPhysical(newModels[partition], dr, gModel, r, partition, maxDim);
            }
            
            newModelHaveRegion[r->trihedra[i]->getPartition()]->trihedra.push_back(r->trihedra[i]);
        }
        
        //Polyhedra
        for(unsigned int i = 0; i < r->polyhedra.size(); i++)
        {
            const int partition = r->polyhedra[i]->getPartition();
            if(!newModelHaveRegion[partition])
            {
                discreteRegion *dr = new discreteRegion(newModels[partition], newModels[partition]->getNumRegions()+1);
                newModels[partition]->add(dr);
                newModelHaveRegion[partition] = dr;
                maxDim = 3;
                addPhysical(newModels[partition], dr, gModel, r, partition, maxDim);
            }
            
            newModelHaveRegion[r->polyhedra[i]->getPartition()]->polyhedra.push_back(r->polyhedra[i]);
        }
    }
    
    //Loop over faces
    for(GModel::fiter it = gModel->firstFace(); it != gModel->lastFace(); ++it)
    {
        GFace *f = *it;
        
        std::vector<GFace *> newModelHaveFace(nparts, nullptr);
        
        //Triangles
        for(unsigned int i = 0; i < f->triangles.size(); i++)
        {
            const int partition = f->triangles[i]->getPartition();
            if(!newModelHaveFace[partition])
            {
                discreteFace *df = new discreteFace(newModels[partition], newModels[partition]->getNumFaces()+1);
                newModels[partition]->add(df);
                newModelHaveFace[partition] = df;
                if(maxDim == -1)
                {
                    maxDim = 2;
                }
                addPhysical(newModels[partition], df, gModel, f, partition, maxDim);
            }
            
            newModelHaveFace[f->triangles[i]->getPartition()]->triangles.push_back(f->triangles[i]);
        }
        
        //Quadrangles
        for(unsigned int i = 0; i < f->quadrangles.size(); i++)
        {
            const int partition = f->quadrangles[i]->getPartition();
            if(!newModelHaveFace[partition])
            {
                discreteFace *df = new discreteFace(newModels[partition], newModels[partition]->getNumFaces()+1);
                newModels[partition]->add(df);
                newModelHaveFace[partition] = df;
                if(maxDim == -1)
                {
                    maxDim = 2;
                }
                addPhysical(newModels[partition], df, gModel, f, partition, maxDim);
            }
            
            newModelHaveFace[f->quadrangles[i]->getPartition()]->quadrangles.push_back(f->quadrangles[i]);
        }
        
        //Polygons
        for(unsigned int i = 0; i < f->polygons.size(); i++)
        {
            const int partition = f->polygons[i]->getPartition();
            if(!newModelHaveFace[partition])
            {
                discreteFace *df = new discreteFace(newModels[partition], newModels[partition]->getNumFaces()+1);
                newModels[partition]->add(df);
                newModelHaveFace[partition] = df;
                if(maxDim == -1)
                {
                    maxDim = 2;
                }
                addPhysical(newModels[partition], df, gModel, f, partition, maxDim);
            }
            
            newModelHaveFace[f->polygons[i]->getPartition()]->polygons.push_back(f->polygons[i]);
        }
    }
    
    //Loop over edges
    for(GModel::eiter it = gModel->firstEdge(); it != gModel->lastEdge(); ++it)
    {
        GEdge *e = *it;
        
        std::vector<GEdge *> newModelHaveEdge(nparts, nullptr);
        
        //Lines
        for(unsigned int i = 0; i < e->lines.size(); i++)
        {
            const int partition = e->lines[i]->getPartition();
            if(!newModelHaveEdge[partition])
            {
                discreteEdge *de = new discreteEdge(newModels[partition], newModels[partition]->getNumEdges()+1, nullptr, nullptr);
                newModels[partition]->add(de);
                newModelHaveEdge[partition] = de;
                if(maxDim == -1)
                {
                    maxDim = 1;
                }
                addPhysical(newModels[partition], de, gModel, e, partition, maxDim);
            }
            
            newModelHaveEdge[e->lines[i]->getPartition()]->lines.push_back(e->lines[i]);
        }
    }
    
    //Loop over vertices
    for(GModel::viter it = gModel->firstVertex(); it != gModel->lastVertex(); ++it)
    {
        GVertex *v = *it;
        
        std::vector<GVertex *> newModelHaveVertex(nparts, nullptr);
        
        for(unsigned int i = 0; i < v->points.size(); i++)
        {
            const int partition = v->points[i]->getPartition();
            if(!newModelHaveVertex[partition])
            {
                discreteVertex *dv = new discreteVertex(newModels[partition], newModels[partition]->getNumVertices()+1);
                newModels[partition]->add(dv);
                newModelHaveVertex[partition] = dv;
                if(maxDim == -1)
                {
                    maxDim = 0;
                }
                addPhysical(newModels[partition], dv, gModel, v, partition, maxDim);
            }
            
            newModelHaveVertex[v->points[i]->getPartition()]->points.push_back(v->points[i]);
        }
    }
    
    return newModels;
}

void assignMeshVerticesToModel(GModel *gModel)
{
    std::unordered_map<MVertex*, GEntity*> vertexToEntity;
    
    //Loop over regions
    for(GModel::riter it = gModel->firstRegion(); it != gModel->lastRegion(); ++it)
    {
        GRegion *r = *it;
        
        fillVertexToEntity(vertexToEntity, r, r->tetrahedra.begin(), r->tetrahedra.end());
        fillVertexToEntity(vertexToEntity, r, r->hexahedra.begin(), r->hexahedra.end());
        fillVertexToEntity(vertexToEntity, r, r->prisms.begin(), r->prisms.end());
        fillVertexToEntity(vertexToEntity, r, r->pyramids.begin(), r->pyramids.end());
        fillVertexToEntity(vertexToEntity, r, r->trihedra.begin(), r->trihedra.end());
        fillVertexToEntity(vertexToEntity, r, r->polyhedra.begin(), r->polyhedra.end());
    }
    
    //Loop over faces
    for(GModel::fiter it = gModel->firstFace(); it != gModel->lastFace(); ++it)
    {
        GFace *f = *it;
        
        fillVertexToEntity(vertexToEntity, f, f->triangles.begin(), f->triangles.end());
        fillVertexToEntity(vertexToEntity, f, f->quadrangles.begin(), f->quadrangles.end());
        fillVertexToEntity(vertexToEntity, f, f->polygons.begin(), f->polygons.end());
    }
    
    //Loop over edges
    for(GModel::eiter it = gModel->firstEdge(); it != gModel->lastEdge(); ++it)
    {
        GEdge *e = *it;
        
        fillVertexToEntity(vertexToEntity, e, e->lines.begin(), e->lines.end());
    }
    
    //Loop over vertices
    for(GModel::viter it = gModel->firstVertex(); it != gModel->lastVertex(); ++it)
    {
        GVertex *v = *it;
        
        fillVertexToEntity(vertexToEntity, v, v->points.begin(), v->points.end());
    }
    
    //Fill the entities
    for(std::unordered_map<MVertex*, GEntity*>::iterator it = vertexToEntity.begin(); it != vertexToEntity.end(); ++it)
    {
        it->second->addMeshVertex(it->first);
    }
}

template <class ITERATOR>
void fillVertexToEntity(std::unordered_map<MVertex*, GEntity*> &vertexToEntity, GEntity* entity, ITERATOR it_beg, ITERATOR it_end)
{
    for(ITERATOR it = it_beg; it != it_end; ++it)
    {
        for(unsigned int j = 0; j < (*it)->getNumVertices(); j++)
        {
            if(vertexToEntity.count((*it)->getVertex(j)) > 0)
            {
                if(vertexToEntity[(*it)->getVertex(j)]->dim() > entity->dim())
                {
                    vertexToEntity[(*it)->getVertex(j)] = entity;
                }
            }
            else
            {
                vertexToEntity[(*it)->getVertex(j)] = entity;
            }
        }
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
                models[static_cast<partitionFace*>(f)->_partitions[j]]->add(f);
                models[static_cast<partitionFace*>(f)->_partitions[j]]->setPhysicalName(name, f->dim(), maxNumPhysical);
                gModel->setPhysicalName(name, f->dim(), maxNumPhysical);
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
                models[static_cast<partitionEdge*>(e)->_partitions[j]]->add(e);
                models[static_cast<partitionEdge*>(e)->_partitions[j]]->setPhysicalName(name, e->dim(), maxNumPhysical);
                gModel->setPhysicalName(name, e->dim(), maxNumPhysical);
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
                models[static_cast<partitionVertex*>(v)->_partitions[j]]->add(v);
                models[static_cast<partitionVertex*>(v)->_partitions[j]]->setPhysicalName(name, v->dim(), maxNumPhysical);
                gModel->setPhysicalName(name, v->dim(), maxNumPhysical);
            }
        }
    }
}

void freeModels(std::vector<GModel*> &models)
{
    for(unsigned int i = 0; i < models.size(); i++)
    {
        std::vector<GEntity*> entities;
        models[i]->getEntities(entities);
        
        for(unsigned int j = 0; j < entities.size(); j++)
        {            
            switch(entities[j]->dim())
            {
                case 3:
                    models[i]->remove(static_cast<GRegion*>(entities[j]));
                    break;
                case 2:
                    models[i]->remove(static_cast<GFace*>(entities[j]));
                    break;
                case 1:
                    models[i]->remove(static_cast<GEdge*>(entities[j]));
                    break;
                case 0:
                    models[i]->remove(static_cast<GVertex*>(entities[j]));
                    break;
                default:
                    break;
            }
        }
    }
}

void addPhysical(GModel *newModel, GEntity *newEntity, GModel *oldModel, GEntity *oldEntity, int partition, int maxDim)
{
    std::vector<int> oldPhysical = oldEntity->getPhysicalEntities();
    std::string name;
    
    if(maxDim == newEntity->dim())
    {
        name = "_omega{";
        name += std::to_string(partition);
        name += "}";
    
        const int number = oldModel->setPhysicalName(name, oldEntity->dim(), oldModel->getMaxPhysicalNumber(-1)+1);
        oldEntity->addPhysicalEntity(number);
        newModel->setPhysicalName(name, newEntity->dim(), number);
        newEntity->addPhysicalEntity(number);
    }
    
    for(unsigned int i = 0; i < oldPhysical.size(); i++)
    {
        name = oldModel->getPhysicalName(oldEntity->dim(), oldPhysical[i]);
        
        if(name[0] != '_')
        {
            newModel->setPhysicalName(name, newEntity->dim(), oldPhysical[i]);
            newEntity->addPhysicalEntity(oldPhysical[i]);
        }
    }
}







