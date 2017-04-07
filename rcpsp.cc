/* vim: set tabstop=4 autoindent: */
#include "rcpsp.hh"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <exception>
#include <iostream>

bool operator == (const RCPSP& lhs, const RCPSP& rhs) {
	if (lhs.m_ActivitiesCount != rhs.m_ActivitiesCount)
		return false;
	bool * checkedColumns = new bool[lhs.m_ActivitiesCount];
	for (int i = 0; i < lhs.m_ActivitiesCount; i++) {
		checkedColumns[i] = false;
	}

	for (int c = 0; c < lhs.m_ActivitiesCount; c++) {
		for (int j = 0; j < lhs.m_ActivitiesCount; j++) {
			if (checkedColumns[j])
				continue;
			//Check Column equality(c,j)
			for (int i = 0; i < lhs.m_ActivitiesCount; i++) {
				if (lhs.m_PredecessorsMatrix[i][c] == rhs.m_PredecessorsMatrix[i][j]) {
					checkedColumns[c] = true;
					break;
				}
			}
			if (checkedColumns[j])
				return false; // if there is at least one column in first one , that not exist in the other one , then they are not equal
		}
	}

	return true;
}

void RCPSP::GeneratePredecessorsMatrix() {
	int activityId = 1;
	// ROW , COL
	this->m_PredecessorsMatrix = new bool *[this->m_ActivitiesCount];
	for (int i = 0; i < this->m_ActivitiesCount; i++) {
		this->m_PredecessorsMatrix[i] = new bool[this->m_ActivitiesCount];
		for (int j = 0; j < this->m_ActivitiesCount; j++) {
			this->m_PredecessorsMatrix[i][j] = false;
		}
	}

	for (auto predecessors = this->m_ActivitiesPredecessors.begin(); predecessors != this->m_ActivitiesPredecessors.end(); predecessors++) {
		for (auto predecessorActivity = predecessors->begin(); predecessorActivity != predecessors->end(); predecessorActivity++) {
			this->m_PredecessorsMatrix[(*predecessorActivity) - 1][activityId - 1] = true;
		}
		activityId++;
	}
}

void RCPSP::PrintPredecessorsMatrix() {
	for (int i = 0; i < this->m_ActivitiesCount; i++) {
		for (int j = 0; j < this->m_ActivitiesCount; j++) {
			if (this->m_PredecessorsMatrix[i][j])
				std::cout << "1 ";
			else
				std::cout << "0 ";
		}
		std::cout << std::endl;
	}
}

void RCPSP::CalculatePredecessors() {
	//Allocate memory for predecessors
	this->m_ActivitiesPredecessors.resize(this->m_ActivitiesCount);
	//note : Acitivity ID starts at 1
	int activityId = 1;
	for (auto successors = this->m_ActivitiesSuccessors.begin(); successors != this->m_ActivitiesSuccessors.end(); successors++) {
		for (auto successorActivity = successors->begin(); successorActivity != successors->end(); successorActivity++) {
			this->m_ActivitiesPredecessors.at((*successorActivity) - 1).push_back(activityId);
		}
		activityId++;
	}
}

int RCPSP::GetRootActivity() {
	int i = 0;
	for (auto predecessors = this->m_ActivitiesPredecessors.begin(); predecessors != this->m_ActivitiesPredecessors.end(); predecessors++) {
		i++;
		if (predecessors->size() == 0)
			return i;
	}
	return -1;
}

int RCPSP::FindActivityLevel(int activity) {
	int level = 0;
	for (std::vector<int>::const_iterator predecessor = this->m_ActivitiesPredecessors[activity - 1].begin(); predecessor != this->m_ActivitiesPredecessors[activity - 1].end(); predecessor++) {
		if (this->m_ActivitiesLevel[(*predecessor) - 1] == -1)
			return -1;
		int levelOfPredecessorActivity = this->m_ActivitiesLevel[(*predecessor) - 1];
		if (levelOfPredecessorActivity + 1 > level)
			level = levelOfPredecessorActivity + 1;
	}
	return level;
}

