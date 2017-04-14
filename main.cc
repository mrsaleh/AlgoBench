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


float ComputeDistributionOfActivities(std::vector<uint32_t> _levelsWidth, uint32_t _numberOfWorks) {
	// Sigma(Wa,Wbar) a=1 to m / 2(m-1)(Wbar-1)
	float wBar = 0.0f;
	//Calculate wBar which is average of levels width
	wBar = (float)std::accumulate(_levelsWidth.begin(), _levelsWidth.end(), 0);
	wBar = wBar / static_cast<float>(_levelsWidth.size());
	//Calculate Standard Deviation
	float sigma = 0.0f;
	for (std::vector<uint32_t>::const_iterator it = _levelsWidth.begin(); it != _levelsWidth.end(); it++) {
		sigma += fabs(wBar - (*it));
	}
	sigma = sigma / (static_cast<float>(_numberOfWorks - 1)*(wBar - 1.0f));
	return sigma;
}

uint32_t ComputeLevelsNumber(uint32_t _numberOfWorks, float _ConcurrencyIndicator) {
	// I2 = (m-1) / (n-1) where m is levels number and n is number of works
	//so (m-1) = I2 * (n-1) -> m = (I2 * (n-1)) + 1
	uint32_t levelsNumber = static_cast<uint32_t>(round((_ConcurrencyIndicator * ((float)_numberOfWorks - 1.0f)) + 1.0f));
	std::cout << "s:" << _ConcurrencyIndicator * ((float)_numberOfWorks - 1.0f) << std::endl;
	return levelsNumber;
}

void TestComputeLevelsNumber() {
	uint32_t result = ComputeLevelsNumber(23, 0.73f);
	if (17 != result) {
		std::cout << "ComputeLevelsNumber Test failed!" << result << std::endl;
		std::exit(1);
	}
	result = ComputeLevelsNumber(14, 0.43f);
	if (7 != result) {
		std::cout << "ComputeLevelsNumber Test failed!" << result << std::endl;
		std::exit(1);
	}
	result = ComputeLevelsNumber(39, 0.83f);
	if (33 != result) {
		std::cout << "ComputeLevelsNumber Test failed!" << result << std::endl;
		std::exit(1);
	}

	std::cout << "ComputeLevelsNumber Test Passed!" << std::endl;
}

int main(int argc, char * argv) {
	uint32_t lastFileId = 0;
	std::cout << "Program will search for files with .rcp extensions, from 1.rcp to {INDEX_YOU_SPECIFIED}.rcp file , and append TAO to the end of files, You better to backup your files first if you want to keep original files." << std::endl;
	std::cout << "Enter last file index(first file name supposed to be 1):";
	std::cin >> lastFileId;
	for (uint32_t i = 1; i <= lastFileId; i++) {
		RCPSP rcpsp(std::to_string(i) + ".rcp");
		float TAO = rcpsp.ComputeTAO();
		float esTAO = rcpsp.ComputeEarliestStartTAO();
		std::ofstream rcpspFile;
		rcpspFile.open(std::to_string(i) + ".rcp", std::ios::app);
		rcpspFile << std::endl << "TAO:" << TAO;
		rcpspFile << std::endl << "esTAO:" << esTAO;
		//calculate I1 to I5
		rcpsp.CalculateI1();
		rcpsp.CalculateI2();
		rcpsp.CalculateI3();
		rcpsp.CalculateI4();
		rcpsp.CalculateI5();
		rcpsp.CalculateI6();

		rcpspFile << std::endl << "I1:" << rcpsp.I1();
		rcpspFile << std::endl << "I2:" << rcpsp.I2();
		rcpspFile << std::endl << "I3:" << rcpsp.I3();
		rcpspFile << std::endl << "I4:" << rcpsp.I4();
		rcpspFile << std::endl << "I5:" << rcpsp.I5();
		rcpspFile << std::endl << "I6:" << rcpsp.I6();

		//

		rcpspFile << std::endl << "Resource Factor:" << rcpsp.CalculateResourceFactor();
		for (int resource = 0; resource < rcpsp.GetResourcesCount(); resource++)
			rcpspFile << std::endl << "Resource " << resource << " Strength:" << rcpsp.CalculateResourceStrength(resource);

		for (int resource = 0; resource < rcpsp.GetResourcesCount(); resource++)
			rcpspFile << std::endl << "Resource " << resource << " Constrainedness:" << rcpsp.CalculateResourceConstrainedness(resource);

		rcpspFile.close();
	}
	return 0;
}

