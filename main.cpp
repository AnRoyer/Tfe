#include <iostream>
#include <cstdlib>
#include <string>

#include "Gmsh.h"
#include "GModel.h"
#include "MElement.h"

#include "metis.h"

#include "graph.h"
#include "topology.h"

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        return 0;
    }
    
    std::cout << "*****************************************************************************" << std::endl;
    std::cout << "*                                                                           *" << std::endl;
    std::cout << "*                         *   *  *****   ****  *   *                        *" << std::endl;
    std::cout << "*                         ** **  *      *      *   *                        *" << std::endl;
    std::cout << "*                         * * *  ****   *****  *****                        *" << std::endl;
    std::cout << "*                         *   *  *          *  *   *                        *" << std::endl;
    std::cout << "*                         *   *  *****  ****   *   *                        *" << std::endl;
    std::cout << "*                                                                           *" << std::endl;
    std::cout << "*                                                                           *" << std::endl;
    std::cout << "*  ****    ***   *****  *****  ***  *****  ***   ***   *   *  *****  *****  *" << std::endl;
    std::cout << "*  *   *  *   *  *   *    *     *     *     *   *   *  **  *  *      *   *  *" << std::endl;
    std::cout << "*  ****   *****  *****    *     *     *     *   *   *  * * *  ****   *****  *" << std::endl;
    std::cout << "*  *      *   *  * **     *     *     *     *   *   *  * * *  *      * **   *" << std::endl;
    std::cout << "*  *      *   *  *   *    *    ***    *    ***   ***   *  **  *****  *   *  *" << std::endl;
    std::cout << "*                                                                           *" << std::endl;
    std::cout << "*****************************************************************************" << std::endl << std::endl;
    
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
    idx_t options[METIS_NOPTIONS];
    METIS_SetDefaultOptions(options);

    const int error = METIS_PartMeshNodal((idx_t*)&numElements, (idx_t*)&numVertices, (idx_t*)eptr, (idx_t*)eind, NULL, NULL, &nparts, NULL, options, &objval, epart, npart);

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
        m->getMeshElementByTag(metisToGmshIndex[i])->setPartition(epart[i]+1); //epart has the first partition at zero and we want to start with one
    }
    
    std::cout << "Creating new GModel..." << std::flush;
    std::vector<GModel*> models = createNewModels(m, nparts);
    std::cout << "Done!" << std::endl;
    
    std::cout << "Creating new elements..." << std::endl;
    createPartitionBoundaries(m, false);
    std::cout << "Done!" << std::endl;
    
    std::cout << "Assign mesh vertex to models..." << std::flush;
    for (unsigned int i = 0; i < nparts; i++)
    {
        assignMeshVerticesToModel(models[i]);
    }
    std::cout << "Done!" << std::endl;
    
    std::cout << "Assign partition boundary to models..." << std::flush;
    assignPartitionBoundariesToModels(m, models);
    std::cout << "Done!" << std::endl;
    
    for(unsigned int i = 0; i < models.size(); i++)
    {
        std::string str = "Partition_";
        str += std::to_string(i+1);
        str += ".msh";
        
        models[i]->writeMSH(str.c_str());
    }
    std::cout << "Partition meshes writed." << std::endl;
    
    m->writeMSH("output.msh");
    std::cout << "Global mesh writed." << std::endl;
  
    freeModels(&models);
    delete[] eptr;
    delete[] eind;
    delete m;
    GmshFinalize();
    
    return 1;
}
