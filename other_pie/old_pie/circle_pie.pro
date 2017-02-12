Include "circle_pie_data.geo";

DefineConstant[ // allows to set these from outside
  // transmission boundary condition
  TC_TYPE = {2, Name "Input/01Transmission condition",
    Choices {0="Order 0", 1="Order 2", 2="Pade (OSRC)"}},
  NP_OSRC = 4,
  PRECONDITIONER = {0, Name "Input/01Sweeping preconditioner", ReadOnly 1,
    Choices{0="Unpreconditioned",
      1="Double sweep",
      2="SGS"}},
  ListOfCuts() = { {0, N_DOM} }
];

Function {
  I[] = Complex[0, 1];
  k = WAVENUMBER;
  k[] = k;

  // incidence angle
  theta_inc = THETA_INC;
  XYZdotTheta[] = X[] * Cos[theta_inc] + Y[] * Sin[theta_inc];
  uinc[] = Complex[Cos[k*XYZdotTheta[]], Sin[k*XYZdotTheta[]]];
  grad_uinc[] =  I[] * k * Vector[1,0,0] * uinc[];
  dn_uinc[] = Normal[] * grad_uinc[];

  // parameter for ABC
  kInf[] = k;
  alphaBT[] = 1/(2*R_EXT) - I[]/(8*k*R_EXT^2*(1+I[]/(k*R_EXT)));
  betaBT[] = - 1/(2*I[]*k*(1+I[]/(k*R_EXT)));

  // parameter for 0th order TC
  kIBC[] = k + (2*Pi /-I[]);

  // parameters for 2nd order TC
  // OO2 Gander 2002, pp. 46-47
  xsimin = 0;
  xsimax = Pi / LC;
  deltak[] = Pi ; // check this
  alphastar[] = I[] * ((k^2 - xsimin^2) * (k^2 - (k-deltak[])^2))^(1/4);
  betastar[] = ((xsimax^2 - k^2) * ((k+deltak[])^2 - k^2))^(1/4);
  a[] = - (alphastar[] * betastar[] - k^2) / (alphastar[] + betastar[]);
  b[] = - 1 / (alphastar[] + betastar[]);

  // parameters for Pade-type TC
  kepsI = 0.;
  keps[] = k*(1+kepsI*I[]);
  theta_branch = Pi/4;

  // not ready yet for PMLs
  D[] = 1;
  E[] = 1;
}

Group{
    Omega~{0} = Region[(4)];
    Omega~{1} = Region[(5)];
    Omega~{2} = Region[(6)];
    
    Sigma~{0}~{0} = Region[{(9)}]; // left boundary (if looking to the center from infinity)
    Sigma~{0}~{1} = Region[{(7)}]; // right boundary
    
    Sigma~{1}~{0} = Region[{(8)}]; // left boundary (if looking to the center from infinity)
    Sigma~{1}~{1} = Region[{(9)}]; // right boundary

    Sigma~{2}~{0} = Region[{(7)}]; // left boundary (if looking to the center from infinity)
    Sigma~{2}~{1} = Region[{(8)}]; // right boundary
    
For idom In {0:N_DOM-1}
    GammaD0~{idom} = Region[{}];
    GammaD~{idom} = Region[{(2)}];
    GammaN~{idom} = Region[{}];
    GammaInf~{idom} = Region[{(1)}];

    Sigma~{idom} = Region[{Sigma~{idom}~{0}, Sigma~{idom}~{1}}] ;

    BndGammaD~{idom}~{0} = Region[{}];
    BndGammaD~{idom}~{1} = Region[{}];
    BndGammaD~{idom} = Region[{}] ;

    BndGammaInf~{idom}~{0} = Region[{}];
    BndGammaInf~{idom}~{1} = Region[{}];
    BndGammaInf~{idom} = Region[{}] ;

    BndSigma~{idom}~{0} = Region[{}];
    BndSigma~{idom}~{1} = Region[{}];
    BndSigma~{idom} = Region[{}] ;
  EndFor
}

Function{
  // definitions for parallel (MPI) runs:

  ListOfSubdomains = {} ; // the domains that I'm in charge of
  ListOfFields = {}; // my fields
  ListOfConnectedFields = {}; // my neighbors

  // this describes a layered (1-d like) decomposition (domain 0 and N-1 are adjacent)
  //         +--------+------+------+---...---+------------+
  //  field: |2N-1   0|1    2|3    4|5    2N-4|2N-3    2N-2|
  //   idom: |    0   |   1  |   2  |         |    N-1     |
  //         +--------+------+------+---...---+------------+

  For idom In {0:N_DOM-1}
    If (idom % MPI_Size == MPI_Rank)
      // my fields
      myFieldLeft  = {(2*(idom + N_DOM) + (0-1)) % (2*N_DOM)};
      myFieldRight = {(2*(idom + N_DOM) + (1-1)) % (2*N_DOM)};
      connectedFieldLeft  = {(2*(idom - 1 + N_DOM)+(1-1)) % (2*N_DOM)}; // right boundary of left neightbor
      connectedFieldRight = {(2*(idom + 1 + N_DOM)+(0-1)) % (2*N_DOM)}; // left boundary of right neightbor
      // 2 "blocks"

      ListOfSubdomains += idom;
      ListOfFields += {myFieldLeft(), myFieldRight()};
      If(ANALYSIS == 0)
        g_in~{idom}~{0}[Sigma~{idom}~{0}] = ComplexScalarField[XYZ[]]{connectedFieldLeft()};
        g_in~{idom}~{1}[Sigma~{idom}~{1}] = ComplexScalarField[XYZ[]]{connectedFieldRight()};
      EndIf
      If(ANALYSIS == 1)
        g_in~{idom}~{0}[Sigma~{idom}~{0}] = ComplexVectorField[XYZ[]]{connectedFieldLeft()};
        g_in~{idom}~{1}[Sigma~{idom}~{1}] = ComplexVectorField[XYZ[]]{connectedFieldRight()};
      EndIf
    EndIf
  EndFor

  /*
  MPI_Printf["ListOfSubdomains = ", ListOfSubdomains()];
  MPI_Printf["ListOfFields = ", ListOfFields()];
  MPI_Printf["ListOfConnectedFields = ", ListOfConnectedFields()];
  */
}

Include "Helmholtz.pro" ;
Include "Schwarz.pro" ;
