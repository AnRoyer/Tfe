Include "geometry.dat";
Point(1) = {0, 0, 0, 1.0};
Point(2) = {0, 13.52, 0, 1.0};
Point(3) = {4.2, 13.52, 0, 1.0};
Point(4) = {4.2, 0, 0, 1.0};

Point(5) = {4.2, 6.76, 0, 1.0};
Point(6) = {0, 6.76, 0, 1.0};
Line(1) = {1, 6};
Line(2) = {6, 2};
Line(3) = {2, 3};
Line(4) = {3, 5};
Line(5) = {5, 4};
Line(6) = {4, 1};
Line(7) = {5, 6};

Line Loop(8) = {2, 3, 4, 7};
Plane Surface(9) = {-8};
Line Loop(10) = {1, -7, 5, 6};
Plane Surface(11) = {-10};

Physical Point(SOURCE) = {5};
Physical Line(TOP) = {5, 4};
Physical Line(BOTTOM) = {1, 2};
Physical Line(BORDER) = {6, 3};
Physical Surface(GROUND) = {9, 11};

//To modify!!!!!
Transfinite Line {1, 2, 5, 4} = 500 Using Progression 1;
Transfinite Line {6, 7, 3} = (2*500*4.2/13.52) Using Progression 1;

Transfinite Surface {9};
Transfinite Surface {11};
Recombine Surface {11, 9};

