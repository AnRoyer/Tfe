// Gmsh project created on Fri Jan 27 14:15:43 2017
DefineConstant[
meshSize = { 20, Name "Mesh Size", Min 8, Max 30}
];
//+
Point(1) = {1, 0, 0, 1.0};
//+
Point(2) = {0, 1, 0, 1.0};
//+
Point(3) = {-1, 0, 0, 1.0};
//+
Point(4) = {0, -1, 0, 1.0};
//+
Point(5) = {0, 0, 0, 1.0};
//+
Circle(1) = {1, 5, 2};
//+
Circle(2) = {2, 5, 3};
//+
Circle(3) = {3, 5, 4};
//+
Circle(4) = {4, 5, 1};
//+
Point(6) = {5, 0, 0, 1.0};
//+
Point(7) = {0, 5, 0, 1.0};
//+
Point(8) = {-5, 0, 0, 1.0};
//+
Point(9) = {0, -5, 0, 1.0};
//+
Circle(5) = {6, 5, 7};
//+
Circle(6) = {7, 5, 8};
//+
Circle(7) = {8, 5, 9};
//+
Circle(8) = {9, 5, 6};
//+
Line Loop(9) = {6, 7, 8, 5};
//+
Line Loop(10) = {2, 3, 4, 1};
//+
Plane Surface(11) = {9, 10};
//+
Physical Line("out") = {6, 5, 8, 7};
//+
Physical Line("int") = {2, 1, 4, 3};
//+
Physical Surface("surf") = {11};
//+
Transfinite Line {2, 1, 4, 3} = meshSize Using Progression 1;
//+
Transfinite Line {6, 5, 8, 7} = 5*meshSize Using Progression 1;
