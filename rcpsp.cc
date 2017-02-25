#include "rcpsp.hh"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <exception>

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

int RCPSP::FindActivityLevel(int activity){
	int level = 0;
	for(std::vector<int>::const_iterator predecessor = this->m_ActivitiesPredecessors[activity-1].begin();predecessor !=this->m_ActivitiesPredecessors[activity-1].end();predecessor++){
		if(this->m_ActivitiesLevel[(*predecessor)-1]==-1)
			return -1;		
		int levelOfPredecessorActivity = this->m_ActivitiesLevel[(*predecessor)-1];
		if(levelOfPredecessorActivity+1>level)
			level = levelOfPredecessorActivity+1;	
	}
	return level;
}

void RCPSP::CalculateLevels(){
	int rootActivity = GetRootActivity();
	this->m_ActivitiesLevel = std::vector<int>(this->m_ActivitiesCount,-1);
	//this->m_ActivitiesLevel[rootActivity-1] = 0;
	
	//Find each activity level
	bool reset = false;
	int activity=1;
	std::vector<int>::iterator activityLevel = this->m_ActivitiesLevel.begin();
	do{
		reset = false;
		activity = 1;
		activityLevel = this->m_ActivitiesLevel.begin();
		for(;activityLevel != this->m_ActivitiesLevel.end();activityLevel++,activity++){
			if(*activityLevel!=-1)
				continue;
			*activityLevel = FindActivityLevel(activity);
			if (*activityLevel == -1) {
				reset = true;
			}			
		}
	}while(reset);
	
	for (auto activityLevel = this->m_ActivitiesLevel.begin();activityLevel != this->m_ActivitiesLevel.end();activityLevel++) {
		if (this->m_LevelsCount < *activityLevel+1) {
			this->m_LevelsCount = *activityLevel + 1;
		}
	}
	//Categorizing activities by level
	this->m_LevelsActivities.resize(this->m_LevelsCount);
	activity = 1;
	for(std::vector<int>::const_iterator activityLevel=this->m_ActivitiesLevel.begin();activityLevel!=this->m_ActivitiesLevel.end();activity++, activityLevel++){
		this->m_LevelsActivities[(*activityLevel)].push_back(activity);
	}
}


RCPSP::RCPSP(std::string pattersonFilename){
	std::ifstream pattersonFile;
	pattersonFile.open(pattersonFilename,std::ios::in);
	if(! pattersonFile){
		throw pattersonFilename + " file not found";
	}
	pattersonFile >> this->m_ActivitiesCount;
	pattersonFile >> this->m_ResourcesCount;

	this->m_ActivitiesDuration.resize(this->m_ActivitiesCount);
	this->m_ResourcesStock.resize(this->m_ResourcesCount);
	this->m_ActivitiesSuccessors.resize(this->m_ActivitiesCount);
	this->m_ActivitiesResourceConsumption.resize(this->m_ActivitiesCount);

	for(int resource=0;resource<this->m_ResourcesCount;resource++){
		pattersonFile >> this->m_ResourcesStock.at(resource);
	}
	for(int activity=0;activity<this->m_ActivitiesCount;activity++){
		//read activity duration
		pattersonFile >> this->m_ActivitiesDuration.at(activity);
		//Allocate memory for current activity resource consumption
		this->m_ActivitiesResourceConsumption[activity].resize(this->m_ResourcesCount);
		//read acitvity's each resource consumption
		for(int resource=0;resource<this->m_ResourcesCount;resource++){
			pattersonFile >> this->m_ActivitiesResourceConsumption.at(activity).at(resource);
		}
		//read successors
		int successorActivitiesCount = 0;
		pattersonFile >> successorActivitiesCount;
		//allocate memory for successor activities
		this->m_ActivitiesSuccessors[activity].resize(successorActivitiesCount);
		for(int successorActivity = 0;successorActivity < successorActivitiesCount;successorActivity++){
			pattersonFile >> this->m_ActivitiesSuccessors.at(activity).at(successorActivity);
		}
	}
	pattersonFile.close();

	CalculatePredecessors();
	CalculateLevels();
}

int RCPSP::GetActivityResourceConsumption(int activity,int resource){
	return this->m_ActivitiesResourceConsumption[activity-1][resource];
}


std::vector<int> RCPSP::GetLevelActivities(int level){
	return this->m_LevelsActivities[level];
}


int RCPSP::GetResourceStock(int resource){
	return this->m_ResourcesStock[resource];
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
int RCPSP::ComputeResourceConsumptionAtSpecifiedLevel(int level,int resource){	
	int sigma = 0;
	std::vector<int> activities = this->GetLevelActivities(level);
	for(auto activity=activities.begin();activity!=activities.end();activity++){		
		sigma += this->GetActivityResourceConsumption(*activity,resource); 
	}
	return this->GetResourceStock(resource) - sigma;
}


float RCPSP::ComputeEta(int resource){
	float sigma = 0;
	for(auto level = 0;level < this->m_LevelsCount;level++){
		sigma += std::fabs(this->ComputeResourceConsumptionAtSpecifiedLevel(level,resource));
	}
	return sigma / this->m_LevelsCount;
}

float RCPSP::sgn(float num){
	if(num>0)
		return 1;
	else
		return 0;
}

float RCPSP::ComputeKeiP(){
	float sigma = 0;
	for(int resource=0;resource<this->m_ResourcesCount;resource++){
		sigma += this->sgn(this->ComputeEta(resource));
	}
	return sigma;
}

float RCPSP::ComputeTAO(){
	float sigma = 0;
	for(int resource = 0;resource<this->m_ResourcesCount;resource++){
		sigma += this->ComputeEta(resource);
	}
	return sigma / this->ComputeKeiP();
}

