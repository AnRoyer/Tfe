#ifndef TOPOLOGY_INCLUDED
#define TOPOLOGY_INCLUDED

#include "Gmsh.h"
#include "GModel.h"

std::vector<GModel *> *splitMesh(GModel* gModel, int nparts);
void assignGVertex(std::vector<GModel *> *models, GModel* gModel);
void assignGEdge(std::vector<GModel *> *models, GModel* gModel, int nparts);

void createNewGVertex(std::vector<GModel *> *models, GModel* gModel, std::map<int, std::vector<int> > borderNodes);
void createNewGEdge(std::vector<GModel *> *models, GModel* gModel, std::map<int, std::vector<int> > borderNodes);

std::map<int, std::vector<int> > getBorderNodes(GModel* gModel);
void createEdge(std::vector<GModel *> *models, GModel* gModel, int num0, int num1, std::vector<int> partitions);

void assignOmegaTopology(GModel *gModel);

#endif //TOPOLOGY_INCLUDED
