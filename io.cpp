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
        std::string str = "mesh_";
        str += std::to_string(i);
        str += ".msh";
        
        models[i]->writeMSH(str.c_str());
    }
}

void writeProFile(GModel* m, const int npart)
{
    std::ofstream file("partition.pro", std::ofstream::trunc);
    
    file << "N_DOM = " << npart << ";" << std::endl << std::endl;
    
    //-----------Group-----------
    file << "Group{" << std::endl;
    
    //Omega
    std::unordered_map<int, std::vector<int> > listOfOmega;
    for(GModel::piter it = m->firstPhysicalName(); it != m->lastPhysicalName(); ++it)
    {
        std::string name = it->second;
        
        if(name[0] == '_' && name[1] == 'o')
        {
            std::vector<int> num = getNumFromString(name);
            
            std::vector<int> vec = listOfOmega[num[0]];
            vec.push_back(it->first.second);
            listOfOmega[num[0]] = vec;
        }
    }
    for(std::unordered_map<int, std::vector<int> >::iterator it = listOfOmega.begin(); it != listOfOmega.end(); ++it)
    {
        std::vector<int> vec = it->second;
        file << "\tOmega_" << it->first << " = Region[{";
        
        for(unsigned int i = 0; i < vec.size(); i++)
        {
            if(i != 0)
            {
                file << ", ";
            }
            file << vec[i];
        }
        file << "}];" << std::endl;
    }
    file << std::endl;
    
    //Sigma
    std::unordered_map<int, std::vector<int> > listOfSigma;
    for(GModel::piter it = m->firstPhysicalName(); it != m->lastPhysicalName(); ++it)
    {
        std::string name = it->second;
        
        if(name[0] == '_' && name[1] == 's')
        {
            std::vector<int> num = getNumFromString(name);
            
            for(unsigned int i = 0; i < num.size(); i++)
            {
                std::vector<int> vec = listOfSigma[num[i]];
                vec.push_back(it->first.second);
                listOfSigma[num[i]] = vec;
            }
        }
    }
    file << std::endl;
    //List of Sigma between two partitions (Sigma_x_y)
    for(std::unordered_map<int, std::vector<int> >::iterator it = listOfSigma.begin(); it != listOfSigma.end(); ++it)
    {
        for(std::unordered_map<int, std::vector<int> >::iterator it2 = listOfSigma.begin(); it2 != listOfSigma.end(); ++it2)
        {
            if(it != it2)
            {
                std::vector<int> vec1 = it->second;
                std::vector<int> vec2 = it2->second;
                std::vector<int>* vecCommun =  new std::vector<int>;
            
                if(communPhysicals(vec1, vec2, vecCommun))
                {
                    file << "\tSigma_" << it->first << "_" << it2->first << " = Region[{";
                    for(unsigned int i = 0; i < vecCommun->size(); i++)
                    {
                        if(i != 0)
                        {
                            file << ", ";
                        }
                        file << (*vecCommun)[i];
                    }
                    file << "}];" << std::endl;
                }
                delete vecCommun;
            }
        }
    }
    file << std::endl;
    //List of Sigma of a partiton (Sigma_x)
    for(std::unordered_map<int, std::vector<int> >::iterator it = listOfSigma.begin(); it != listOfSigma.end(); ++it)
    {
        std::vector<int> vec = it->second;
        file << "\tSigma_" << it->first << " = Region[{";
        
        for(unsigned int i = 0; i < vec.size(); i++)
        {
            if(i != 0)
            {
                file << ", ";
            }
            file << vec[i];
        }
        file << "}];" << std::endl;
    }
    
    //To work well
    //**********************************************
    file << "//To work well" << std::endl;
    for(std::unordered_map<int, std::vector<int> >::iterator it = listOfSigma.begin(); it != listOfSigma.end(); ++it)
    {
        std::vector<int> vec = it->second;
        file << "\tSigma_" << it->first << "_0 = Region[{" << vec[0] << "}];" << std::endl;
        file << "\tSigma_" << it->first << "_1 = Region[{" << vec[1] << "}];" << std::endl;

    }
    
    file << "}" << std::endl << std::endl << std::endl;
    //**********************************************
    
    
    //-----------Function-----------
    file << "Function {" << std::endl;
    file << "\tListOfSubdomains = {} ; // the domains that I'm in charge of" << std::endl;
    file << "\tListOfFields = {}; // my fields" << std::endl;
    file << "\tListOfConnectedFields = {}; // my neighbors" << std::endl << std::endl;
    
    
    file << "\tFor idom In {0:N_DOM-1}" << std::endl;
    file << "\t\tIf (idom % MPI_Size == MPI_Rank)" << std::endl;
    file << "\t\t\tListOfSubdomains += idom; " << std::endl << std::endl;
    file << "\t\t\tmyFieldLeft  = {(2*(idom + N_DOM) + (0-1)) % (2*N_DOM)};" << std::endl;
    file << "\t\t\tmyFieldRight = {(2*(idom + N_DOM) + (1-1)) % (2*N_DOM)};" << std::endl;
    file << "\t\t\tListOfFields += {myFieldLeft(), myFieldRight()};" << std::endl;
    file << "\t\t\tIf(ANALYSIS == 0)" << std::endl;
    file << "\t\t\t\tg_in~{idom}~{0}[Sigma~{idom}~{0}] = ComplexScalarField[XYZ[]];" << std::endl;
    file << "\t\t\t\tg_in~{idom}~{1}[Sigma~{idom}~{1}] = ComplexScalarField[XYZ[]];" << std::endl;
    file << "\t\t\tEndIf" << std::endl;
    file << "\t\t\tIf(ANALYSIS == 1)" << std::endl;
    file << "\t\t\t\tg_in~{idom}~{0}[Sigma~{idom}~{0}] = ComplexVectorField[XYZ[]];" << std::endl;
    file << "\t\t\t\tg_in~{idom}~{1}[Sigma~{idom}~{1}] = ComplexVectorField[XYZ[]];" << std::endl;
    file << "\t\t\tEndIf" << std::endl;
    file << "\t\tEndIf" << std::endl;
    file << "\tEndFor" << std::endl;
    
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

bool communPhysicals(const std::vector<int> vec1, const std::vector<int> vec2, std::vector<int>* vecCommun)
{
    for(unsigned int i = 0; i < vec1.size(); i++)
    {
        for(unsigned int j = 0; j < vec2.size(); j++)
        {
            if(vec1[i] == vec2[j])
            {
                vecCommun->push_back(vec1[i]);
            }
        }
    }
    
    if(vecCommun->size() > 0)
    {
        return true;
    }
    return false;
}


