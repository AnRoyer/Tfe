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
    std::multimap<int, int> neighbour;
    std::map<int, std::vector<std::vector<int> > > border;
    
    for(unsigned int i = 0; i < models.size(); i++)
    {
        std::vector<GEntity*> entities;
        models[i]->getEntities(entities);
        
        for(unsigned int j = 0; j < entities.size(); j++)
        {
            if(entities[j]->geomType() == GEntity::PartitionSurface)
            {
                border[i+1].push_back(static_cast<partitionFace*>(entities[j])->_partitions);
                for(unsigned int k = 0; k < static_cast<partitionFace*>(entities[j])->_partitions.size(); k++)
                {
                    neighbour.insert(std::pair<int, int>(i+1, static_cast<partitionFace*>(entities[j])->_partitions[k]));
                }
            }
            else if(entities[j]->geomType() == GEntity::PartitionCurve)
            {
                border[i+1].push_back(static_cast<partitionEdge*>(entities[j])->_partitions);
                for(unsigned int k = 0; k < static_cast<partitionEdge*>(entities[j])->_partitions.size(); k++)
                {
                    neighbour.insert(std::pair<int, int>(i+1, static_cast<partitionEdge*>(entities[j])->_partitions[k]));
                }
            }
            else if(entities[j]->geomType() == GEntity::PartitionVertex)
            {
                border[i+1].push_back(static_cast<partitionVertex*>(entities[j])->_partitions);
                for(unsigned int k = 0; k < static_cast<partitionVertex*>(entities[j])->_partitions.size(); k++)
                {
                    neighbour.insert(std::pair<int, int>(i+1, static_cast<partitionVertex*>(entities[j])->_partitions[k]));
                }
            }
        }
    }
    
    std::ofstream file("topology.pro", std::ofstream::trunc);
    
    file << "//Neighbour partitions (omega)" << std::endl;
    for(unsigned int i = 0; i < models.size(); i++)
    {
        std::unordered_set<int> neighbourSet;
        
        file << "D[";
        file << i+1;
        file << "] = {";
        
        std::pair<std::multimap<int,int>::iterator, std::multimap<int,int>::iterator> ret = neighbour.equal_range(i+1);
        for(std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it)
        {
            if(neighbourSet.count(it->second) == 0)
            {
                if(it != ret.first)
                {
                    file << ", ";
                }
                
                file << it->second;
                neighbourSet.insert(it->second);
            }
        }
        
        file << "};" << std::endl;
    }
    
    file << "\n//Border of partition (sigma)" << std::endl;
    for(std::map<int, std::vector<std::vector<int> > >::iterator it = border.begin(); it != border.end(); ++it)
    {
        file << "S[";
        file << it->first;
        file << "] = {";
            
        //Loop over borders
        for(std::vector<std::vector<int> >::iterator itVect = it->second.begin(); itVect != it->second.end(); ++itVect)
        {
            if(itVect != it->second.begin())
            {
                file << ", ";
            }
            
            file << "{";
            for(unsigned int i = 0; i < itVect->size(); i++)
            {
                if(i > 0)
                {
                    file << ", ";
                }
                
                file << (*itVect)[i];
            }
            file << "}";
        }
        
        file << "};" << std::endl;
    }
}
