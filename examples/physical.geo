//+//+
Point(1) = {0, 0, 0, 1.0};
//+
Point(2) = {1, 0, 0, 1.0};
//+
Point(3) = {1, 1, 0, 1.0};
//+
Point(4) = {0, 1, 0, 1.0};
//+
Point(5) = {0, 0.3, 0, 1.0};
//+
Point(6) = {1, 0.7, 0, 1.0};
//+
Line(1) = {1, 2};
//+
Line(2) = {2, 6};
//+
Line(3) = {6, 5};
//+
Line(4) = {5, 1};
//+
Line(5) = {5, 4};
//+
Line(6) = {4, 3};
//+
Line(7) = {3, 6};
//+
Line Loop(8) = {5, 6, 7, 3};
//+
Plane Surface(9) = -{8};
//+
Line Loop(10) = {4, 1, 2, 3};
//+
Plane Surface(11) = {10};
//+
Physical Surface("s1") = {9};
//+
Physical Surface("s2") = {11};
//+
Physical Line("l1") = {3};
//+
Physical Line("l2") = {5, 6, 7};
//+
Physical Line("l3") = {4, 1, 2};
//+
Transfinite Line {5, 6, 4, 3, 7, 1, 2} = 31 Using Progression 1;
