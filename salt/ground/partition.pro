Group{
	Omega_2 = Region[{8}];
	Omega_1 = Region[{7}];
	Omega_0 = Region[{6}];


	Sigma_1_2 = Region[{9}];
	Sigma_2_1 = Region[{9}];
	BndSigma_1_2 = Region[{10}];
	BndSigma_2_1 = Region[{10}];
	BndGammaInf_1_2 = Region[{}];
	BndGammaInf_2_1 = Region[{}];
	BndGammaD_1_2 = Region[{}];
	BndGammaD_2_1 = Region[{}];
	BndGammaInf_1 = Region[{}];
	BndGammaInf_2 = Region[{}];
	BndGammaD_1 = Region[{}];
	BndGammaD_2 = Region[{}];
	Sigma_1_0 = Region[{8}];
	Sigma_0_1 = Region[{8}];
	BndSigma_1_0 = Region[{10}];
	BndSigma_0_1 = Region[{10}];
	BndGammaInf_1_0 = Region[{}];
	BndGammaInf_0_1 = Region[{}];
	BndGammaD_1_0 = Region[{}];
	BndGammaD_0_1 = Region[{}];
	BndGammaInf_1 = Region[{}];
	BndGammaInf_0 = Region[{}];
	BndGammaD_1 = Region[{}];
	BndGammaD_0 = Region[{}];
	Sigma_2_0 = Region[{7}];
	Sigma_0_2 = Region[{7}];
	BndSigma_2_0 = Region[{10}];
	BndSigma_0_2 = Region[{10}];
	BndGammaInf_2_0 = Region[{}];
	BndGammaInf_0_2 = Region[{}];
	BndGammaD_2_0 = Region[{}];
	BndGammaD_0_2 = Region[{}];
	BndGammaInf_2 = Region[{}];
	BndGammaInf_0 = Region[{}];
	BndGammaD_2 = Region[{}];
	BndGammaD_0 = Region[{}];

	Sigma_1 = Region[{8, 9}];
	Sigma_2 = Region[{7, 9}];
	Sigma_0 = Region[{7, 8}];

	BndSigma_2 = Region[{10}];
	BndSigma_1 = Region[{10}];
	BndSigma_0 = Region[{10}];


	D() = {0, 1, 2};
	N_DOM = #D();
	D_0 = {1, 2};
	D_2 = {1, 0};
	D_1 = {2, 0};
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
			myD() += D(idom);
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

			tag_g~{i}~{j} = i * 1000 + j;
			tag_g~{j}~{i} = j * 1000 + i;

			ListOfFields() += tag_g~{i}~{j};
			ListOfConnectedFields() += 1;
			ListOfConnectedFields() += tag_g~{j}~{i};
			If(ANALYSIS == 0)
				g_in~{i}~{j}[ Sigma~{i}~{j} ] = ComplexScalarField[XYZ[]]{ tag_g~{j}~{i} };
			EndIf
			If(ANALYSIS == 1)
				g_in~{i}~{j}[ Sigma~{i}~{j} ] = ComplexVectorField[XYZ[]]{ tag_g~{j}~{i} };
			EndIf
		EndFor
	EndFor
}
