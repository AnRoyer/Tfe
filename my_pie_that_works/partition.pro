N_DOM = 3;

Group{
	Omega_2 = Region[{6}];
	Omega_1 = Region[{5}];
	Omega_0 = Region[{4}];

	Sigma_1 = Region[{8, 9}];
	Sigma_2 = Region[{7, 8}];
	Sigma_0 = Region[{9, 7}];
//To work well
	Sigma_1_0 = Region[{8}];
	Sigma_1_1 = Region[{9}];
	Sigma_2_0 = Region[{7}];
	Sigma_2_1 = Region[{8}];
	Sigma_0_1 = Region[{7}];
	Sigma_0_0 = Region[{9}];
}


Function {
	ListOfSubdomains = {} ; // the domains that I'm in charge of
	ListOfFields = {}; // my fields
	ListOfConnectedFields = {}; // my neighbors

	For idom In {0:N_DOM-1}
		If (idom % MPI_Size == MPI_Rank)
			ListOfSubdomains += idom; 

			myFieldLeft  = {(2*(idom + N_DOM) + (0-1)) % (2*N_DOM)};
			myFieldRight = {(2*(idom + N_DOM) + (1-1)) % (2*N_DOM)};
			ListOfFields += {myFieldLeft(), myFieldRight()};
			If(ANALYSIS == 0)
				g_in~{idom}~{0}[Sigma~{idom}~{0}] = ComplexScalarField[XYZ[]];
				g_in~{idom}~{1}[Sigma~{idom}~{1}] = ComplexScalarField[XYZ[]];
			EndIf
			If(ANALYSIS == 1)
				g_in~{idom}~{0}[Sigma~{idom}~{0}] = ComplexVectorField[XYZ[]];
				g_in~{idom}~{1}[Sigma~{idom}~{1}] = ComplexVectorField[XYZ[]];
			EndIf
		EndIf
	EndFor
}