void RCPSP::CalculateLevels() {
	this->m_ActivitiesLevel = std::vector<int>(this->m_ActivitiesCount, -1);

	//Find each activity level
	bool reset = false;
	int activity = 1;
	std::vector<int>::iterator activityLevel = this->m_ActivitiesLevel.begin();
	do {
		reset = false;
		activity = 1;
		activityLevel = this->m_ActivitiesLevel.begin();
		for (; activityLevel != this->m_ActivitiesLevel.end(); activityLevel++, activity++) {
			if (*activityLevel != -1)
				continue;
			*activityLevel = FindActivityLevel(activity);
			if (*activityLevel == -1) {
				reset = true;
			}
		}
	} while (reset);

	for (auto activityLevel = this->m_ActivitiesLevel.begin(); activityLevel != this->m_ActivitiesLevel.end(); activityLevel++) {
		if (this->m_LevelsCount < *activityLevel + 1) {
			this->m_LevelsCount = *activityLevel + 1;
		}
	}
	//Categorizing activities by level
	this->m_LevelsActivities.resize(this->m_LevelsCount);
	activity = 1;
	for (std::vector<int>::const_iterator activityLevel = this->m_ActivitiesLevel.begin(); activityLevel != this->m_ActivitiesLevel.end(); activity++, activityLevel++) {
		this->m_LevelsActivities[(*activityLevel)].push_back(activity);
	}
	//levels count used in fomula is without first and last nodes
	m_CorrectedLevelsCount = this->m_LevelsCount - 2;
}


RCPSP::RCPSP(std::string pattersonFilename) {
	std::ifstream pattersonFile;
	pattersonFile.open(pattersonFilename, std::ios::in);
	if (!pattersonFile) {
		throw pattersonFilename + " file not found";
	}
	pattersonFile >> this->m_ActivitiesCount;
	pattersonFile >> this->m_ResourcesCount;

	this->m_ActivitiesDuration.resize(this->m_ActivitiesCount);
	this->m_ResourcesStock.resize(this->m_ResourcesCount);
	this->m_ActivitiesSuccessors.resize(this->m_ActivitiesCount);
	this->m_ActivitiesResourceConsumption.resize(this->m_ActivitiesCount);

	for (int resource = 0; resource < this->m_ResourcesCount; resource++) {
		pattersonFile >> this->m_ResourcesStock.at(resource);
	}
	for (int activity = 0; activity < this->m_ActivitiesCount; activity++) {
		//read activity duration
		pattersonFile >> this->m_ActivitiesDuration.at(activity);
		//Allocate memory for current activity resource consumption
		this->m_ActivitiesResourceConsumption[activity].resize(this->m_ResourcesCount);
		//read acitvity's each resource consumption
		for (int resource = 0; resource < this->m_ResourcesCount; resource++) {
			pattersonFile >> this->m_ActivitiesResourceConsumption.at(activity).at(resource);
		}
		//read successors
		int successorActivitiesCount = 0;
		pattersonFile >> successorActivitiesCount;
		//allocate memory for successor activities
		this->m_ActivitiesSuccessors[activity].resize(successorActivitiesCount);
		for (int successorActivity = 0; successorActivity < successorActivitiesCount; successorActivity++) {
			pattersonFile >> this->m_ActivitiesSuccessors.at(activity).at(successorActivity);
		}
	}
	pattersonFile.close();
	m_CorrectedActivitiesCount = m_ActivitiesCount - 2; // ActivitiesCount used in formula is without first and last dummy nodes

	CalculatePredecessors();
	CalculateLevels();
	CalculateActivitiesEearliestStartRecursive();
	GeneratePredecessorsMatrix();
	//Values required for I1 to I6 indicators
	CalculateAverageOfLevelsWidths();
	CalculateNPrime();
	CalculateTotalNumberOfArcs();
}

int RCPSP::GetActivityResourceConsumption(int activity, int resource) {
	return this->m_ActivitiesResourceConsumption[activity - 1][resource];
}


std::vector<int> RCPSP::GetLevelActivities(int level) {
	return this->m_LevelsActivities[level];
}


