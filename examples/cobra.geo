cl__1 = 1;
Point(1) = {0, 0, 0, 1};
Point(2) = {0, 1, 0, 1};
Point(3) = {0, 1, 1, 1};
Point(4) = {0, 0, 1, 1};
Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};
//+
Line Loop(5) = {2, 3, 4, 1};
//+
Plane Surface(6) = {5};
//+
Extrude {5, 0, 0} {
  Surface{6};
}
