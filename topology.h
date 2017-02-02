#ifndef TOPOLOGY_INCLUDED
#define TOPOLOGY_INCLUDED

#include <unordered_map>

#include "GModel.h"

#include "partitionFace.h"
#include "partitionEdge.h"
#include "partitionVertex.h"

int createPartitionBoundaries(GModel *model, bool createGhostCells);

template <class ITERATOR>
void fillit_(std::multimap<MFace, MElement*, Less_Face> &faceToElement, ITERATOR it_beg, ITERATOR it_end);
template <class ITERATOR>
void fillit_(std::multimap<MEdge, MElement*, Less_Edge> &edgeToElement, ITERATOR it_beg, ITERATOR it_end);
template <class ITERATOR>
void fillit_(std::multimap<MVertex*, MElement*> &vertexToElement, ITERATOR it_beg, ITERATOR it_end);

void assignPartitionBoundary(GModel *model, MFace &me, std::set<partitionFace*, Less_partitionFace> &pfaces, std::vector<MElement*> &v);
void assignPartitionBoundary(GModel *model, MEdge &me, std::set<partitionEdge*, Less_partitionEdge> &pedges, std::vector<MElement*> &v, std::set<partitionFace*, Less_partitionFace> &pfaces);
void assignPartitionBoundary(GModel *model, MVertex *ve, std::set<partitionVertex*, Less_partitionVertex> &pvertices, std::vector<MElement*> &v, std::set<partitionEdge*, Less_partitionEdge> &pedges, std::set<partitionFace*, Less_partitionFace> &pfaces);

std::vector<GModel*> createNewModels(GModel *gModel, int nparts);

void assignMeshVerticesToModel(GModel *gModel);
template <class ITERATOR>
void fillVertexToEntity(std::unordered_map<MVertex*, GEntity*> &vertexToEntity, GEntity* entity, ITERATOR it_beg, ITERATOR it_end);

void assignPartitionBoundariesToModels(GModel *gModel, std::vector<GModel*> &models);

void freeModels(std::vector<GModel*> *models);

void addPhysical(GModel *newModel, GEntity *newEntity, GModel *oldModel, GEntity *oldEntity, int partition, int maxDim);


#endif //TOPOLOGY_INCLUDED
