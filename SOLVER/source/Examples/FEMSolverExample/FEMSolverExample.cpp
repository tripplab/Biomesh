// FEMSolverExample.cpp
// Copyright (C) 2011 Miguel Vargas (miguel.vargas@gmail.com)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

//
// Compilation
// GCC:
//     g++ FEMSolverExample.cpp
// Intel:
//     icpc FEMSolverExample.cpp
//

#include <stdio.h>

int main()
{
// Commands
	enum Command {
		command_end              = 0, 
		command_set_connectivity = 1, 
		command_fill_A           = 2, 
		command_set_Ae           = 3, 
		command_set_all_Ae       = 4, 
		command_set_x            = 5, 
		command_set_b            = 6, 
		command_set_fixed        = 7, 
		command_solver_init      = 8, 
		command_solver_run       = 9};
// Number of elements
	int E = 21;
// Number of nodes
	int M = 17;
// Element type (2=Triangle, 3=Quadrilateral, 4=Tetrahedra, 5=Hexahedra)
	int T = 2;
// Nodes per element
	int N = 3;
// Degrees of freedom
	int D = 1;
// Connectivity
	int connectivity[21*3] = {
		3, 8, 5, 
		3, 5, 1, 
		5, 8, 12, 
		7, 2, 4, 
		7, 4, 11, 
		4, 2, 1, 
		17, 16, 13, 
		17, 13, 15, 
		13, 16, 14, 
		12, 15, 10, 
		10, 15, 13, 
		12, 10, 5, 
		14, 11, 9, 
		9, 11, 4, 
		14, 9, 13, 
		10, 13, 6, 
		6, 13, 9, 
		10, 6, 5, 
		6, 9, 4, 
		5, 6, 4, 
		1, 5, 4};
// Elemental matrices
	double Ke[21][3*3] ={
		{6.4699850468326987e-005, -2.6213892626938322e-005, -3.8485957841388651e-005, -2.6213892626938322e-005, 6.4699852119895029e-005, -3.8485959492956721e-005, -3.8485957841388651e-005, -3.8485959492956721e-005, 7.6971917334345372e-005}, 
		{0.0001194163, -6.4851481229273816e-005, -5.4564862420669250e-005, -6.4851481229273816e-005, 6.4518950865214794e-005, 3.3253036405902742e-007, -5.4564862420669250e-005, 3.3253036405902742e-007, 5.4232332056610221e-005}, 
		{6.4518952570947138e-005, -6.4851496361322073e-005, 3.3254379037494387e-007, -6.4851496361322073e-005, 0.0001194164, -5.4564874551608474e-005, 3.3254379037494387e-007, -5.4564874551608474e-005, 5.4232330761233532e-005}, 
		{6.4699857166259221e-005, -2.6213888552978070e-005, -3.8485968613281151e-005, -2.6213888552978070e-005, 6.4699842120739703e-005, -3.8485953567761646e-005, -3.8485968613281151e-005, -3.8485953567761646e-005, 7.6971922181042797e-005}, 
		{0.0001194164, -6.4851506330956674e-005, -5.4564867583856409e-005, -6.4851506330956674e-005, 6.4518961777500545e-005, 3.3254455345613586e-007, -5.4564867583856409e-005, 3.3254455345613586e-007, 5.4232323030400286e-005}, 
		{6.4518956933392303e-005, -6.4851494166733798e-005, 3.3253723334149865e-007, -6.4851494166733798e-005, 0.0001194164, -5.4564864260067519e-005, 3.3253723334149865e-007, -5.4564864260067519e-005, 5.4232327026726017e-005}, 
		{6.4699848987102081e-005, -2.6213890805761957e-005, -3.8485958181340134e-005, -2.6213890805761957e-005, 6.4699852125378693e-005, -3.8485961319616740e-005, -3.8485958181340134e-005, -3.8485961319616740e-005, 7.6971919500956867e-005}, 
		{0.0001194164, -6.4851485993385854e-005, -5.4564874361501674e-005, -6.4851485993385854e-005, 6.4518947014268201e-005, 3.3253897911763883e-007, -5.4564874361501674e-005, 3.3253897911763883e-007, 5.4232335382384034e-005}, 
		{6.4518961173541726e-005, -6.4851491940559977e-005, 3.3253076701823620e-007, -6.4851491940559977e-005, 0.0001194163, -5.4564854162971068e-005, 3.3253076701823620e-007, -5.4564854162971068e-005, 5.4232323395952824e-005}, 
		{6.4699842120739703e-005, -2.6213888552978070e-005, -3.8485953567761646e-005, -2.6213888552978070e-005, 6.4699857166259221e-005, -3.8485968613281151e-005, -3.8485953567761646e-005, -3.8485968613281151e-005, 7.6971922181042797e-005}, 
		{0.000118198, -6.4851513039450068e-005, -5.3346449325098674e-005, -6.4851513039450068e-005, 6.5184045310075873e-005, -3.3253227062582225e-007, -5.3346449325098674e-005, -3.3253227062582225e-007, 5.3678981595724479e-005}, 
		{6.5184037957109425e-005, -6.4851504353269536e-005, -3.3253360383990270e-007, -6.4851504353269536e-005, 0.000118198, -5.3346454060646762e-005, -3.3253360383990270e-007, -5.3346454060646762e-005, 5.3678987664486678e-005}, 
		{6.4699852125378693e-005, -2.6213890805761964e-005, -3.8485961319616733e-005, -2.6213890805761964e-005, 6.4699848987102095e-005, -3.8485958181340127e-005, -3.8485961319616733e-005, -3.8485958181340127e-005, 7.6971919500956867e-005}, 
		{0.0001181979, -6.4851479284895551e-005, -5.3346442685054684e-005, -6.4851479284895551e-005, 6.5184030546843529e-005, -3.3255126194796938e-007, -5.3346442685054684e-005, -3.3255126194796938e-007, 5.3678993947002662e-005}, 
		{6.5184022707578200e-005, -6.4851491940559963e-005, -3.3253076701823625e-007, -6.4851491940559963e-005, 0.000118198, -5.3346469426500777e-005, -3.3253076701823625e-007, -5.3346469426500777e-005, 5.3679000193519014e-005}, 
		{6.4434406098309372e-005, -2.5551726981809021e-005, -3.8882679116500358e-005, -2.5551726981809021e-005, 6.4434416237837695e-005, -3.8882689256028667e-005, -3.8882679116500358e-005, -3.8882689256028667e-005, 7.7765368372529039e-005}, 
		{7.6740651804875536e-005, -3.9583147051491118e-005, -3.7157504753384418e-005, -3.9583147051491118e-005, 6.6011012444926317e-005, -2.6427865393435195e-005, -3.7157504753384418e-005, -2.6427865393435195e-005, 6.3585370146819603e-005}, 
		{6.6010999096737498e-005, -3.9583145859760614e-005, -2.6427853236976891e-005, -3.9583145859760614e-005, 7.6740665893494642e-005, -3.7157520033734028e-005, -2.6427853236976891e-005, -3.7157520033734028e-005, 6.3585373270710926e-005}, 
		{7.4932933309631666e-005, -3.8450383464282361e-005, -3.6482549845349306e-005, -3.8450383464282361e-005, 6.6423858769906367e-005, -2.7973475305623986e-005, -3.6482549845349306e-005, -2.7973475305623986e-005, 6.4456025150973285e-005}, 
		{6.0475784682483771e-005, -6.7551660037995739e-005, 7.0758753555119602e-006, -6.7551660037995739e-005, 0.0001333117, -6.5760034998968276e-005, 7.0758753555119602e-006, -6.5760034998968276e-005, 5.8684159643456314e-005}, 
		{7.1839453851444221e-005, -3.5919728741874401e-005, -3.5919725109569793e-005, -3.5919728741874401e-005, 6.6664336196327976e-005, -3.0744607454453576e-005, -3.5919725109569793e-005, -3.0744607454453576e-005, 6.6664332564023382e-005}};
// Vector with constrain values
	double x[17] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 273, 0, 273, 0};
