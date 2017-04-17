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
    else
    {
        fileData << n1 << " " << n2 << " " << n3 << std::endl;
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
    else
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
        
        for(unsigned int i = 0; i < n3; i++)
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
            
            for(unsigned int k = 0; k < n3; k++)
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
        fileGeo << "hStep = 500;//Step in meter" << std::endl << std::endl;
        
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
        fileGeo << "Transfinite Line {1, 2, 5, 4} = 6760/hStep+1 Using Progression 1;" << std::endl;
        fileGeo << "Transfinite Line {6, 7, 3} = 4200/hStep+1 Using Progression 1;" << std::endl << std::endl;

        fileGeo << "Transfinite Surface {9};" << std::endl;
        fileGeo << "Transfinite Surface {11};" << std::endl;
        fileGeo << "Recombine Surface {11, 9};" << std::endl << std::endl;
    }
    else if(param.sliceZ)
    {
        fileGeo << "hStep = 500;//Step in meter" << std::endl << std::endl;
        
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
        fileGeo << "Transfinite Line {1, 2, 5, 4} = 6760/hStep+1 Using Progression 1;" << std::endl;
        fileGeo << "Transfinite Line {6, 7, 3} = 4200/hStep+1 Using Progression 1;" << std::endl << std::endl;
        
        fileGeo << "Transfinite Surface {9};" << std::endl;
        fileGeo << "Transfinite Surface {11};" << std::endl;
        fileGeo << "Recombine Surface {11, 9};" << std::endl << std::endl;
    }
    else
    {
        /*
         *                              .
         *                            . .
         *       .  .  .  .  .  .  ...  .  .  .  .  .  .  .
         *   ^x  .                . .   .                 .
         *   *   .              .   .   .                 .
         *   *   .   * z        .   .   .                 .
         *   *   . *            .   . .                   .
         *   *   *  .  .  .  .  .  ...  .  .  .  .  .  .  .
         *   * *                . .
         *   *******************.*********************> y
         *
         */
        fileGeo << "hStep = 500;//Step in meter" << std::endl << std::endl;
        
        fileGeo << "Point(1) = {0, 0, 0, 1.0};" << std::endl;
        fileGeo << "Point(2) = {0, " << n2*h << ", 0, 1.0};" << std::endl;
        fileGeo << "Point(3) = {0, " << n2*h << ", " << n3*h << ", 1.0};" << std::endl;
        fileGeo << "Point(4) = {0, 0, " << n3*h << ", 1.0};" << std::endl;
        fileGeo << "Point(5) = {" << n1*h << ", 0, 0, 1.0};" << std::endl;
        fileGeo << "Point(6) = {" << n1*h << ", " << n2*h << ", 0, 1.0};" << std::endl;
        fileGeo << "Point(7) = {" << n1*h << ", " << n2*h << ", " << n3*h << ", 1.0};" << std::endl;
        fileGeo << "Point(8) = {" << n1*h << ", 0, " << n3*h << ", 1.0};" << std::endl;
        
        fileGeo << "Point(9) = {0, " << n2*h/2 << ", " << n3*h/2 << ", 1.0};" << std::endl;
        fileGeo << "Point(10) = {" << n1*h << ", " << n2*h/2 << ", " << n3*h/2 << ", 1.0};" << std::endl;
        
        fileGeo << "Point(11) = {0, " << 0 << ", " << n3*h/2 << ", 1.0};" << std::endl;
        fileGeo << "Point(12) = {" << n1*h << ", " << 0 << ", " << n3*h/2 << ", 1.0};" << std::endl;
        fileGeo << "Point(13) = {0, " << n2*h << ", " << n3*h/2 << ", 1.0};" << std::endl;
        fileGeo << "Point(14) = {" << n1*h << ", " << n2*h << ", " << n3*h/2 << ", 1.0};" << std::endl;
        
        fileGeo << "Point(15) = {0, " << n2*h/2 << ", " << 0 << ", 1.0};" << std::endl;
        fileGeo << "Point(16) = {" << n1*h << ", " << n2*h/2 << ", " << 0 << ", 1.0};" << std::endl;
        fileGeo << "Point(17) = {0, " << n2*h/2 << ", " << n3*h << ", 1.0};" << std::endl;
        fileGeo << "Point(18) = {" << n1*h << ", " << n2*h/2 << ", " << n3*h << ", 1.0};" << std::endl  << std::endl;
        
        fileGeo << "Line(1) = {1, 15};" << std::endl;
        fileGeo << "Line(2) = {15, 2};" << std::endl;
        fileGeo << "Line(3) = {2, 13};" << std::endl;
        fileGeo << "Line(4) = {13, 3};" << std::endl;
        fileGeo << "Line(5) = {3, 17};" << std::endl;
        fileGeo << "Line(6) = {17, 4};" << std::endl;
        fileGeo << "Line(7) = {4, 11};" << std::endl;
        fileGeo << "Line(8) = {11, 1};" << std::endl;
        fileGeo << "Line(9) = {5, 16};" << std::endl;
        fileGeo << "Line(10) = {16, 6};" << std::endl;
        fileGeo << "Line(11) = {6, 14};" << std::endl;
        fileGeo << "Line(12) = {14, 7};" << std::endl;
        fileGeo << "Line(13) = {7, 18};" << std::endl;
        fileGeo << "Line(14) = {18, 8};" << std::endl;
        fileGeo << "Line(15) = {8, 12};" << std::endl;
        fileGeo << "Line(16) = {12, 5};" << std::endl;
        fileGeo << "Line(17) = {1, 5};" << std::endl;
        fileGeo << "Line(18) = {2, 6};" << std::endl;
        fileGeo << "Line(19) = {3, 7};" << std::endl;
        fileGeo << "Line(20) = {4, 8};" << std::endl;
        fileGeo << "Line(21) = {9, 10};" << std::endl;
        fileGeo << "Line(22) = {9, 11};" << std::endl;
        fileGeo << "Line(23) = {11, 12};" << std::endl;
        fileGeo << "Line(24) = {12, 10};" << std::endl;
        fileGeo << "Line(25) = {9, 13};" << std::endl;
        fileGeo << "Line(26) = {13, 14};" << std::endl;
        fileGeo << "Line(27) = {14, 10};" << std::endl;
        fileGeo << "Line(28) = {9, 15};" << std::endl;
        fileGeo << "Line(29) = {15, 16};" << std::endl;
        fileGeo << "Line(30) = {16, 10};" << std::endl;
        fileGeo << "Line(31) = {9, 17};" << std::endl;
        fileGeo << "Line(32) = {17, 18};" << std::endl;
        fileGeo << "Line(33) = {18, 10};" << std::endl << std::endl;
        
        fileGeo << "Line Loop(34) = {1, -28, 22, 8};" << std::endl;
        fileGeo << "Plane Surface(35) = -{34};" << std::endl;
        fileGeo << "Line Loop(36) = {2, 3, -25, 28};" << std::endl;
        fileGeo << "Plane Surface(37) = -{36};" << std::endl;
        fileGeo << "Line Loop(38) = {31, -5, -4, -25};" << std::endl;
        fileGeo << "Plane Surface(39) = {38};" << std::endl;
        fileGeo << "Line Loop(40) = {7, -22, 31, 6};" << std::endl;
        fileGeo << "Plane Surface(41) = -{40};" << std::endl;
        fileGeo << "Line Loop(42) = {16, 9, 30, -24};" << std::endl;
        fileGeo << "Plane Surface(43) = {42};" << std::endl;
        fileGeo << "Line Loop(44) = {30, -27, -11, -10};" << std::endl;
        fileGeo << "Plane Surface(45) = -{44};" << std::endl;
        fileGeo << "Line Loop(46) = {33, -27, 12, 13};" << std::endl;
        fileGeo << "Plane Surface(47) = {46};" << std::endl;
        fileGeo << "Line Loop(48) = {15, 24, -33, 14};" << std::endl;
        fileGeo << "Plane Surface(49) = {48};" << std::endl;
        fileGeo << "Line Loop(50) = {1, 29, -9, -17};" << std::endl;
        fileGeo << "Plane Surface(51) = {50};" << std::endl;
        fileGeo << "Line Loop(52) = {2, 18, -10, -29};" << std::endl;
        fileGeo << "Plane Surface(53) = {52};" << std::endl;
        fileGeo << "Line Loop(54) = {18, 11, -26, -3};" << std::endl;
        fileGeo << "Plane Surface(55) = -{54};" << std::endl;
        fileGeo << "Line Loop(56) = {26, 12, -19, -4};" << std::endl;
        fileGeo << "Plane Surface(57) = -{56};" << std::endl;
        fileGeo << "Line Loop(58) = {13, -32, -5, 19};" << std::endl;
        fileGeo << "Plane Surface(59) = -{58};" << std::endl;
        fileGeo << "Line Loop(60) = {14, -20, -6, 32};" << std::endl;
        fileGeo << "Plane Surface(61) = -{60};" << std::endl;
        fileGeo << "Line Loop(62) = {20, 15, -23, -7};" << std::endl;
        fileGeo << "Plane Surface(63) = -{62};" << std::endl;
        fileGeo << "Line Loop(64) = {8, 17, -16, -23};" << std::endl;
        fileGeo << "Plane Surface(65) = {64};" << std::endl;
        fileGeo << "Line Loop(66) = {23, 24, -21, 22};" << std::endl;
        fileGeo << "Plane Surface(67) = -{66};" << std::endl;
        fileGeo << "Line Loop(68) = {21, -27, -26, -25};" << std::endl;
        fileGeo << "Plane Surface(69) = {68};" << std::endl;
        fileGeo << "Line Loop(70) = {21, -30, -29, -28};" << std::endl;
        fileGeo << "Plane Surface(71) = {70};" << std::endl;
        fileGeo << "Line Loop(72) = {21, -33, -32, -31};" << std::endl;
        fileGeo << "Plane Surface(73) = {72};" << std::endl << std::endl;
        
        fileGeo << "Surface Loop(74) = {35, 65, 51, 43, 71, 67};" << std::endl;
        fileGeo << "Volume(75) = {74};" << std::endl;
        fileGeo << "Surface Loop(76) = {71, 53, 37, 55, 45, 69};" << std::endl;
        fileGeo << "Volume(77) = {76};" << std::endl;
        fileGeo << "Surface Loop(78) = {73, 69, 57, 39, 59, 47};" << std::endl;
        fileGeo << "Volume(79) = {78};" << std::endl;
        fileGeo << "Surface Loop(80) = {61, 41, 63, 49, 67, 73};" << std::endl;
        fileGeo << "Volume(81) = {80};" << std::endl << std::endl;
        
        fileGeo << "Physical Point(SOURCE) = {10};" << std::endl;
        fileGeo << "Physical Surface(TOP) = {47, 45, 43, 49};" << std::endl;
        fileGeo << "Physical Surface(BOTTOM) = {39, 37, 41, 35};" << std::endl;
        fileGeo << "Physical Surface(BORDER) = {59, 57, 55, 53, 51, 65, 63, 61};" << std::endl;
        fileGeo << "Physical Volume(GROUND) = {81, 79, 77, 75};" << std::endl << std::endl;
        
        fileGeo << "Transfinite Line {8, 1, 28, 22, 2, 3, 25, 4, 5, 31, 6, 7, 9, 30, 24, 16, 10, 11, 27, 12, 13, 33, 14, 15} = 6760/hStep+1 Using Progression 1;" << std::endl;
        fileGeo << "Transfinite Line {29, 18, 26, 19, 32, 20, 23, 17, 21} = 4200/hStep+1 Using Progression 1;" << std::endl << std::endl;
        
        fileGeo << "Transfinite Surface {39} = {9, 13, 3, 17};" << std::endl;
        fileGeo << "Transfinite Surface {41} = {11, 9, 17, 4};" << std::endl;
        fileGeo << "Transfinite Surface {35} = {1, 15, 9, 11};" << std::endl;
        fileGeo << "Transfinite Surface {43} = {16, 5, 12, 10};" << std::endl;
        fileGeo << "Transfinite Surface {45} = {10, 14, 6, 16};" << std::endl;
        fileGeo << "Transfinite Surface {39} = {17, 3, 13, 9};" << std::endl;
        fileGeo << "Transfinite Surface {41} = {4, 17, 9, 11};" << std::endl;
        fileGeo << "Transfinite Surface {61} = {4, 8, 18, 17};" << std::endl;
        fileGeo << "Transfinite Surface {59} = {18, 7, 3, 17};" << std::endl;
        fileGeo << "Transfinite Surface {57} = {3, 7, 14, 13};" << std::endl;
        fileGeo << "Transfinite Surface {55} = {14, 6, 2, 13};" << std::endl;
        fileGeo << "Transfinite Surface {53} = {2, 6, 16, 15};" << std::endl;
        fileGeo << "Transfinite Surface {51} = {16, 5, 1, 15};" << std::endl;
        fileGeo << "Transfinite Surface {65} = {1, 5, 12, 11};" << std::endl;
        fileGeo << "Transfinite Surface {63} = {11, 12, 8, 4};" << std::endl;
        fileGeo << "Transfinite Surface {67} = {9, 10, 12, 11};" << std::endl;
        fileGeo << "Transfinite Surface {73} = {9, 10, 18, 17};" << std::endl;
        fileGeo << "Transfinite Surface {69} = {9, 10, 14, 13};" << std::endl;
        fileGeo << "Transfinite Surface {71} = {9, 15, 16, 10};" << std::endl;
        fileGeo << "Transfinite Surface {49} = {10, 18, 8, 12};" << std::endl;
        fileGeo << "Transfinite Surface {47} = {14, 7, 18, 10};" << std::endl;
        fileGeo << "Transfinite Surface {37} = {13, 2, 15, 9};" << std::endl;
        fileGeo << "Recombine Surface {59, 57, 39, 61, 47, 73, 69, 41, 55, 37, 49, 63, 67, 45, 71, 53, 35, 43, 65, 51};" << std::endl << std::endl;
        
        fileGeo << "Transfinite Volume{81} = {4, 11, 9, 17, 8, 12, 10, 18};" << std::endl;
        fileGeo << "Transfinite Volume{75} = {1, 15, 9, 11, 5, 16, 10, 12};" << std::endl;
        fileGeo << "Transfinite Volume{77} = {15, 2, 13, 9, 16, 6, 14, 10};" << std::endl;
        fileGeo << "Transfinite Volume{79} = {9, 13, 3, 17, 10, 14, 7, 18};" << std::endl;
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
