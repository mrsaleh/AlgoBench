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


float ComputeDistributionOfActivities(std::vector<uint32_t> _levelsWidth,uint32_t _numberOfWorks){
	// Sigma(Wa,Wbar) a=1 to m / 2(m-1)(Wbar-1)
	float wBar = 0.0f;
	//Calculate wBar which is average of levels width
	wBar = std::accumulate(_levelsWidth.begin(),_levelsWidth.end(),0);
	wBar = wBar / static_cast<float>(_levelsWidth.size());
	//Calculate Standard Deviation
	float sigma = 0.0f;
	for(std::vector<uint32_t>::const_iterator it=_levelsWidth.begin();it!=_levelsWidth.end();it++){
		sigma += fabs(wBar - (*it));
	}
	sigma = sigma / (static_cast<float>(_numberOfWorks-1)*(wBar - 1.0f));
	return sigma;
}

uint32_t ComputeLevelsNumber(uint32_t _numberOfWorks,float _ConcurrencyIndicator){
	// I2 = (m-1) / (n-1) where m is levels number and n is number of works
	//so (m-1) = I2 * (n-1) -> m = (I2 * (n-1)) + 1
	uint32_t levelsNumber =round( (_ConcurrencyIndicator * ((float)_numberOfWorks - 1.0f)) + 1.0f);
	std::cout<<"s:"<<_ConcurrencyIndicator * ((float)_numberOfWorks-1.0f)<<std::endl;
	return levelsNumber;
}

void TestComputeLevelsNumber(){
	uint32_t result = ComputeLevelsNumber(23,0.73f);
	if(17 != result){
		std::cout<<"ComputeLevelsNumber Test failed!"<<result<<std::endl;
		std::exit(1);
	}
	result = ComputeLevelsNumber(14,0.43f);
	if(7 != result){
		std::cout<<"ComputeLevelsNumber Test failed!"<<result<<std::endl;
		std::exit(1);
	}
	result = ComputeLevelsNumber(39,0.83f);
	if(33 != result){
		std::cout<<"ComputeLevelsNumber Test failed!"<<result<<std::endl;
		std::exit(1);
	}

	std::cout<<"ComputeLevelsNumber Test Passed!"<<std::endl;
}

int main(int argc,char * argv){
	uint32_t lastFileId = 0;
	std::cout<< "Enter last file index(first file name supposed to be 1):";
	std::cin >>lastFileId;
	for(int i=1;i<=lastFileId;i++){
		RCPSP rcpsp(std::to_string(lastFileId));
		float TAO  = rcpsp.ComputeTAO();
		std::ofstream rcpspFile;
		rcpspFile.open(std::to_string(lastFileId),std::ios::app);
		rcpspFile<<std::endl<<"TAO:"<<TAO;
		rcpspFile.close();
	}
	return 0;
}

