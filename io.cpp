#include <map>
#include <vector>
#include <fstream>
#include <unordered_set>

#include "GModel.h"
#include "GEntity.h"

#include "partitionFace.h"
#include "partitionEdge.h"
#include "partitionVertex.h"

#include "io.h"

void writeModels(std::vector<GModel*> &models)
{
    for(unsigned int i = 0; i < models.size(); i++)
    {
        std::string str = "Partition_";
        str += std::to_string(i+1);
        str += ".msh";
        
        models[i]->writeMSH(str.c_str());
    }
}

void writeProFile(std::vector<GModel*> &models)
{
    
}
