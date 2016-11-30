//+
Point(1) = {0, 0, 0, 1.0};
//+
Point(2) = {1, 0, 0, 1.0};
//+
Point(3) = {1, 1, 0, 1.0};
//+
Point(4) = {0, 1, 0, 1.0};
//+
Line(1) = {1, 2};
//+
Line(2) = {2, 3};
//+
Line(3) = {3, 4};
//+
Line(4) = {4, 1};
//+
Line Loop(5) = {4, 1, 2, 3};
//+
Plane Surface(6) = {5};

//+
Extrude {0, 0, 1} {
  Surface{6};
}
//+
Transfinite Line {11, 10, 9, 8, 22, 18, 14, 13, 4, 3, 2, 1} = 10 Using Progression 1;