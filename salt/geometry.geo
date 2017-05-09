Include "geometry.dat";
hStep = 1000;//Step in meter

Point(1) = {0, 0, 0, 1.0};
Point(2) = {13.52, 0, 0, 1.0};
Point(3) = {13.52, 13.52, 0, 1.0};
Point(4) = {0, 13.52, 0, 1.0};

Point(5) = {6.76, 0, 0, 1.0};
Point(6) = {6.76, 13.52, 0, 1.0};

Point(7) = {0, 6.76, 0, 1.0};
Point(8) = {13.52, 6.76, 0, 1.0};

Point(9) = {6.76, 6.76, 0, 1.0};

Line(1) = {1, 5};
Line(2) = {5, 2};
Line(3) = {2, 8};
Line(4) = {8, 3};
Line(5) = {3, 6};
Line(6) = {6, 4};
Line(7) = {4, 7};
Line(8) = {7, 1};

Line(9) = {9, 5};
Line(10) = {9, 6};
Line(11) = {9, 7};
Line(12) = {9, 8};

Line Loop(1) = {1, -9, 11, 8};
Plane Surface(1) = {1};

Line Loop(2) = {2, 3, -12, 9};
Plane Surface(2) = {2};

Line Loop(3) = {4, 5, -10, 12};
Plane Surface(3) = {3};

Line Loop(4) = {6, 7, -11, 10};
Plane Surface(4) = {4};

Transfinite Line {8, 1, 7, 9, 11, 2, 6, 10, 3, 12, 4, 5} = 6760/hStep+1 Using Progression 1;
Recombine Surface {1, 2, 3, 4};
Transfinite Surface {1};
Transfinite Surface {2};
Transfinite Surface {3};
Transfinite Surface {4};
Extrude {0, 0, -4.2} {
	Surface{1}; Surface{2}; Surface{3}; Surface{4}; Layers{4200/hStep+1}; Recombine;
}

Physical Point(SOURCE) = {9};
Physical Surface(TOP) = {4, 3, 2, 1};
Physical Surface(BOTTOM) = {100, 34, 56, 78};
Physical Surface(BORDER) = {65, 47, 43, 21, 33, 91, 87, 69};
Physical Volume(GROUND) = {1, 2, 3, 4};