// Vector of independent terms
	double b[17] = {0.000281733, 0.000281733, 0, 0, 0, 0, 0.000281733, 0.000281733, 0, 0, 0.000281733, 0.000281733, 0, 0, 0.000281733, 0, 0.000281733};
// Vector that indicates where the constrains are
	bool fixed[17] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0};
// Result vector
	double r[17];
// Variable to indicate command
	char command;
// Data pipe
	FILE* FEMData;
// Result pipe
	FILE* FEMResult;
// Indexes
	int i;
// Names for the pipes
#ifdef WIN32
	const char* dataname = "\\\\.\\pipe\\FEMData";
	const char* resultname = "\\\\.\\pipe\\FEMResult";
#else
	const char* dataname = "/tmp/FEMData";
	const char* resultname = "/tmp/FEMResult";
#endif

// Open data and result pipes
	FEMData = fopen(dataname, "wb");
	FEMResult = fopen(resultname, "rb");

// Send mesh data
	command = command_set_connectivity;
	fwrite(&command, 1, 1, FEMData);
	// Send number of nodes
	fwrite(&M, sizeof(int), 1, FEMData);
	// Send number of elements
	fwrite(&E, sizeof(int), 1, FEMData);
	// Send element type
	fwrite(&T, sizeof(int), 1, FEMData);
	// Send nodes per element
	fwrite(&N, sizeof(int), 1, FEMData);
	// Send degrees of freedom
	fwrite(&D, sizeof(int), 1, FEMData);
	/// Send connectivity
	fwrite(connectivity, sizeof(int), E*N, FEMData);

// Send elemental matrices
	command = command_set_Ae;
	for (i = 0; i < E; ++i)
	{
		int e = i + 1;
		fwrite(&command, 1, 1, FEMData);
		fwrite(&e, sizeof(int), 1, FEMData);
		fwrite(Ke[i], sizeof(double), N*N, FEMData);
	}

	// Send vector with constrain values
	command = command_set_x;
	fwrite(&command, 1, 1, FEMData);
	fwrite(x, sizeof(double), M, FEMData);

	// Send vector of independent terms
	command = command_set_b;
	fwrite(&command, 1, 1, FEMData);
	fwrite(b, sizeof(double), M, FEMData);

	// Send vector that indicates where the constrains are
	command = command_set_fixed;
	fwrite(&command, 1, 1, FEMData);
	fwrite(fixed, sizeof(bool), M, FEMData);

	// Initialize solver
	command = command_solver_init;
	fwrite(&command, 1, 1, FEMData);

// Run solver and read result
	command = command_solver_run;
	fwrite(&command, 1, 1, FEMData);
	fflush(FEMData);
	fread(r, sizeof(double), M, FEMResult);

// Display result
	for (i = 0; i < M; ++i)
	{
		printf("%f\n", r[i]);
	}

// Send end session command
	command = command_end;
	fwrite(&command, 1, 1, FEMData);
	fflush(FEMData);

// Close pipes
	fclose(FEMResult);
	fclose(FEMData);

 	return 0;
}
