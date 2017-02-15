Group{
	Omega_1 = Region[{6}];
	Omega_2 = Region[{5}];
	Omega_0 = Region[{4}];


	Sigma_2_1 = Region[{9}];
	Sigma_1_2 = Region[{9}];
	Sigma_2_0 = Region[{8}];
	Sigma_0_2 = Region[{8}];
	Sigma_1_0 = Region[{7}];
	Sigma_0_1 = Region[{7}];

	Sigma_2 = Region[{8, 9}];
	Sigma_1 = Region[{7, 9}];
	Sigma_0 = Region[{7, 8}];




	D() = {0, 1, 2};
	N_DOM = #D();
	D_0 = {2, 1};
	D_1 = {2, 0};
	D_2 = {1, 0};
}


Function {
	myD = {} ; // the domains that I'm in charge of
	myD_0 = {};
	myD_1 = {};
	myD_2 = {};
	ListOfFields = {};
	ListOfConnectedFields = {};

	For idom In {0:N_DOM-1}
		If (idom % MPI_Size == MPI_Rank)
			myD() += idom;
			myD~{idom} += D~{idom}();
		EndIf
	EndFor
	For ii In {0:#myD()-1}
		i = myD(ii);
		If(#myD~{i}() == 2)
			Printf("We can do sweeping!");
		EndIf
		For jj In {0:#myD~{i}()-1}
			j = myD~{i}(jj);
			If (#myD~{i}() == 1)
				tag_g~{i}~{j} = D(i) * 1000 + D~{i};
			Else
				tag_g~{i}~{j} = D(i) * 1000 + D~{i}(jj);
			EndIf
			ListOfFields() += tag_g~{i}~{j};
			If(ANALYSIS == 0)
				g_in~{i}~{j}[ Sigma~{i}~{j} ] = ComplexScalarField[XYZ[]]{ tag_g~{i}~{j} };
			EndIf
			If(ANALYSIS == 1)
				g_in~{i}~{j}[ Sigma~{i}~{j} ] = ComplexVectorField[XYZ[]]{ tag_g~{i}~{j} };
			EndIf
		EndFor
	EndFor
}
