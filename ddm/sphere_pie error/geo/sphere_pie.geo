// Gmsh project created on Sat Apr 29 22:41:31 2017
DefineConstant[
meshSize = { 20, Name "Mesh Size", Min 8, Max 30}
];
SetFactory("OpenCASCADE");
//+
Sphere(1) = {0, 0, 0, 1, -Pi/2, Pi/2, 2*Pi};
//+
Sphere(2) = {0, 0, 0, 5, -Pi/2, Pi/2, 2*Pi};
//+
BooleanDifference{ Volume{2}; Delete; }{ Volume{1}; Delete; }
//+
Physical Surface("out") = {2};
//+
Physical Surface("int") = {1};
//+
Physical Volume("surf") = {2};
//+
Transfinite Line {2} = meshSize Using Progression 1;
//+
Transfinite Line {5} = 5*meshSize Using Progression 1;
