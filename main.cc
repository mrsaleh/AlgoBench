/* vim: set tabstop=4 autoindent: */

#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <numeric>

using namespace std;

float ComputeDistributionOfActivities(std::vector<uint32_t> _levelsWidth,uint32_t _numberOfWorks){
	// Sigma(Wa,Wbar) a=1 to m / 2(m-1)(Wbar-1)
	float wBar = 0.0f;
	//Calculate wBar which is average of levels width
	wBar = accumulate(_levelsWidth.begin(),_levelsWidth.end(),0);
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
	cout<<"s:"<<_ConcurrencyIndicator * ((float)_numberOfWorks-1.0f)<<endl;
	return levelsNumber;
}

void TestComputeLevelsNumber(){
	uint32_t result = ComputeLevelsNumber(23,0.73f);
	if(17 != result){
		cout<<"ComputeLevelsNumber Test failed!"<<result<<endl;
		exit(1);
	}
	result = ComputeLevelsNumber(14,0.43f);
	if(7 != result){
		cout<<"ComputeLevelsNumber Test failed!"<<result<<endl;
		exit(1);
	}
	result = ComputeLevelsNumber(39,0.83f);
	if(33 != result){
		cout<<"ComputeLevelsNumber Test failed!"<<result<<endl;
		exit(1);
	}

	cout<<"ComputeLevelsNumber Test Passed!"<<endl;
}

int main(int argc,char * argv){
	TestComputeLevelsNumber();
	uint32_t i1 = 0; //Number of works
	float i2 = 0.0f; //Concurrency of works range from 0.0 to 1.0
	cout<<"Enter I1(number of works): ";
	cin>>i1;
	cout<<endl;
	cout<<"Enter I2(Concurreny constant)(0-1): ";
	cin>>i2;
	cout<<endl;
	return 0;
}

