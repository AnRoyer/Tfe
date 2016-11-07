//+
Point(1) = {0, 0, 0, 1.0};
//+
Point(2) = {1, 0, 0, 1.0};
//+
Point(3) = {1, 1, 0, 1.0};
//+
Point(4) = {0, 1, 0, 1.0};
//+
Point(5) = {0.5, 0.5, 0, 1.0};
//+
Point(6) = {0.8, 0.5, 0, 1.0};
//+
Point(7) = {0.5, 0.8, 0, 1.0};
//+
Point(8) = {0.2, 0.5, 0, 1.0};
//+
Point(9) = {0.5, 0.2, 0, 1.0};
//+
Line(1) = {1, 2};
//+
Line(2) = {2, 3};
//+
Line(3) = {3, 4};
//+
Line(4) = {4, 1};
//+
Circle(5) = {6, 5, 7};
//+
Circle(6) = {7, 5, 8};
//+
Circle(7) = {8, 5, 9};
//+
Circle(8) = {9, 5, 6};
//+
Line Loop(9) = {4, 1, 2, 3};
//+
Line Loop(10) = {6, 7, 8, 5};
//+
Plane Surface(11) = {9, 10};
//+
Plane Surface(12) = {10};
//+
Transfinite Line {4, 3, 2, 1} = 10 Using Progression 1;
//+
Transfinite Line {6, 5, 8, 7} = 15 Using Progression 1;
//+
Recombine Surface {12};
