Group{
	Omega_2 = Region[{7}];
	Omega_3 = Region[{6}];
	Omega_0 = Region[{5}];
	Omega_1 = Region[{4}];


	Sigma_2_0 = Region[{10}];
	Sigma_0_2 = Region[{10}];
	BndSigma_2_0 = Region[{13}];
	BndSigma_0_2 = Region[{13}];
	BndGammaInf_2_0 = Region[{}];
	BndGammaInf_0_2 = Region[{}];
	BndGammaD_2_0 = Region[{}];
	BndGammaD_0_2 = Region[{}];
	BndGammaInf_2 = Region[{}];
	BndGammaInf_0 = Region[{}];
	BndGammaD_2 = Region[{}];
	BndGammaD_0 = Region[{}];
	Sigma_2_3 = Region[{12}];
	Sigma_3_2 = Region[{12}];
	BndSigma_2_3 = Region[{13}];
	BndSigma_3_2 = Region[{13}];
	BndGammaInf_2_3 = Region[{}];
	BndGammaInf_3_2 = Region[{}];
	BndGammaD_2_3 = Region[{}];
	BndGammaD_3_2 = Region[{}];
	BndGammaInf_2 = Region[{}];
	BndGammaInf_3 = Region[{}];
	BndGammaD_2 = Region[{}];
	BndGammaD_3 = Region[{}];
	Sigma_0_3 = Region[{11}];
	Sigma_3_0 = Region[{11}];
	BndSigma_0_3 = Region[{13}];
	BndSigma_3_0 = Region[{13}];
	BndGammaInf_0_3 = Region[{}];
	BndGammaInf_3_0 = Region[{}];
	BndGammaD_0_3 = Region[{}];
	BndGammaD_3_0 = Region[{}];
	BndGammaInf_0 = Region[{}];
	BndGammaInf_3 = Region[{}];
	BndGammaD_0 = Region[{}];
	BndGammaD_3 = Region[{}];
	Sigma_0_1 = Region[{9}];
	Sigma_1_0 = Region[{9}];
	BndSigma_0_1 = Region[{}];
	BndSigma_1_0 = Region[{}];
	BndGammaInf_0_1 = Region[{}];
	BndGammaInf_1_0 = Region[{}];
	BndGammaD_0_1 = Region[{}];
	BndGammaD_1_0 = Region[{}];
	BndGammaInf_0 = Region[{}];
	BndGammaInf_1 = Region[{}];
	BndGammaD_0 = Region[{}];
	BndGammaD_1 = Region[{}];
	Sigma_3_1 = Region[{8}];
	Sigma_1_3 = Region[{8}];
	BndSigma_3_1 = Region[{}];
	BndSigma_1_3 = Region[{}];
	BndGammaInf_3_1 = Region[{}];
	BndGammaInf_1_3 = Region[{}];
	BndGammaD_3_1 = Region[{}];
	BndGammaD_1_3 = Region[{}];
	BndGammaInf_3 = Region[{}];
	BndGammaInf_1 = Region[{}];
	BndGammaD_3 = Region[{}];
	BndGammaD_1 = Region[{}];

	Sigma_2 = Region[{10, 12}];
	Sigma_0 = Region[{9, 10, 11}];
	Sigma_3 = Region[{8, 11, 12}];
	Sigma_1 = Region[{8, 9}];

	BndSigma_1 = Region[{}];
	BndSigma_3 = Region[{13}];
	BndSigma_2 = Region[{13}];
	BndSigma_0 = Region[{13}];


	D() = {0, 1, 2, 3};
	N_DOM = #D();
	D_1 = {0, 3};
	D_3 = {2, 0, 1};
	D_0 = {2, 3, 1};
	D_2 = {0, 3};
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
			ListOfFields() += tag_g~{i}~{j};

			tag_g~{j}~{i} = j * 1000 + i;
			ListOfConnectedFields() += tag_g~{j}~{i};
			If(ANALYSIS == 0)
				g_in~{i}~{j}[ Sigma~{i}~{j} ] = ComplexScalarField[XYZ[]]{ tag_g~{i}~{j} };
			EndIf
			If(ANALYSIS == 1)
				g_in~{i}~{j}[ Sigma~{i}~{j} ] = ComplexVectorField[XYZ[]]{ tag_g~{i}~{j} };
			EndIf
		EndFor
	EndFor
}
