#include <iostream>
#include <cstdlib>
#include <string>

#include "Gmsh.h"
#include "GModel.h"
#include "MElement.h"

#include "metis.h"

#include "graph.h"
#include "topology.h"
#include "io.h"

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        return 0;
    }
    
    std::cout << "#############################################################################" << std::endl;
    std::cout << "#                                                                           #" << std::endl;
    std::cout << "#                         #   #  #####   ####  #   #                        #" << std::endl;
    std::cout << "#                         ## ##  #      #      #   #                        #" << std::endl;
    std::cout << "#                         # # #  ####   #####  #####                        #" << std::endl;
    std::cout << "#                         #   #  #          #  #   #                        #" << std::endl;
    std::cout << "#                         #   #  #####  ####   #   #                        #" << std::endl;
    std::cout << "#                                                                           #" << std::endl;
    std::cout << "#                                                                           #" << std::endl;
    std::cout << "#  ####    ###   #####  #####  ###  #####  ###   ###   #   #  #####  #####  #" << std::endl;
    std::cout << "#  #   #  #   #  #   #    #     #     #     #   #   #  ##  #  #      #   #  #" << std::endl;
    std::cout << "#  ####   #####  #####    #     #     #     #   #   #  # # #  ####   #####  #" << std::endl;
    std::cout << "#  #      #   #  # ##     #     #     #     #   #   #  # # #  #      # ##   #" << std::endl;
    std::cout << "#  #      #   #  #   #    #    ###    #    ###   ###   #  ##  #####  #   #  #" << std::endl;
    std::cout << "#                                                                           #" << std::endl;
    std::cout << "#############################################################################" << std::endl << std::endl;
    
    GmshInitialize(argc, argv);
    GModel *m = new GModel();
    std::cout << "Reading msh file... " << std::flush;
    m->readMSH(argv[1]);
    std::cout << "Done!" << std::endl;

    const int numElements = m->getNumMeshElements();
    const int numVertices = m->getNumMeshVertices();

    int* eptr = new int[numElements+1];
    int* eind = NULL;
    int *metisToGmshIndex = new int[numElements];

    std::cout << "Creating Metis structure... " << std::flush;
    GModelToGraph(m, eptr, &eind, metisToGmshIndex);
    std::cout << "Done!" << std::endl;
  
    std::cout << "Mesh partitioning... " << std::flush;
  
    idx_t objval;
    idx_t *epart = new idx_t[numElements];
    idx_t *npart = new idx_t[numVertices];
    idx_t nparts = atoi(argv[2]);
    idx_t ncommon = 3;
    idx_t options[METIS_NOPTIONS];
    METIS_SetDefaultOptions(options);
    
    const int error = METIS_PartMeshDual((idx_t*)&numElements, (idx_t*)&numVertices, (idx_t*)eptr, (idx_t*)eind, NULL, NULL, &ncommon, &nparts, NULL, options, &objval, epart, npart);
    
    switch(error)
    {
        case METIS_OK:
            std::cout << "Done!" << std::endl;
            break;
        case METIS_ERROR_INPUT:
            std::cout << "Metis error (input)!" << std::endl;
            return 0;
            break;
        case METIS_ERROR_MEMORY:
            std::cout << "Metis error (memory)!" << std::endl;
            return 0;
            break;
        case METIS_ERROR:
            std::cout << "Metis error!" << std::endl;
            return 0;
            break;
        default:
            std::cout << "Error!" << std::endl;
            return 0;
            break;
    }

    for(unsigned int i = 0; i < numElements; i++)
    {
        m->getMeshElementByTag(metisToGmshIndex[i])->setPartition(epart[i]);
    }
    
    std::cout << "Creating new GModel..." << std::flush;
    GModel* global = new GModel();
    std::vector<GModel*> models = createNewModels(m, global, nparts);
    std::cout << "Done!" << std::endl;
    
    std::cout << "Assign mesh vertices to models..." << std::flush;
    for (unsigned int i = 0; i < nparts; i++)
    {
        assignMeshVerticesToModel(models[i]);
    }
    assignMeshVerticesToModel(global);
    std::cout << "Done!" << std::endl;
    
    std::cout << "Creating new elements..." << std::endl;
    createPartitionBoundaries(global, false);
    std::cout << "Done!" << std::endl;
    
    std::cout << "Assign partition boundary to global model..." << std::flush;
    assignPartitionBoundariesToModels(global, models);
    std::cout << "Done!" << std::endl;
    
    std::cout << "Writing partition meshes..." << std::flush;
    writeModels(models);
    std::cout << "Done!" << std::endl;
    
    std::cout << "Writing global mesh..." << std::flush;
    global->writeMSH("global.msh");
    std::cout << "Done!" << std::endl;
    
    std::cout << "Writing .pro file..." << std::flush;
    writeProFile(global, nparts);
    std::cout << "Done!" << std::endl;
    
    freeModels(models, global);
    
    delete[] eptr;
    delete[] eind;
    delete m;
    GmshFinalize();
    
    return 1;
}
