/* vim: set tabstop=4 autoindent: */

#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <numeric>
#include <string>
#include <fstream>
#include <algorithm>

#include "rcpsp.hh"

int main(int argc, char * argv) {
	//TODO:
	//This is just a sample code

	float userI2 = 0.43;
	
	std::vector<RCPSP> generatedGraphes;
	float i2;
	for(int i=0;i<1000;i++) {
		RCPSP fullgraph(5);
		do {
			fullgraph.RandomReduction();
			i2 = fullgraph.I2();
		} while (i2 > userI2);
		generatedGraphes.push_back(fullgraph);
	}
	/*

	uint32_t lastFileId = 0,firstFileId=0;
	std::cout << "Program will search for files with .rcp extensions, from 1.rcp to {INDEX_YOU_SPECIFIED}.rcp file , and append TAO to the end of files, You better to backup your files first if you want to keep original files." << std::endl;
	std::cout << "first file index:";
	std::cin >> firstFileId;
	std::cout << "last file index:";
	std::cin >> lastFileId;
	for (uint32_t i = firstFileId; i<=lastFileId ; i++) {
		std::ifstream rcpspInFile(std::to_string(i) + ".rcp",std::ios::in);
		if (!rcpspInFile){
			std::cout << "error occured while trying to open " + std::to_string(i) + ".rcp" + " file!!" << std::endl;
			continue;
		}
		RCPSP rcpsp(rcpspInFile);
		float TAO = rcpsp.ComputeTAO();
		float esTAO = rcpsp.ComputeEarliestStartTAO();
		std::ofstream rcpspOutFile;
		rcpspOutFile.open(std::to_string(i) + ".rcp", std::ios::app);
		if (!rcpspOutFile)
			break;
		rcpspOutFile << std::endl << "TAO:" << TAO;
		rcpspOutFile << std::endl << "esTAO:" << esTAO;
		//calculate I1 to I5

		rcpspOutFile << std::endl << "I1:" << rcpsp.I1();
		rcpspOutFile << std::endl << "I2:" << rcpsp.I2();
		rcpspOutFile << std::endl << "I3:" << rcpsp.I3();
		rcpspOutFile << std::endl << "I4:" << rcpsp.I4();
		rcpspOutFile << std::endl << "I5:" << rcpsp.I5();
		rcpspOutFile << std::endl << "I6:" << rcpsp.I6();

		//

		rcpspOutFile << std::endl << "Resource Factor:" << rcpsp.CalculateResourceFactor();
		for (int resource = 0; resource < rcpsp.GetResourcesCount(); resource++)
			rcpspOutFile << std::endl << "Resource " << resource << " Strength:" << rcpsp.CalculateResourceStrength(resource);

		for (int resource = 0; resource < rcpsp.GetResourcesCount(); resource++)
			rcpspOutFile << std::endl << "Resource " << resource << " Constrainedness:" << rcpsp.CalculateResourceConstrainedness(resource);

		rcpspInFile.close();
		rcpspOutFile.close();
		rcpsp.SaveToCPlexFile(std::to_string(i));
	}

	*/
	return 0;
}

