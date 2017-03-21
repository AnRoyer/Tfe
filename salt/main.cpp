#include <cstdio>
#include <iostream>
#include <fstream>

typedef struct Param{
    bool sliceX;
    bool sliceY;
    bool sliceZ;
    
    int x, y, z;
}Param;

Param readParam(int argc, char** argv);

int main(int argc, char** argv)
{
    Param param = readParam(argc, argv);
    FILE *fp = fopen("fvp.dat", "r");

    int n1=210, n2=676, n3=676;
    int nn = n1*n2*n3;

    float *vp = new float[nn];
    fread(vp, sizeof(float), nn, fp);

    fclose(fp);
    
    std::ofstream fileData("velocity.dat");
    
    if(param.sliceX)
    {
        fileData << n2 << " " << n3 << std::endl;
    }
    else if(param.sliceY)
    {
        fileData << n1 << " " << n3 << std::endl;
    }
    else if(param.sliceZ)
    {
        fileData << n1 << " " << n2 << std::endl;
    }
    
    float h = 0.02;
    if(param.sliceX)
    {
        for(unsigned int i = 0; i < n2; i++)
        {
            fileData << i*h << " ";
        }
        fileData << std::endl;
        
        for(unsigned int i = 0; i < n3; i++)
        {
            fileData << i*h << " ";
        }
        fileData << std::endl;
    }
    else if(param.sliceY)
    {
        for(unsigned int i = 0; i < n1; i++)
        {
            fileData << i*h << " ";
        }
        fileData << std::endl;
        
        for(unsigned int i = 0; i < n3; i++)
        {
            fileData << i*h << " ";
        }
        fileData << std::endl;
    }
    else if(param.sliceZ)
    {
        for(unsigned int i = 0; i < n1; i++)
        {
            fileData << i*h << " ";
        }
        fileData << std::endl;
        
        for(unsigned int i = 0; i < n2; i++)
        {
            fileData << i*h << " ";
        }
        fileData << std::endl;
    }
    
    for(unsigned int i = 0; i < n1; i++)
    {
        if(param.sliceX)
        {
            i = param.x;
        }
        
        for(unsigned int j = 0; j < n2; j++)
        {
            if(param.sliceY)
            {
                j = param.y;
            }
            
            for(unsigned int k = 0; k < 1; k++)
            {
                if(param.sliceZ)
                {
                    k = param.z;
                }
                
                fileData << vp[i*n2*n3 + j*n3 + k] << " ";
                
                if(param.sliceZ)
                {
                    break;
                }
            }
            
            if(param.sliceY)
            {
                break;
            }
        }
        
        if(param.sliceX)
        {
            break;
        }
    }
    
    fileData.close();
    
    //Creation .geo file
    /*
     *   ^x
     *   *
     *   *    * z
     *   *   *
     *   *  *
     *   * *
     *   *****************************************> y
     *
     */
    
    std::ofstream fileGeo("geometry.geo");
    
    fileGeo << "Include \"geometry.dat\";" << std::endl;
    
    if(param.sliceX)
    {
        fileGeo << "Point(1) = {" << param.x*h << ", 0, 0, 1.0};" << std::endl;
        fileGeo << "Point(2) = {" << param.x*h << ", " << n2*h << ", 0, 1.0};" << std::endl;
        fileGeo << "Point(3) = {" << param.x*h << ", " << n2*h << ", " << n3*h << ", 1.0};" << std::endl;
        fileGeo << "Point(4) = {" << param.x*h << ", 0, " << n3*h << ", 1.0};" << std::endl;
    }
    else if(param.sliceY)
    {
        fileGeo << "Point(1) = {0, " << param.y*h << ", 0, 1.0};" << std::endl;
        fileGeo << "Point(2) = {0, " << param.y*h << ", " << n3*h << ", 1.0};" << std::endl;
        fileGeo << "Point(3) = {" << n1*h << ", " << param.y*h << ", " << n3*h << ", 1.0};" << std::endl;
        fileGeo << "Point(4) = {" << n1*h << ", " << param.y*h << ", 0, 1.0};" << std::endl << std::endl;
        
        fileGeo << "Point(5) = {" << n1*h << ", " << param.y*h << ", " << n3*h/2 << ", 1.0};" << std::endl;
        fileGeo << "Point(6) = {0, " << param.y*h << ", " << n3*h/2 << ", 1.0};" << std::endl;
        
        fileGeo << "Line(1) = {1, 6};" << std::endl;
        fileGeo << "Line(2) = {6, 2};" << std::endl;
        fileGeo << "Line(3) = {2, 3};" << std::endl;
        fileGeo << "Line(4) = {3, 5};" << std::endl;
        fileGeo << "Line(5) = {5, 4};" << std::endl;
        fileGeo << "Line(6) = {4, 1};" << std::endl;
        fileGeo << "Line(7) = {5, 6};" << std::endl << std::endl;
        
        fileGeo << "Line Loop(8) = {2, 3, 4, 7};" << std::endl;
        fileGeo << "Plane Surface(9) = {8};" << std::endl;
        fileGeo << "Line Loop(10) = {1, -7, 5, 6};" << std::endl;
        fileGeo << "Plane Surface(11) = {10};" << std::endl  << std::endl;

        fileGeo << "Physical Point(SOURCE) = {5};" << std::endl;
        fileGeo << "Physical Line(TOP) = {5, 4};" << std::endl;
        fileGeo << "Physical Line(BOTTOM) = {1, 2};" << std::endl;
        fileGeo << "Physical Line(BORDER) = {6, 3};" << std::endl;
        fileGeo << "Physical Surface(GROUND) = {9, 11};" << std::endl << std::endl;

        fileGeo << "//To modify!!!!!" << std::endl;
        fileGeo << "Transfinite Line {1, 2, 5, 4} = 10 Using Progression 1;" << std::endl;
        fileGeo << "Transfinite Line {6, 7, 3} = 5 Using Progression 1;" << std::endl << std::endl;

        fileGeo << "Transfinite Surface {9};" << std::endl;
        fileGeo << "Transfinite Surface {11};" << std::endl;
        fileGeo << "Recombine Surface {11, 9};" << std::endl << std::endl;
    }
    else if(param.sliceZ)
    {
        fileGeo << "Point(1) = {0, 0, " << param.z*h << ", 1.0};" << std::endl;
        fileGeo << "Point(2) = {0, " << n2*h << ", " << param.z*h << ", 1.0};" << std::endl;
        fileGeo << "Point(3) = {" << n1*h << ", " << n2*h << ", " << param.z*h << ", 1.0};" << std::endl;
        fileGeo << "Point(4) = {" << n1*h << ", 0, " << param.z*h << ", 1.0};" << std::endl << std::endl;
        
        fileGeo << "Point(5) = {" << n1*h << ", " << n2*h/2 << ", " << param.z*h << ", 1.0};" << std::endl;
        fileGeo << "Point(6) = {0, " << n2*h/2 << ", " << param.z*h << ", 1.0};" << std::endl;
        
        fileGeo << "Line(1) = {1, 6};" << std::endl;
        fileGeo << "Line(2) = {6, 2};" << std::endl;
        fileGeo << "Line(3) = {2, 3};" << std::endl;
        fileGeo << "Line(4) = {3, 5};" << std::endl;
        fileGeo << "Line(5) = {5, 4};" << std::endl;
        fileGeo << "Line(6) = {4, 1};" << std::endl;
        fileGeo << "Line(7) = {5, 6};" << std::endl << std::endl;
        
        fileGeo << "Line Loop(8) = {2, 3, 4, 7};" << std::endl;
        fileGeo << "Plane Surface(9) = {-8};" << std::endl;
        fileGeo << "Line Loop(10) = {1, -7, 5, 6};" << std::endl;
        fileGeo << "Plane Surface(11) = {-10};" << std::endl  << std::endl;
        
        fileGeo << "Physical Point(SOURCE) = {5};" << std::endl;
        fileGeo << "Physical Line(TOP) = {5, 4};" << std::endl;
        fileGeo << "Physical Line(BOTTOM) = {1, 2};" << std::endl;
        fileGeo << "Physical Line(BORDER) = {6, 3};" << std::endl;
        fileGeo << "Physical Surface(GROUND) = {9, 11};" << std::endl << std::endl;
        
        fileGeo << "//To modify!!!!!" << std::endl;
        fileGeo << "Transfinite Line {1, 2, 5, 4} = 10 Using Progression 1;" << std::endl;
        fileGeo << "Transfinite Line {6, 7, 3} = 5 Using Progression 1;" << std::endl << std::endl;
        
        fileGeo << "Transfinite Surface {9};" << std::endl;
        fileGeo << "Transfinite Surface {11};" << std::endl;
        fileGeo << "Recombine Surface {11, 9};" << std::endl << std::endl;
    }
    
    fileGeo.close();

    return 0;
}

Param readParam(int argc, char** argv)
{
    Param param;
    param.sliceX = false;
    param.sliceY = false;
    param.sliceZ = false;
    
    param.x = 0;
    param.y = 0;
    param.z = 0;
    
    for(unsigned int i = 0; i < argc; i++)
    {
        if(strcmp(argv[i], "-sx") == 0)
        {
            param.sliceX = true;
            param.x = atoi(argv[i+1]);
        }
        else if(strcmp(argv[i], "-sy") == 0)
        {
            param.sliceY = true;
            param.y = atoi(argv[i+1]);
        }
        else if(strcmp(argv[i], "-sz") == 0)
        {
            param.sliceZ = true;
            param.z = atoi(argv[i+1]);
        }
    }
    
    return param;
}
