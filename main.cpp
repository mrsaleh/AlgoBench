/* vim: set tabstop=4 autoindent: */

#include <iostream>

using namespace std;

int main(int argc,char * argv){
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

