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

int main(int argc,char * argv){
	uint32_t lastFileId = 0;
	std::cout<< "Program will search for files with .rcp extensions, from 1.rcp to {INDEX_YOU_SPECIFIED}.rcp file , and append TAO to the end of files, You better to backup your files first if you want to keep original files."<<std::endl;
	std::cout << "Enter last file index(first file name supposed to be 1):";
	std::cin >>lastFileId;
	for(int i=1;i<=lastFileId;i++){
		RCPSP rcpsp(std::to_string(i)+".rcp");		
		rcpsp.GeneratePredecessorsMatrix();
		std::cout<<"Predecessors Matrix"<<std::endl;
		rcpsp.PrintPredecessorsMatrix();
		std::ofstream rcpspFile;
	}
	return 0;
}

