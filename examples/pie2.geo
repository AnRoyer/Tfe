// Gmsh project created on Fri Jan 27 14:15:43 2017
eps = 0.1;
//+
Point(1) = {1, 1+eps/2, 0, 1.0};
//+
Point(2) = {0, 2+eps/2, 0, 1.0};
//+
Point(3) = {-1, 1+eps/2, 0, 1.0};
//+
Point(4) = {0, 0+eps/2, 0, 1.0};
//+
Point(5) = {0, 1+eps/2, 0, 1.0};
//+
Circle(1) = {1, 5, 2};
//+
Circle(2) = {2, 5, 3};
//+
Circle(3) = {3, 5, 4};
//+
Circle(4) = {4, 5, 1};
//+
Point(100) = {1, -1-eps/2, 0, 1.0};
//+
Point(200) = {0, -2-eps/2, 0, 1.0};
//+
Point(300) = {-1, -1-eps/2, 0, 1.0};
//+
Point(400) = {0, 0-eps/2, 0, 1.0};
//+
Point(500) = {0, -1-eps/2, 0, 1.0};
//+
Circle(100) = {100, 500, 200};
//+
Circle(200) = {200, 500, 300};
//+
Circle(300) = {300, 500, 400};
//+
Circle(400) = {400, 500, 100};
//+
Point(1000) = {0, 0, 0, 1.0};
//+
Point(6) = {5, 0, 0, 1.0};
//+
Point(7) = {0, 5, 0, 1.0};
//+
Point(8) = {-5, 0, 0, 1.0};
//+
Point(9) = {0, -5, 0, 1.0};
//+
Circle(5) = {6, 1000, 7};
//+
Circle(6) = {7, 1000, 8};
//+
Circle(7) = {8, 1000, 9};
//+
Circle(8) = {9, 1000, 6};
//+
Line Loop(9) = {6, 7, 8, 5};
//+
Line Loop(10) = {2, 3, 4, 1};
//+
Line Loop(100) = {200, 300, 400, 100};
//+
Plane Surface(11) = {9, 10, 100};
//+
Physical Line("out") = {6, 5, 8, 7};
//+
Physical Line("int") = {2, 1, 4, 3, 200, 100, 400, 300};
//+
Physical Surface("surf") = {11};
//+
Transfinite Line {2, 1, 4, 3, 200, 100, 400, 300} = 21 Using Progression 1;
//+
Transfinite Line {6, 5, 8, 7} = 105 Using Progression 1;
