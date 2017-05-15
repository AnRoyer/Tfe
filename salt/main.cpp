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
    //z y x
    int nn = n1*n2*n3;

    float *vp = new float[nn];
    fread(vp, sizeof(float), nn, fp);

    fclose(fp);
    
    std::ofstream fileData("velocity.dat");
    
    float h = 0.02;
    if(param.sliceX)
    {
        //y z
        fileData << n2 << " " << n1 << std::endl;
        //list x
        for(unsigned int i = 0; i < n2; i++)
        {
            fileData << i*h << " ";
        }
        fileData << std::endl;
        
        //list y
        for(int i = n1-1; i >= 0; i--)
        {
            fileData << -i*h << " ";
        }
        fileData << std::endl;
        
        for(unsigned int i = 0; i < n1; i++)
        {
            for(unsigned int j = 0; j < n2; j++)
            {
                fileData << vp[param.x*n1*n2 + j*n1 + (n1-1-i)] << " ";
            }
        }
    }
    else
    {
        //x y z
        fileData << n3 << " " << n2 << " " << n1 << std::endl;
        //list x
        for(unsigned int i = 0; i < n2; i++)
        {
            fileData << i*h << " ";
        }
        fileData << std::endl;
        
        //list y
        for(unsigned int i = 0; i < n2; i++)
        {
            fileData << i*h << " ";
        }
        fileData << std::endl;
        
        //list z
        for(int i = n1-1; i >= 0; i--)
        {
            fileData << -i*h << " ";
        }
        fileData << std::endl;
        
        for(unsigned int i = 0; i < n1; i++)
        {
            for(unsigned int j = 0; j < n2; j++)
            {
                for(unsigned int k = 0; k < n3; k++)
                {
                    fileData << vp[k*n1*n2 + j*n1 + (n1-1-i)] << " ";
                }
            }
        }
    }
    
    fileData.close();
    
    //Creation .geo file
    /*
     *
     *
     *       * x
     *      *
     *     *
     *    *
     *   *****************************************> y
     *   *
     *   *
     *   *
     *   *
     *   *
     *   * z
     *
     *
     */
    
    std::ofstream fileGeo("geometry.geo");
    
    fileGeo << "Include \"geometry.dat\";" << std::endl;
    
    if(param.sliceX)
    {
        fileGeo << "hStep = 500;//Step in meter" << std::endl << std::endl;
        
        fileGeo << "Point(1) = {" << 0      << ", " << 0     << ", " << 0    << ", 1.0};" << std::endl;
        fileGeo << "Point(2) = {" << 0      << ", " << -n1*h  << ", " << 0    << ", 1.0};" << std::endl;
        fileGeo << "Point(3) = {" << n2*h   << ", " << -n1*h  << ", " << 0    << ", 1.0};" << std::endl;
        fileGeo << "Point(4) = {" << n2*h   << ", " << 0     << ", " << 0    << ", 1.0};" << std::endl << std::endl;
        
        fileGeo << "Point(5) = {" << n2*h/2 << ", " << 0     << ", " << 0    << ", 1.0};" << std::endl;
        fileGeo << "Point(6) = {" << n2*h/2 << ", " << -n1*h  << ", " << 0 << ", 1.0};" << std::endl;
        
        fileGeo << "Line(1) = {1, 2};" << std::endl;
        fileGeo << "Line(2) = {2, 6};" << std::endl;
        fileGeo << "Line(3) = {6, 3};" << std::endl;
        fileGeo << "Line(4) = {3, 4};" << std::endl;
        fileGeo << "Line(5) = {4, 5};" << std::endl;
        fileGeo << "Line(6) = {5, 1};" << std::endl;
        fileGeo << "Line(7) = {5, 6};" << std::endl << std::endl;
        
        fileGeo << "Line Loop(8) = {1, 2, -7, 6};" << std::endl;
        fileGeo << "Plane Surface(9) = {8};" << std::endl;
        fileGeo << "Line Loop(10) = {3, 4, 5, 7};" << std::endl;
        fileGeo << "Plane Surface(11) = {10};" << std::endl  << std::endl;
        
        fileGeo << "Physical Point(SOURCE) = {5};" << std::endl;
        fileGeo << "Physical Line(TOP) = {6, 5};" << std::endl;
        fileGeo << "Physical Line(BOTTOM) = {2, 3};" << std::endl;
        fileGeo << "Physical Line(BORDER) = {1, 4};" << std::endl;
        fileGeo << "Physical Surface(GROUND) = {9, 11};" << std::endl << std::endl;
        
        fileGeo << "Transfinite Line {2, 3, 5, 6} = 6760/hStep+1 Using Progression 1;" << std::endl;
        fileGeo << "Transfinite Line {1, 4, 7} = 4200/hStep+1 Using Progression 1;" << std::endl << std::endl;
        
        fileGeo << "Transfinite Surface {9};" << std::endl;
        fileGeo << "Transfinite Surface {11};" << std::endl;
        fileGeo << "Recombine Surface {11, 9};" << std::endl << std::endl;
    }
    else
    {
        fileGeo << "hStep = 500;//Step in meter" << std::endl << std::endl;
        
        fileGeo << "Point(1) = {" << 0      << ", " << 0      << ", " << 0    << ", 1.0};" << std::endl;
        fileGeo << "Point(2) = {" << n3*h   << ", " << 0      << ", " << 0    << ", 1.0};" << std::endl;
        fileGeo << "Point(3) = {" << n3*h   << ", " << n2*h   << ", " << 0    << ", 1.0};" << std::endl;
        fileGeo << "Point(4) = {" << 0      << ", " << n2*h   << ", " << 0    << ", 1.0};" << std::endl << std::endl;
        
        fileGeo << "Point(5) = {" << n3*h/2 << ", " << 0      << ", " << 0    << ", 1.0};" << std::endl;
        fileGeo << "Point(6) = {" << n3*h/2 << ", " << n2*h   << ", " << 0    << ", 1.0};" << std::endl << std::endl;
        
        fileGeo << "Point(7) = {" << 0      << ", " << n2*h/2 << ", " << 0    << ", 1.0};" << std::endl;
        fileGeo << "Point(8) = {" << n3*h   << ", " << n2*h/2 << ", " << 0    << ", 1.0};" << std::endl << std::endl;
        
        fileGeo << "Point(9) = {" << n3*h/2 << ", " << n2*h/2 << ", " << 0    << ", 1.0};" << std::endl << std::endl;
        
        fileGeo << "Line(1) = {1, 5};" << std::endl;
        fileGeo << "Line(2) = {5, 2};" << std::endl;
        fileGeo << "Line(3) = {2, 8};" << std::endl;
        fileGeo << "Line(4) = {8, 3};" << std::endl;
        fileGeo << "Line(5) = {3, 6};" << std::endl;
        fileGeo << "Line(6) = {6, 4};" << std::endl;
        fileGeo << "Line(7) = {4, 7};" << std::endl;
        fileGeo << "Line(8) = {7, 1};" << std::endl << std::endl;
        
        fileGeo << "Line(9) = {9, 5};" << std::endl;
        fileGeo << "Line(10) = {9, 6};" << std::endl;
        fileGeo << "Line(11) = {9, 7};" << std::endl;
        fileGeo << "Line(12) = {9, 8};" << std::endl << std::endl;
        
        fileGeo << "Line Loop(1) = {1, -9, 11, 8};" << std::endl;
        fileGeo << "Plane Surface(1) = {1};" << std::endl << std::endl;

        fileGeo << "Line Loop(2) = {2, 3, -12, 9};" << std::endl;
        fileGeo << "Plane Surface(2) = {2};" << std::endl << std::endl;

        fileGeo << "Line Loop(3) = {4, 5, -10, 12};" << std::endl;
        fileGeo << "Plane Surface(3) = {3};" << std::endl << std::endl;
        
        fileGeo << "Line Loop(4) = {6, 7, -11, 10};" << std::endl;
        fileGeo << "Plane Surface(4) = {4};" << std::endl << std::endl;
        
        fileGeo << "Transfinite Line {8, 1, 7, 9, 11, 2, 6, 10, 3, 12, 4, 5} = 6760/hStep+1 Using Progression 1;" << std::endl;
        fileGeo << "Recombine Surface {1, 2, 3, 4};" << std::endl;
        fileGeo << "Transfinite Surface {1};" << std::endl;
        fileGeo << "Transfinite Surface {2};" << std::endl;
        fileGeo << "Transfinite Surface {3};" << std::endl;
        fileGeo << "Transfinite Surface {4};" << std::endl;
        fileGeo << "Extrude {0, 0, " << -n1*h << "} {" << std::endl;
        fileGeo << "\tSurface{1}; Surface{2}; Surface{3}; Surface{4}; Layers{4200/hStep+1}; Recombine;" << std::endl;
        fileGeo << "}" << std::endl << std::endl;
        
        fileGeo << "Physical Point(SOURCE) = {9};" << std::endl;
        fileGeo << "Physical Surface(TOP) = {4, 3, 2, 1};" << std::endl;
        fileGeo << "Physical Surface(BOTTOM) = {100, 34, 56, 78};" << std::endl;
        fileGeo << "Physical Surface(BORDER) = {65, 47, 43, 21, 33, 91, 87, 69};" << std::endl;
        fileGeo << "Physical Volume(GROUND) = {1, 2, 3, 4};" << std::endl;
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
