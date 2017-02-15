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

class RCPSP{
	private:
		int m_LevelsCount;
		int m_ResourcesCount;
		int m_ActivitiesCount;
		std::vector<int> m_ActivitiesDuration;
		std::vector<int> m_ResourcesStock;
		std::vector<std::vector<int>> m_ActivitiesSuccessors;
		std::vector<std::vector<int>> m_ActivitiesPredecessors;
		std::vector<std::vector<int>> m_ActivitiesResourceConsumption;
		std::vector<std::vector<int>> m_LevelsActivities;
		std::vector<std::vector<int>> m_LevelsActivitiesNoDuplicates;
		std::vector<int> m_ActivitiesLevel;
	private:
	void CalculatePredecessors();
	int GetRootActivity();
	public:
	RCPSP(std::string pattersonFilename);
};

void RCPSP::CalculatePredecessors(){
	//Allocate memory for predecessors
	this->m_ActivitiesPredecessors.resize(this->m_ActivitiesCount);
	//note : Acitivity ID starts at 1
	int activityId = 1;
	for(auto successors = this->m_ActivitiesSuccessors.begin();successors!=this->m_ActivitiesSuccessors.end();successors++){
		for(auto successorActivity = successors->begin();successorActivity!=successors->end();successorActivity++){
			this->m_ActivitiesPredecessors.at((*successorActivity)-1).push_back(activityId);
		}
		activityId++;
	}
}

int RCPSP::GetRootActivity(){
	int i=0;
	for(auto predecessors = this->m_ActivitiesPredecessors.begin();predecessors!=this->m_ActivitiesPredecessors.end();predecessors++){
		i++;
		if(predecessors->size()==0)
			return i;
	}
	return -1;
}

RCSP::CalculateLevelRecursive(std::vector<int> successors,int level){
	if(this->m_LevelsCount<level)
		this->m_LevelsCount = level+1;
	if(this->levelsActivities.size()<level+1){
		this->levelsActivities.resize(level+1);
	}
	for(auto activity=successors.begin();activity!=successors.end();activity++){
		if(level>this->m_ActivitiesLevel[*activity])
			this->m_ActivitiesLevel[*activity]=level;
		// prevent from adding activity more than one time to the level activities vector
		// notice that still we may have the activity in another level acitivities vector		
		if(! std::find(this->m_levelsActivities[level].begin(),this->m_levelsActivities[level].end(),activity))
			this->m_LevelsActivities[level].push_back(activity);
		this->CalculateLevelRecursive(this->ActivitiesSuccessors[*activity],level+1);
	}
}

/*
Calculate levels for activities and activities of each level
*/
RCSP::CalculateLevelsTotal(){
	//Root activity is the activity at level 0 that have no predecessors
	int rootActivity = GetZeroLevelActivity();
	if(rootActivity==-1){
		throw std::exception("no root activty found for the patterson matrix");
	}
	this->m_ActivitiesLevel[rootActivity] = 0;
	this->m_LevelsActivities.resize(1);
	this->m_LevelsCount = 1;
	this->m_LevelsActivities[0].push_back(rootActivity);
	int level = 1;
	std::vector<int> successors = this->m_ActivitiesSuccessors[rootActivity];
	CalculateLevelRecursive(successors,level);
	//categorize activities by level
	int activity = 0;
	this->m_LevelsActovitiesNoDuplicates.resize(this->LevelsCount);
	for(auto level = this->m_ActivitiesLevel.begin();level == this->m_ActivitiesLevel.end();level++){		
		this->m_LevelsActivitiesNoDuplicates[*level] = activity;
		activity++;
	}
}


RCPSP::RCPSP(std::string pattersonFilename){
	std::ifstream pattersonFile;
	pattersonFile.open(pattersonFilename);
	if(! pattersonFile){
		throw pattersonFilename + " file not found";
	}
	pattersonFile >> this->m_ActivitiesCount;
	pattersonFile >> this->m_ResourcesCount;

	this->m_ActivitiesDuration.resize(this->m_ActivitiesCount);
	this->m_ResourcesStock.resize(this->m_ResourcesCount);

	for(int resource=0;resource<this->m_ResourcesCount;resource++){
		pattersonFile >> this->m_ResourcesStock.at(resource);
	}
	for(int activity=0;activity<this->m_ActivitiesCount;activity++){
		//read activity duration
		pattersonFile >> this->m_ActivitiesDuration.at(activity);
		//Allocate memory for current activity resource consumption
		this->m_ActivitiesResourceConsumption.resize(this->m_ResourcesCount);
		//read acitvity's each resource consumption
		for(int resource=0;resource>this->m_ResourcesCount;resource++){
			pattersonFile >> this->m_ActivitiesResourceConsumption.at(activity).at(resource);
		}
		//read successors
		int successorActivitiesCount = 0;
		pattersonFile >> successorActivitiesCount;
		//allocate memory for successor activities
		this->m_ActivitiesSuccessors.resize(successorActivitiesCount);
		for(int successorActivity = 0;successorActivity < successorActivitiesCount;successorActivity++){
			pattersonFile >> this->m_ActivitiesSuccessors.at(activity).at(successorActivity);
		}
	}
	pattersonFile.close();

	CalculatePredecessors();
	CalculateLevelsTotal();
}

float RCPSP::getActivityResourceConsumption(int activity){
	return this->m_ActivitiesResourceConsuption[activity];
}


std::vector<int> RCPSP::getLevelActivities(int level){
	return this->m_LevelsActivitiesNoDuplicates[level];
}


int getResourceStock(int resource){
	return this->m_ResourcesStocks[resource];
}

/*
	w(k,a) = R(k) - Sigma( (j belongs to a) r(j,k)
	k is resource
	R(k) is stock of resource k
	j is activity
	a is level
	r(j,k) is the amount of consumption of activity j from resource k

	returns : W(a,k)
*/
int RCPSP::computeResourceConsumptionAtSpecifiedLevel(int level,int resource){	
	int sigma = 0;
	std::vector<int> activities = getLevelActivities(level);
	for(auto activity=activities.begin();activity!=activities.end();activity++){		
		sigma += getActivityResourceConsumption(*activity); 
	}
	return getResourceStock(resource) - sigma;
}


float RCPSP::computeEta(int resource){
	float sigma = 0;
	int levelsCount = this->m_LevelsCount;
	for(auto level = 0;level < levelsCount;level++){
		sigma += std::fabs(computeResourceConsumptionAtSpecifiedLevel(level,resource));
	}
	return sigma / getLevelsCount();
}

float RCPSP::sgn(float num){
	if(num>0)
		return 1;
	else
		return 0;
}

float RCPSP::computeKeiP(){
	std::vector<int> resources = getResources();
	float sigma = 0;
	for(auto resource=resources.begin();resource!=resources.end();resource++){
		sigma += sgn(computeEta(*resource));
	}
	return sigma;
}

float RCPSP::computeTao(){
	float sigma = 0;
	std::vector<int> resources = getResources();
	for(auto resource = resources.begin();resource!=resources.end();resource++){
		sigma += computeEta(*resource);
	}
	return sigma / computeKeiP();
}


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
	TestComputeLevelsNumber();
	uint32_t i1 = 0; //Number of works
	float i2 = 0.0f; //Concurrency of works range from 0.0 to 1.0
	std::cout<<"Enter I1(number of works): ";
	std::cin>>i1;
	std::cout<<std::endl;
	std::cout<<"Enter I2(Concurreny constant)(0-1): ";
	std::cin>>i2;
	std::cout<<std::endl;
	return 0;
}

