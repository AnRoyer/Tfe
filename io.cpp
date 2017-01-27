#include <map>
#include <unordered_map>
#include <vector>
#include <fstream>

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

void writeProFile(GModel* m)
{
    std::ofstream file("partition.pro", std::ofstream::trunc);
    
    
    //Group
    file << "Group{" << std::endl;
    
    //Omega
    std::unordered_map<int, bool> omegaAlreadyCreated;
    for(GModel::piter it = m->firstPhysicalName(); it != m->lastPhysicalName(); ++it)
    {
        std::string name = it->second;
        
        if(name[0] == '_' && name[1] == 'o')
        {
            std::vector<int> num = getNumFromString(name);
            
            if(omegaAlreadyCreated.count(num[0]) > 0)
            {
                file << "\tOmega_" << num[0] << " += Region[" << it->first.second << "];" << std::endl;
            }
            else
            {
                omegaAlreadyCreated.insert(std::pair<int, bool>(num[0], true));
                file << "\tOmega_" << num[0] << " = Region[" << it->first.second << "];" << std::endl;
            }
        }
    }
    file << std::endl;
    
    //Sigma
    std::map<int, std::vector<int> > sigmas;
    for(GModel::piter it = m->firstPhysicalName(); it != m->lastPhysicalName(); ++it)
    {
        std::string name = it->second;
        
        if(name[0] == '_' && name[1] == 's')
        {
            std::vector<int> num = getNumFromString(name);
            
            for(unsigned int i = 0; i < num.size(); i++)
            {
                //std::cout << num[i] << std::endl;
                std::vector<int> physicals = sigmas[num[i]];
                physicals.push_back(it->first.second);
                sigmas[num[i]] = physicals;
            }
        }
    }
    for(std::map<int, std::vector<int> >::iterator it = sigmas.begin(); it != sigmas.end(); ++it)
    {
        std::vector<int> physicals = it->second;
        file << "\tSigma_" << it->first << " = Region[{";
        
        for(unsigned int i = 0; i < physicals.size(); i++)
        {
            if(i != 0)
            {
                file << ", ";
            }
            file << physicals[i];
        }
        file << "}];" << std::endl;
    }
    
    file << "}" << std::endl;
    
    file.close();
}

std::vector<int> getNumFromString(std::string name)
{
    std::vector<int> num;
    std::string currentNum;
    
    for(unsigned int i = 0; i < name.size(); i++)
    {
        if(name[i] == '0' || name[i] == '1' || name[i] == '2'|| name[i] == '3'|| name[i] == '4'|| name[i] == '5'|| name[i] == '6'|| name[i] == '7'|| name[i] == '8'|| name[i] == '9')
        {
            currentNum += name[i];
        }
        
        if(name[i] == ',' || name[i] == '}')
        {
            num.push_back(stoi(currentNum));
            currentNum.clear();
        }
    }
    
    return num;
}


