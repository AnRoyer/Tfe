#ifndef TOPOLOGY_MPI_INCLUDED
#define TOPOLOGY_MPI_INCLUDED

#include <unordered_map>

#include "GModel.h"

#include "partitionFace.h"
#include "partitionEdge.h"
#include "partitionVertex.h"

#include "MFaceHash.h"
#include "MEdgeHash.h"

namespace PAR {
    std::vector<GModel*> createNewModels(GModel *gModel, GModel *global, int nparts, std::vector<int> myPart);
    int getPos(std::vector<int> myPart, int partition);
    void addPhysical(GModel *newModel, GEntity *newEntity, GModel *oldModel, GEntity *oldEntity, int partition, int maxDim);
    void assignPartitionBoundariesToModels(GModel *gModel, std::vector<GModel*> &models, std::vector<int> myPart);
    int createPartitionBoundaries(GModel *model, bool createGhostCells);
}

#endif //TOPOLOGY_MPI_INCLUDED