int RCPSP::GetResourceStock(int resource) {
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
int RCPSP::ComputeResourceConstrainednesstSpecifiedLevel(int level, int resource) {
	int sigma = 0;
	std::vector<int> activities = this->GetLevelActivities(level);
	for (auto activity = activities.begin(); activity != activities.end(); activity++) {
		sigma += this->GetActivityResourceConsumption(*activity, resource);
	}
	int result = this->GetResourceStock(resource) - sigma;
	if (result >= 0)
		return 0;
	else
		return -result;
}


float RCPSP::ComputeEta(int resource) {
	float sigma = 0;
	for (auto level = 0; level < this->m_LevelsCount; level++) {
		sigma += static_cast<float>(this->ComputeResourceConstrainednesstSpecifiedLevel(level, resource));
	}
	return sigma / static_cast<float>(this->m_CorrectedLevelsCount);
}

float RCPSP::sgn(float num) {
	if (num > 0)
		return 1;
	else
		return 0;
}


float RCPSP::ComputeTAO() {
	float sigma = 0;
	int keiP = 0;
	for (int resource = 0; resource < this->m_ResourcesCount; resource++) {
		float eta = this->ComputeEta(resource);
		sigma += eta;
		if (eta > 0)
			keiP++;
	}
	return sigma / keiP;
}

/*
findes(int activity){
	if(activity==0)
		return activities_duration[0];
	else
	return sum(findes(predecessors))
}

*/

int RCPSP::FindActivityEarliestFinishRecursive(int activity) {


	if (activity == 1) {
		return (this->m_ActivitiesDuration[0]);
	}
	else {
		//Find max of predecessors duration
		int max = 0;
		for (auto predecessor = m_ActivitiesPredecessors[activity - 1].begin(); predecessor != m_ActivitiesPredecessors[activity - 1].end(); predecessor++) {
			int es = this->m_ActivitiesDuration[activity - 1] + FindActivityEarliestFinishRecursive(*predecessor);
			if (es > max)
				max = es;
		}
		return max;
	}
}

void RCPSP::CalculateActivitiesEearliestStartRecursive() {
	this->m_ActivitiesEarliestStart.resize(this->m_ActivitiesCount, -1);

	for (int activity = 1; activity <= this->m_ActivitiesCount; activity++) {
		this->m_ActivitiesEarliestStart[activity - 1] = FindActivityEarliestFinishRecursive(activity) - this->m_ActivitiesDuration[activity - 1];
	}
}

//Calculate ES for activities

int RCPSP::FindActivityEearliestStart(int activity) {
	int totalDuration = 0;
	for (std::vector<int>::const_iterator predecessor = this->m_ActivitiesPredecessors[activity - 1].begin(); predecessor != this->m_ActivitiesPredecessors[activity - 1].end(); predecessor++) {
		if (this->m_ActivitiesEarliestStart[(*predecessor) - 1] == -1)
			return -1;
		int durationOfPredecessorActivity = this->m_ActivitiesDuration[(*predecessor) - 1];
		totalDuration += durationOfPredecessorActivity;
	}
	if (totalDuration > this->m_ActivitiesEarliestStart[activity - 1]) {
		m_ActivitiesEarliestStart[activity - 1] = totalDuration;
		return -1;
	}
	return totalDuration;
}

void RCPSP::CalculateActivitiesEearliestStart() {
	this->m_ActivitiesEarliestStart = std::vector<int>(this->m_ActivitiesCount, -1);

	//Find each activity total duration
	bool reset = false;
	int activity = 1;
	std::vector<int>::iterator activityEarliestStart = this->m_ActivitiesEarliestStart.begin();
	do {
		reset = false;
		activity = 1;
		activityEarliestStart = this->m_ActivitiesEarliestStart.begin();
		for (; activityEarliestStart != this->m_ActivitiesEarliestStart.end(); activityEarliestStart++, activity++) {
			if (*activityEarliestStart != -1)
				continue;
			int p = FindActivityEearliestStart(activity);
			if (p == -1) {
				reset = true;
			}
		}
	} while (reset);
}

//Project duration : EF (Earliest Finish) of last activity

int RCPSP::ComputeResourceConstrainednesstSpecifiedTime(int time, int resource) {
	int sigma = 0;

	for (int activity = 0; activity < this->m_ActivitiesCount; activity++) {
		if (this->m_ActivitiesEarliestStart[activity] <= time &&  time < this->m_ActivitiesEarliestStart[activity] + this->m_ActivitiesDuration[activity]) {
			sigma += this->m_ActivitiesResourceConsumption[activity][resource];
		}
	}

	int result = this->GetResourceStock(resource) - sigma;
	if (result >= 0)
		return 0;
	else
		return -result;
}

float RCPSP::ComputeEarliestStartEta(int resource) {
	float sigma = 0;
	for (int time = 0; time <= this->GetProjectDuration(); time++) {
		sigma += static_cast<float>(this->ComputeResourceConstrainednesstSpecifiedTime(time, resource));
	}
	return sigma / static_cast<float>(this->GetProjectDuration());
}


int RCPSP::GetProjectDuration() {
	return this->m_ActivitiesEarliestStart[this->m_ActivitiesCount - 1] + this->m_ActivitiesDuration[this->m_ActivitiesCount - 1];
}

float RCPSP::ComputeEarliestStartTAO() {
	float sigma = 0;
	int keiP = 0;
	for (int resource = 0; resource < this->m_ResourcesCount; resource++) {
		float eta = this->ComputeEarliestStartEta(resource);
		sigma += eta;
		if (eta > 0)
			keiP++;
	}
	return sigma / keiP;
}

//Network size indicator
void RCPSP::CalculateI1() {
	this->m_I1 = static_cast<float>(this->m_CorrectedActivitiesCount);
}

//Serial or parallel indicator
void RCPSP::CalculateI2() {
	if (this->m_CorrectedActivitiesCount == 1)
		this->m_I2 = 1;
	else {
		this->m_I2 = static_cast<float>(this->m_CorrectedLevelsCount - 1) / static_cast<float>(this->m_CorrectedActivitiesCount - 1);
	}

}


//Its the W_Bar in formula
void RCPSP::CalculateAverageOfLevelsWidths() {
	this->m_AverageOfLevelsWidths = 0;
	int sigma = 0;
	for (int level = 1; level <= this->m_CorrectedLevelsCount; level++) {
		sigma += this->m_LevelsActivities[level].size();
	}
	this->m_AverageOfLevelsWidths = static_cast<float>(sigma) / this->m_CorrectedLevelsCount;
}

//Activity distribuation indicator
// m is levels count
// n is activities count
// a is level
// (Sigma(a=1,m)(|W(a) - W_bar|)) / (2 * (m-1) * (W_bar - 1))
//notice that we don't count begin and end node in folmula
void RCPSP::CalculateI3() {
	float sigma = 0;
	if (this->m_LevelsCount == 1 || this->m_LevelsCount == this->m_ActivitiesCount - 2)
		this->m_I3 = 0;
	else {
		for (int level = 1; level <= this->m_CorrectedLevelsCount; level++) {
			//notice that we minus activities count with 2 to remove virtual nodes effect (begin node and end node)
			sigma += abs((this->m_LevelsActivities[level].size()) /* W(a) */ - this->m_AverageOfLevelsWidths) /*W_bar*/;
		}
		this->m_I3 = static_cast<float>(sigma) / static_cast<float>(2 * (this->m_CorrectedLevelsCount - 1) * (this->m_AverageOfLevelsWidths - 1));
	}
}


//D= Sigma(a=1,m-1)(W(a),W_bar)
//Maximum possible arcs with length of 1
int RCPSP::D() {
	int sigma = 0;
	for (int level = 1; level <= this->m_CorrectedLevelsCount - 1; level++) {
		sigma += (this->m_LevelsActivities[level].size()) * (this->m_LevelsActivities[level + 1].size());
	}
	return sigma;
}


// n_prime is the count of predecessor relation with length of arcLength
int RCPSP::CalculateArcsWithLength(int arcLength) {
	int arcCount = 0;
	for (int activity = 1; activity <= this->m_CorrectedActivitiesCount - 1; activity++) {
		int activityLevel = this->m_ActivitiesLevel[activity];
		for (auto successor = this->m_ActivitiesSuccessors[activity].begin(); successor != this->m_ActivitiesSuccessors[activity].end(); successor++) {
			int successorLevel = this->m_ActivitiesLevel[*successor - 1];
			if (successorLevel - activityLevel == arcLength) {
				arcCount++;
			}
		}
	}
	return arcCount;
}

void RCPSP::CalculateNPrime() {
	this->m_NPrime = CalculateArcsWithLength(1);
}



//Short arc indicator
//n is activities count
void RCPSP::CalculateI4() {

	float i4 = 0;
	auto w1 = this->m_ActivitiesLevel[1];
	// 1 if D = n - w1
	//else
	// (n_prime -  n + w1) / (D - n + w1)
	if (D() == this->m_CorrectedActivitiesCount - this->m_LevelsActivities[1].size()/* w1  */) {
		i4 = 1;
	}
	else {
		i4 = static_cast<float>(this->m_NPrime - this->m_CorrectedActivitiesCount /* n */ + this->m_LevelsActivities[1].size())/* w1  */ / (D() - this->m_CorrectedActivitiesCount  /* w1 */ + this->m_LevelsActivities[1].size());
		std::cout << "D ->" << D() << std::endl;
		std::cout << "this->m_LevelsActivities[1].size() ->" << this->m_LevelsActivities[1].size() << std::endl;
		std::cout << "this->m_CorrectedActivitiesCount ->" << this->m_CorrectedActivitiesCount << std::endl;
		system("pause");
	}
	this->m_I4 = i4;
}

/* |A| is total number of arcs*/
void RCPSP::CalculateTotalNumberOfArcs() {
	int a = 0;
	
	for (int activity = 1; activity <= this->m_CorrectedActivitiesCount; activity++) {
		for (auto predecessor = this->m_ActivitiesPredecessors[activity].begin(); predecessor != this->m_ActivitiesPredecessors[activity].end(); predecessor++) {
			if (*predecessor == 1 )
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
void RCPSP::CalculateI5() {
	//1 if |A| = n - w1
	int sigma = 0;
	for (int level = 1; level < this->m_CorrectedLevelsCount - 1; level++) {
		sigma += (CalculateArcsWithLength(level) * (this->m_CorrectedLevelsCount - level - 1)) / (this->m_CorrectedLevelsCount - 2);
	}
	sigma += -this->m_CorrectedActivitiesCount + this->m_LevelsActivities[1].size();
	this->m_I5 = sigma / static_cast<float>(this->m_TotalNumberOfArcs - this->m_CorrectedActivitiesCount + this->m_LevelsActivities[1].size());
}


