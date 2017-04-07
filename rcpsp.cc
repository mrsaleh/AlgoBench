/* vim: set tabstop=4 autoindent: */
#include "rcpsp.hh"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <exception>
#include <iostream>

bool operator == (const RCPSP& lhs,const RCPSP& rhs){
	if(lhs.m_ActivitiesCount!=rhs.m_ActivitiesCount)
		return false;
	bool * checkedColumns = new bool[lhs.m_ActivitiesCount];
	for(int i=0;i<lhs.m_ActivitiesCount;i++){
		checkedColumns[i] = false;
	}
	
	for(int c=0;c<lhs.m_ActivitiesCount;c++){
		for(int j=0;j<lhs.m_ActivitiesCount;j++){
			if(checkedColumns[j])
				continue;
			//Check Column equality(c,j)
			for(int i=0;i<lhs.m_ActivitiesCount;i++){
				if(lhs.m_PredecessorsMatrix[i][c] == rhs.m_PredecessorsMatrix[i][j]){
					checkedColumns[c] = true;
					break;					
				}
			}
			if(checkedColumns[j])
				return false; // if there is at least one column in first one , that not exist in the other one , then they are not equal
		}
	}

	return true;
}

void RCPSP::GeneratePredecessorsMatrix(){
	int activityId = 1;
	// ROW , COL
	this->m_PredecessorsMatrix = new bool *[this->m_ActivitiesCount];
	for(int i=0;i<this->m_ActivitiesCount;i++){
		this->m_PredecessorsMatrix[i] = new bool[this->m_ActivitiesCount];
		for(int j=0;j<this->m_ActivitiesCount;j++){
			this->m_PredecessorsMatrix[i][j] = false;
		}
	}

	for(auto predecessors = this->m_ActivitiesPredecessors.begin();predecessors!=this->m_ActivitiesPredecessors.end();predecessors++){
		for(auto predecessorActivity = predecessors->begin();predecessorActivity!=predecessors->end();predecessorActivity++){
			this->m_PredecessorsMatrix[(*predecessorActivity)-1][activityId-1] = true;
		}
		activityId++;
	}
}

void RCPSP::PrintPredecessorsMatrix(){
	for(int i=0;i<this->m_ActivitiesCount;i++){
		for(int j=0;j<this->m_ActivitiesCount;j++){
			if(this->m_PredecessorsMatrix[i][j])
				std::cout<<"1 ";
			else
				std::cout<<"0 ";
		}
		std::cout<<std::endl;
	}
}

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
	GeneratePredecessorsMatrix();
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
	int result = this->GetResourceStock(resource) - sigma;
	if(result>0)
		return 0;
	else
		return static_cast<int>(fabs(result));
}


