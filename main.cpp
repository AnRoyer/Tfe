#include <iostream>

#include "Gmsh.h"
#include "GModel.h"
#include "MElement.h"

#include "metis.h"

#include "graph.h"

int main(int argc, char **argv)
{
  GmshInitialize(argc, argv);
  GModel *m = new GModel();
  m->readMSH(argv[1]);

  const int numElements = m->getNumMeshElements();
  const int numVertices = m->getNumMeshVertices();

  int* eptr = new int[numElements+1];
  int* eind = NULL;
  int *metisToGmshIndex = new int[numElements];

  std::cout << "Creating Metis structure... ";

  GModelToGraph(m, eptr, &eind, metisToGmshIndex);
  
  std::cout << "\nMesh partitioning... ";
  
  idx_t objval;
  idx_t *epart = new idx_t[numElements];
  idx_t *npart = new idx_t[numVertices];
  idx_t nparts = argv[2];
  idx_t options[METIS_NOPTIONS];
  METIS_SetDefaultOptions(options); 

  const int error = METIS_PartMeshNodal((idx_t*)&numElements, (idx_t*)&numVertices, (idx_t*)eptr, (idx_t*)eind, NULL, NULL, &nparts, NULL, options, &objval, epart, npart);

  switch(error)
  {
  case METIS_OK:
    std::cout << "Partitioning done!" << std::endl;
    break;
  case METIS_ERROR_INPUT:
    std::cout << "Metis error (input)!" << std::endl;
    break;
  case METIS_ERROR_MEMORY:
    std::cout << "Metis error (memory)!" << std::endl;
    break;
  case METIS_ERROR:
    std::cout << "Metis error!" << std::endl;
    break;
  default:
    std::cout << "Error!" << std::endl;
    break;
  }

  for(unsigned int i = 0; i < numElements; i++)
  {
    m->getMeshElementByTag(metisToGmshIndex[i])->setPartition(epart[i]);
  }

  m->writeMSH("output.msh");
  
  delete[] eptr;
  delete[] eind;
  delete m;
  GmshFinalize();
}
