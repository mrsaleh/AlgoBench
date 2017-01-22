#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MB 1024*1024;

#define MAX_FILE_SIZE 8 * MB;

#define IS_NULL(x) (x==0) 

bool readFileContent(const char * filename,char * buffer,int bufferSize,int * readBytesCount){
	//Parameters Validation
	if(ISNULL(filename)){
		printf("error:filename is NULL \n");
		return false;
	}

	if(IS_NULL(buffer)){
		printf("error:buffer is NULL \n");
		return false;
	}

	if(IS_NULL(readBytesCount))
	{
		printf("error:readBytesCount is NULL \n");
		return false;
	}

	//Read file

	FILE * fin = 0;
	fin = fopen(filename,"rb");
	if(fin=0){
		printf("error:could not open the file ->  %s \n",filename);
		return false;
	}

	fseek(fp, 0, SEEK_END); 
	long filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	fread(buffer,1,filesize,fin);

	return true;
}

#define PARSE_STATE_READING_ACTIVITIES_COUNT 10;
#define PARSE_STATE_READING_RESOURCES_COUNT 11;



bool parsePatterson(const char * patterson){
	int i=0;
	int state = PARSE_STATE_READING_ACTIVITIES_COUNT ;
	while(! IS_NULL(patterson[i])){
		char c = patterson[i];
		if(
		//TODO : replace atoi with a safer function
		
	}

}


int main(int argc,char** argv){
	std::cout<< argc<<std::endl;
	if(argc>=2){
		for(int argi=1;argi<argc;argi++){
			std::string pat_file = argvp[1];			
		}
	}
		
}
