#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <fstream>

#include "Gmsh.h"
#include "GModel.h"
#include "MElement.h"

#ifdef PARALLEL
    #include <mpi.h>
    #include "parmetis.h"
#else
    #include "metis.h"
#endif

#include "graph.h"
#include "topology.h"
#include "io.h"

int main(int argc, char **argv)
{
#ifdef PARALLEL
        int nbproc, myrank;
        MPI_Init(&argc,&argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
        MPI_Comm_size(MPI_COMM_WORLD, &nbproc);
#endif
    
#ifdef PARALLEL
    if(myrank == 0)
    {
#endif
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
    
    
    if(argc < 2)
    {
        std::cout << "Arguments missing! Syntaxe :" << std::endl;
        std::cout << argv[0] << " \"mesh.msh\" \"nbrPartitions\"" << std::endl;
        return 0;
    }
    
    if(atoi(argv[2]) < 2)
    {
        std::cout << "The number of partition must be greater than two!" << std::endl;
        return 0;
    }
    
    float temps;
    clock_t t1, t2;
    t1 = clock();
#ifdef PARALLEL
    }
#endif
    
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
  
#ifdef PARALLEL
    std::cout << "Mesh partitioning... " << std::flush;
    
    idx_t *elmdist = new idx_t[nbproc+1];
    elmdist[0] = 0;
    for(unsigned int i = 1; i < nbproc; i++)
    {
        elmdist[i] = elmdist[i-1] + numElements/nbproc;
    }
    elmdist[nbproc] = numElements;
    
    idx_t objval;
    idx_t *epart = new idx_t[numElements];
    idx_t *npart = new idx_t[numVertices];
    idx_t nparts = atoi(argv[2]);
    idx_t ncommon = 3;
    idx_t options[METIS_NOPTIONS];
    METIS_SetDefaultOptions(options);
    
    const int error = ParMETIS_V3_PartMeshKway((idx_t*) elmdist, (idx_t*)eptr, (idx_t*)eind, NULL, NULL, 0, 0, &ncommon, &nparts, NULL, NULL, options, &objval, epart, MPI_COMM_WORLD);
    
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
#else
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
#endif

    for(unsigned int i = 0; i < numElements; i++)
    {
        m->getMeshElementByTag(metisToGmshIndex[i])->setPartition(epart[i]);
    }
    
    std::cout << "Creating new GModel... " << std::flush;
    GModel* global = new GModel();
    std::vector<GModel*> models = createNewModels(m, global, nparts);
    std::cout << "Done!" << std::endl;
    
    std::cout << "Assign mesh vertices to models... " << std::flush;
    for (unsigned int i = 0; i < nparts; i++)
    {
        assignMeshVerticesToModel(models[i]);
    }
    assignMeshVerticesToModel(global);
    std::cout << "Done!" << std::endl;
    
    std::cout << "Creating new elements... " << std::endl;
    createPartitionBoundaries(global, false);
    std::cout << "Done!" << std::endl;
    
    std::cout << "Assign partition boundary to global model... " << std::flush;
    assignPartitionBoundariesToModels(global, models);
    std::cout << "Done!" << std::endl;
    
    std::cout << "Writing partition meshes... " << std::flush;
    writeModels(models);
    std::cout << "Done!" << std::endl;
    
    std::cout << "Writing global mesh... " << std::flush;
    global->writeMSH("global.msh");
    std::cout << "Done!" << std::endl;
    
    std::cout << "Writing .pro file... " << std::flush;
    writeProFile(global, nparts);
    std::cout << "Done!" << std::endl;
    
    freeModels(models, global);
    
    delete[] eptr;
    delete[] eind;
    delete m;
    GmshFinalize();
    
#ifdef PARALLEL
    if(myrank == 0)
    {
#endif
    t2 = clock();
    temps = (float)(t2-t1)/CLOCKS_PER_SEC;
    
    std::cout << "-> Partition done in " << temps << "seconds" << std::endl;
    
#ifdef PARALLEL
    }
    MPI_Finalize();
#endif
    
    return 1;
}
