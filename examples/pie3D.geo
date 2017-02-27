// Gmsh project created on Fri Jan 27 14:15:43 2017
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
Extrude {0, 0, 0.5} {
  Surface{11};
}
//+
Physical Surface("out") = {24, 28, 32, 36};
//+
Physical Surface("int") = {40, 44, 48, 52};
//+
Physical Surface("surf") = {11, 53};
//+
Transfinite Line {2, 1, 4, 3, 17, 18, 19, 20} = 11 Using Progression 1;
//+
Transfinite Line {6, 5, 8, 7, 13, 14, 15, 16} = 55 Using Progression 1;
//+
Transfinite Line {23, 22, 31, 27, 43, 47, 38, 39} = 6 Using Progression 1;
//+
Physical Volume("vol") = {1};