float RCPSP::ComputeEta(int resource){
	float sigma = 0;
	for(auto level = 0;level < this->m_LevelsCount;level++){
		sigma += static_cast<float>(std::fabs(this->ComputeResourceConsumptionAtSpecifiedLevel(level,resource)));
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

//Network size indicator
void RCPSP::CalculateI1(){
	this->m_I1 = static_cast<float>(this->m_ActivitiesCount);
}

//Serial or parallel indicator
void RCPSP::CalculateI2(){
	if(this->m_ActivitiesCount==1)
		this->m_I2 = 1;
	else{
		this->m_I2 = static_cast<float>(this->m_LevelsCount - 1) / static_cast<float>(this->m_ActivitiesCount - 1);
	}

}


//Its the W_Bar in formula
void RCPSP::CalculateAverageOfLevelsWidths(){
	this->m_AverageOfLevelsWidths = 0;
	int sigma = 0;
	for(int level=0;level<this->m_LevelsCount;level++){
		sigma = this->m_LevelsActivities[level].size() - 2;
	}	
	this->m_AverageOfLevelsWidths = static_cast<float>(sigma) / this->m_LevelsCount;
}

//Activity distribuation indicator
// m is levels count
// n is activities count
// a is level
// (Sigma(a=1,m)(|W(a) - W_bar|)) / (2 * (m-1) * (W_bar - 1))
//notice that we don't count begin and end node in folmula
void RCPSP::CalculateI3(){
	float sigma = 0;
	if(this->m_LevelsCount==1 || this->m_LevelsCount==this->m_ActivitiesCount - 2)
		this->m_I3 = 0;
	else{
		for(int level=1;level<this->m_LevelsCount-1;level++){
			//notice that we minus activities count with 2 to remove virtual nodes effect (begin node and end node)
			sigma += abs((this->m_LevelsActivities[level].size()) /* W(a) */ - this->m_AverageOfLevelsWidths) /*W_bar*/;
		}
		this->m_I3 = static_cast<float>(sigma) / static_cast<float>(2 * (this->m_LevelsCount - 1) * (this->m_AverageOfLevelsWidths - 1));
	}
}


//D= Sigma(a=1,m-1)(W(a),W_bar)
//Maximum possible arcs with length of 1
int RCPSP::D(){
	int sigma = 0;
	for(int level=1;level<this->m_LevelsCount-1;level++){
		sigma = (this->m_LevelsActivities[level].size()) * (this->m_LevelsActivities[level+1].size());
	}		
	return sigma;
}


// n_prime is the count of predecessor relation with length of one
int RCPSP::CalculateArcsWithLength(int arcLength){
	int arcCount = 0;
	for(int activity=1;activity<this->m_ActivitiesCount-1;activity++){
		int activityLevel = this->m_ActivitiesLevel[activity];
		for(auto successor = this->m_ActivitiesSuccessors[activity].begin();successor!=this->m_ActivitiesSuccessors[activity].end();successor++){			
			int successorLevel = this->m_ActivitiesLevel[*successor];
			if(successorLevel - activityLevel == arcLength){
				arcCount ++;
			}
		}
	}
	return arcCount;
}

void RCPSP::CalculateNPrime(){
	this->m_NPrime = CalculateArcsWithLength(1);
}



//Short arc indicator
//n is activities count
void RCPSP::CalculateI4(){
	float i4 = 0;
	auto w1 = this->m_ActivitiesLevel[1];
	// 1 if D = n - w1
	//else
	// (n_prime -  n + w1) / (D - n + w1)
	if(D() == this->m_ActivitiesCount - this->m_LevelsActivities[1].size()/* w1  */){
		i4 = static_cast<float>(this->m_NPrime - this->m_ActivitiesCount /* n */ + this->m_LevelsActivities[1].size())/* w1  */  / (D() -  + this->m_ActivitiesCount  /* w1 */ + this->m_LevelsActivities[1].size());
	}
	this->m_I4 = i4;
}

/* |A| is total number of arcs*/
void RCPSP::CalculateTotalNumberOfArcs(){
	int a = 0;
	int activity = -1;
	for(auto activityPredecessors=this->m_ActivitiesPredecessors.begin();activityPredecessors!=this->m_ActivitiesPredecessors.end();activityPredecessors++){
		activity++;
		if(activity==0)
			continue;
		if(activity==this->m_ActivitiesCount-1)
			continue;
		for(auto predecessor=activityPredecessors->begin();predecessor!=activityPredecessors->end();predecessor++){
			if(*predecessor ==0 || *predecessor==this->m_ActivitiesCount-1)
				continue;
			a++;
		}
	}
	this->m_TotalNumberOfArcs = a;
}

//Long arc indicator
//n is activities count
//w1 is width of level one
//m is levels count
//Resource Constrained Project Scheduling Problem
void RCPSP::CalculateI5(){
	//1 if |A| = n - w1
	int sigma = 0;
	for(int level=1;level<this->m_LevelsCount-1;level++){
		sigma = CalculateArcsWithLength(level) * (this->m_LevelsCount-level-1) / (this->m_LevelsCount-2);
	}
	sigma += CalculateArcsWithLength(1) - this->m_ActivitiesCount + this->m_LevelsActivities[1].size();
	this->m_I5 = sigma / static_cast<float>( this->m_TotalNumberOfArcs - this->m_ActivitiesCount + this->m_LevelsActivities[1].size());
}


