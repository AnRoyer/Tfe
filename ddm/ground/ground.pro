ANALYSIS = 0;

Include "geometry.dat";
Include "partition.pro";

DefineConstant[ // allows to set these from outside
// Analysis type
ANALYSIS = {0, Name "Input/00Type of analysis", ReadOnly 1,
    Choices {0="Helmholtz", 1="Maxwell"}},
// wavenumber
// WAVENUMBER = {2*Pi*15, Name "Input/0Wavenumber"},
FREQ = {7, Name "Input/0Frequency"},
// base msh filename
MSH_BASE_NAME = "mesh_",
// directory for output files
DIR = "out/",
// xSource = 6200,
// ySource = -2300,
xSource = 4585,
ySource = -10,
nLayersTr = 1,
nLayersPml = 4
];

// prefix for (split) mesh files (one for each partition)
MSH_NAME = StrCat(DIR, MSH_BASE_NAME) ;

om = 2*Pi*FREQ;

DefineConstant[ // allows to set these from outside
  // transmission boundary condition
  TC_TYPE = {0, Name "Input/01Transmission condition",
    Choices {0="Order 0", 3="PML"}},
  NP_OSRC = 4,
  // sweeping preconditioner
  PRECONDITIONER = {0, Name "Input/01Sweeping preconditioner",
    Choices{0="Unpreconditioned",
      1="Double sweep",
      2="SGS"}},
  ListOfCuts() = { {0, N_DOM-1} },
  N_ON_TOP = {1, Name "Input/01Neumann condition on top",
	      Choices{0,1}}
];

DELTA_SOURCE = 1; // 1 ? delta function : dirichlet

Function {
  I[] = Complex[0,1];
  velocityField[] = InterpolationBilinear[ $1, $2 ]{ ListFromFile["velocity.dat"] };
  c[] = velocityField[ X[], Y[] ]/1000;

  om[] = om;

  k[] = om[]/c[] ;
  kInf[] = k[];
  kIBC[] = k[];

  uinc[] = 1.;

  V_SOURCE[] = 0.;
  fGrad[] = 0.;

  alphaBT[] = 0;
  betaBT[] = 0;
  
  D[] = 1;
  E[] = 1;
}

Group{
  For idom In {0:N_DOM-1}
    GammaD~{idom} = Region[{}]; // Point source
    GammaPoint~{idom} = Region[ {SOURCE} ]; // Point source
    GammaInf~{idom} = Region[ {BOTTOM} ]; // lower boundary
    GammaInf~{idom} += Region[ {BORDER} ]; // left and right boundaries
	GammaN~{idom} += Region[ {TOP} ]; // top boundary
    
    GammaD0~{idom} = Region[{}];
    
    BndGammaInf~{idom}~{0} = Region[{}];
    BndGammaInf~{idom}~{1} = Region[{}];
    BndGammaInf~{idom} = Region[{BndGammaInf~{idom}~{0}, BndGammaInf~{idom}~{1}}] ;
  EndFor
}

Function{
}

If (PRECONDITIONER)
  // what domains am I in charge of ? Implemented with a list
  ProcOwnsDomain = {};
  For idom In{0:N_DOM-1}
    ProcOwnsDomain += {(idom%MPI_Size == MPI_Rank)}; // define your rule here -- must match listOfDom()
  EndFor
EndIf

If (ANALYSIS == 0)
  Include "Helmholtz.pro";
EndIf

Include "Schwarz.pro";
