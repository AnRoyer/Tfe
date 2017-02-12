Group{
	Omega_3 = Region[{7}];
	Omega_1 = Region[{6}];
	Omega_0 = Region[{5}];
	Omega_2 = Region[{4}];


	Sigma_3_2 = Region[{14, 9}];
	Sigma_2_3 = Region[{14, 9}];
	Sigma_3_0 = Region[{14, 10}];
	Sigma_0_3 = Region[{14, 10}];
	Sigma_2_1 = Region[{13, 8}];
	Sigma_1_2 = Region[{13, 8}];
	Sigma_2_0 = Region[{13, 14, 11}];
	Sigma_0_2 = Region[{13, 14, 11}];
	Sigma_1_0 = Region[{13, 12}];
	Sigma_0_1 = Region[{13, 12}];

	Sigma_3 = Region[{14, 9, 10}];
	Sigma_2 = Region[{13, 14, 8, 9, 11}];
	Sigma_1 = Region[{13, 8, 12}];
	Sigma_0 = Region[{13, 14, 10, 11, 12}];


	D() = {0, 1, 2, 3};
	N_DOM = #D();
	D_1 = {2, 0};
	D_0 = {3, 2, 1};
	D_2 = {3, 1, 0};
	D_3 = {2, 0};
}


Function {
	myD = {} ; // the domains that I'm in charge of
	myD_0 = {};
	myD_1 = {};
	myD_2 = {};
	myD_3 = {};
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
			g_in~{i}~{j}[ Sigma~{i}~{j} ] = ComplexVectorField[XYZ[]]{ tag_g~{i}~{j} };
		EndFor
	EndFor
}
