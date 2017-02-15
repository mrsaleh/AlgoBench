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
	level = 0;
	for(std::vector<int>::const_iterator predecessor = this->m_ActivitiesPredecessors[activity].begin();predecessors!=this->m_ActivitiesPredecessors[activity].end();predecessor++){
		if(this->m_ActivitiesLevel(*predecessor)==-1)
			return -1;		
		levelOfPredecessorActivity = this->m_ActivitiesLevel(*predecessor);
		if(levelOfPredecessorActivity+1>level)
			level = levelOfPredecessorActivity;	
	}
	return level;
}

void RCPSP::CaluclateLevels(){
	int rootActivity = GetRootActivity();
	this->m_ActivitiesLevel = std::vector<int>(this->m_ActivitiesCount,-1);
	this->m_ActivitiesLevel[rootActivity] = 0;
	
	//Find each activity level
	std::vector<int>::const_iterator resetIterator= this->m_ActivitiesLevel.begin();
	int resetActivity = 0;
	bool reset = false;
	int activity=0;
	std::vector<int>::const_iterator activityLevel = this->m_ActivitiesLevel.begin();
	do{
		activity = resetActivity;
		activityLevel = resetIterator;
		reset = false;
		for(;activityLevel != this->m_ActivitiesLevel.end();activityLevel++,activity++){
			if(*activityLevel!=-1)
				continue;
			*activityLevel = FindActivityLevel(activity);
			if(*activityLevel>this->m_LevelsCount){
				this->m_LevelsCount = *activityLevel;			
			}
			if(*activityLevel==-1){
				reset = true;
				resetActivity = activity;
				resetIterator = activityLevel;				
			}
		}
	}while(reset);
	//Categorizing activities by level
	this->m_LevelsActivities.resize(this->m_LevelsCount);
	for(int activity=0,std::vector<int>::const_iterator activityLevel=this->m_ActivitiesLevel;activityLevel!=this->m_ActivitiesLevel.end();activity++){
		this->m_LevelsActivities[*activityLevel].push_back(activity);
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
	CalculateLevels();
}

int RCPSP::GetActivityResourceConsumption(int activity,int resource){
	return this->m_ActivitiesResourceConsumption[activity][resource];
}


std::vector<int> RCPSP::GetLevelActivities(int level){
	return this->m_LevelsActivitiesNoDuplicates[level];
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

