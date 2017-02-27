// Gmsh project created on Fri Jan 27 14:15:43 2017
//+
Point(1) = {0, 0, 0, 1.0};
//+
Point(2) = {1, 0, 0, 1.0};
//+
Point(3) = {1, 1, 0, 1.0};
//+
Point(4) = {0, 1, 0, 1.0};
//+
Point(5) = {0, 0.2, 0, 1.0};
//+
Point(6) = {1, 0.8, 0, 1.0};

//+
Line(1) = {1, 2};
//+
Line(2) = {2, 6};
//+
Line(3) = {6, 3};
//+
Line(4) = {3, 4};
//+
Line(5) = {4, 5};
//+
Line(6) = {5, 1};
//+
Line(7) = {5, 6};
//+
Line Loop(8) = {1, 2, -7, 6};
//+
Plane Surface(9) = {8};
//+
Line Loop(10) = {5, 7, 3, 4};
//+
Plane Surface(11) = {10};
//+
Physical Surface("surf1") = {11};
//+
Physical Surface("surf2") = {9};
//+
Physical Line("line") = {7};
//+
Physical Line("gamma") = {1, 2, 3, 4, 5, 6};
//+
Transfinite Line {1, 2, 3, 4, 5, 6, 7} = 11 Using Progression 1;
