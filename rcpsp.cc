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

int RCPSP::FindActivityRegressiveLevel(int activity) {
	if (activity == this->m_ActivitiesCount - 1)
		return this->m_ActivitiesProgressiveLevel[this->m_ActivitiesCount - 1];
	int level = 0;
	for (auto successor = this->m_ActivitiesSuccessors[activity].begin(); successor != this->m_ActivitiesSuccessors[activity].end(); successor++) {
		int level_tmp = FindActivityRegressiveLevel(*successor - 1);
		if (successor == this->m_ActivitiesSuccessors[activity].begin()) {
			level = level_tmp;
			continue;
		}
		if (level_tmp < level)
			level = level_tmp;
	}

	return level - 1;

}

//

void RCPSP::CalculateRegressiveLevels() {
	this->m_ActivitiesRegressiveLevel.resize(this->m_ActivitiesCount);
	for (int activity = 0; activity < this->m_ActivitiesCount; activity++) {
		this->m_ActivitiesRegressiveLevel[activity] = FindActivityRegressiveLevel(activity);
	}
}

//Calculate Progressive Level

int RCPSP::FindActivityProgressiveLevel(int activity) {
	int level = 0;
	for (std::vector<int>::const_iterator predecessor = this->m_ActivitiesPredecessors[activity - 1].begin(); predecessor != this->m_ActivitiesPredecessors[activity - 1].end(); predecessor++) {
		if (this->m_ActivitiesProgressiveLevel[(*predecessor) - 1] == -1)
			return -1;
		int levelOfPredecessorActivity = this->m_ActivitiesProgressiveLevel[(*predecessor) - 1];
		if (levelOfPredecessorActivity + 1 > level)
			level = levelOfPredecessorActivity + 1;
	}
	return level;
}

void RCPSP::CalculateProgressiveLevels() {
	this->m_ActivitiesProgressiveLevel = std::vector<int>(this->m_ActivitiesCount, -1);

	//Find each activity level
	bool reset = false;
	int activity = 1;
	std::vector<int>::iterator activityLevel = this->m_ActivitiesProgressiveLevel.begin();
	do {
		reset = false;
		activity = 1;
		activityLevel = this->m_ActivitiesProgressiveLevel.begin();
		for (; activityLevel != this->m_ActivitiesProgressiveLevel.end(); activityLevel++, activity++) {
			if (*activityLevel != -1)
				continue;
			*activityLevel = FindActivityProgressiveLevel(activity);
			if (*activityLevel == -1) {
				reset = true;
			}
		}
	} while (reset);

	for (auto activityLevel = this->m_ActivitiesProgressiveLevel.begin(); activityLevel != this->m_ActivitiesProgressiveLevel.end(); activityLevel++) {
		if (this->m_LevelsCount < *activityLevel + 1) {
			this->m_LevelsCount = *activityLevel + 1;
		}
	}
	//Categorizing activities by level
	this->m_ProgressiveLevelsActivities.resize(this->m_LevelsCount);
	activity = 1;
	for (std::vector<int>::const_iterator activityLevel = this->m_ActivitiesProgressiveLevel.begin(); activityLevel != this->m_ActivitiesProgressiveLevel.end(); activity++, activityLevel++) {
		this->m_ProgressiveLevelsActivities[(*activityLevel)].push_back(activity);
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
	CalculateProgressiveLevels();
	//Regressive levels calculation require progressive levels calculation
	CalculateRegressiveLevels();
	CalculateActivitiesEearliestStartRecursive();
	GeneratePredecessorsMatrix();
	//Values required for I1 to I6 indicators
	CalculateAverageOfLevelsWidths();
	CalculateNPrime();
	CalculateTotalNumberOfArcs();
}


std::vector<int> RCPSP::GetLevelActivities(int level) {
	return this->m_ProgressiveLevelsActivities[level];
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
		sigma += this->m_ActivitiesResourceConsumption[*activity - 1][resource];
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

int RCPSP::FindActivityLatestFinishRecursive(int activity) {


	if (activity == this->m_ActivitiesCount-1) {
		return (FindActivityEarliestFinishRecursive(activity));
	}
	else {
		//Find min of successors duration
		int min = FindActivityEarliestFinishRecursive(this->m_ActivitiesCount - 1);
		for (auto successor = m_ActivitiesSuccessors[activity - 1].begin(); successor != m_ActivitiesSuccessors[activity - 1].end(); successor++) {
			int lf = FindActivityLatestFinishRecursive(*successor) - this->m_ActivitiesDuration[activity - 1];
			if (lf < min)
				min = lf;
		}
		return min;
	}
}

void RCPSP::CalculateActivitiesLatestStartRecursive() {
	this->m_ActivitiesLatestStart.resize(this->m_ActivitiesCount, -1);

	for (int activity = 1; activity <= this->m_ActivitiesCount; activity++) {
		this->m_ActivitiesLatestStart[activity - 1] = FindActivityLatestFinishRecursive(activity) - this->m_ActivitiesDuration[activity - 1];
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
float RCPSP::I1() {
	return static_cast<float>(this->m_CorrectedActivitiesCount);
}

//Serial or parallel indicator
float RCPSP::I2() {
	if (this->m_CorrectedActivitiesCount == 1)
		return 1;
	else {
		return static_cast<float>(this->m_CorrectedLevelsCount - 1) / static_cast<float>(this->m_CorrectedActivitiesCount - 1);
	}
}


//Its the W_Bar in formula
void RCPSP::CalculateAverageOfLevelsWidths() {
	this->m_AverageOfLevelsWidths = 0;
	int sigma = 0;
	for (int level = 1; level <= this->m_CorrectedLevelsCount; level++) {
		sigma += this->m_ProgressiveLevelsActivities[level].size();
	}
	this->m_AverageOfLevelsWidths = static_cast<float>(sigma) / this->m_CorrectedLevelsCount;
}

//Activity distribuation indicator
// m is levels count
// n is activities count
// a is level
// (Sigma(a=1,m)(|W(a) - W_bar|)) / (2 * (m-1) * (W_bar - 1))
//notice that we don't count begin and end node in folmula
float RCPSP::I3() {
	float sigma = 0;
	if (this->m_LevelsCount == 1 || this->m_LevelsCount == this->m_ActivitiesCount - 2)
		return 0;
	else {
		for (int level = 1; level <= this->m_CorrectedLevelsCount; level++) {
			//notice that we minus activities count with 2 to remove virtual nodes effect (begin node and end node)
			sigma += abs((this->m_ProgressiveLevelsActivities[level].size()) /* W(a) */ - this->m_AverageOfLevelsWidths) /*W_bar*/;
		}
		return static_cast<float>(sigma) / static_cast<float>(2 * (this->m_CorrectedLevelsCount - 1) * (this->m_AverageOfLevelsWidths - 1));
	}
}


//D= Sigma(a=1,m-1)(W(a),W_bar)
//Maximum possible arcs with length of 1
int RCPSP::D() {
	int sigma = 0;
	for (int level = 1; level <= this->m_CorrectedLevelsCount - 1; level++) {
		sigma += (this->m_ProgressiveLevelsActivities[level].size()) * (this->m_ProgressiveLevelsActivities[level + 1].size());
	}
	return sigma;
}


// n_prime is the count of predecessor relation with length of arcLength
int RCPSP::CalculateArcsWithLength(int arcLength) {
	int arcCount = 0;
	for (int activity = 1; activity <= this->m_CorrectedActivitiesCount - 1; activity++) {
		int activityLevel = this->m_ActivitiesProgressiveLevel[activity];
		for (auto successor = this->m_ActivitiesSuccessors[activity].begin(); successor != this->m_ActivitiesSuccessors[activity].end(); successor++) {
			int successorLevel = this->m_ActivitiesProgressiveLevel[*successor - 1];
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
float RCPSP::I4() {

	float i4 = 0;
	auto w1 = this->m_ActivitiesProgressiveLevel[1];
	// 1 if D = n - w1
	//else
	// (n_prime -  n + w1) / (D - n + w1)
	if (D() == this->m_CorrectedActivitiesCount - this->m_ProgressiveLevelsActivities[1].size()/* w1  */) {
		i4 = 1;
	}
	else {
		i4 = static_cast<float>(this->m_NPrime - this->m_CorrectedActivitiesCount /* n */ + this->m_ProgressiveLevelsActivities[1].size())/* w1  */ / (D() - this->m_CorrectedActivitiesCount  /* w1 */ + this->m_ProgressiveLevelsActivities[1].size());
	}
	return  i4;
}

/* |A| is total number of arcs*/
void RCPSP::CalculateTotalNumberOfArcs() {
	int a = 0;

	for (int activity = 1; activity <= this->m_CorrectedActivitiesCount; activity++) {
		for (auto predecessor = this->m_ActivitiesPredecessors[activity].begin(); predecessor != this->m_ActivitiesPredecessors[activity].end(); predecessor++) {
			if (*predecessor == 1)
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
float RCPSP::I5() {
	//1 if |A| = n - w1
	int sigma = 0;
	for (int level = 1; level < this->m_CorrectedLevelsCount - 1; level++) {
		sigma += (CalculateArcsWithLength(level) * (this->m_CorrectedLevelsCount - level - 1)) / (this->m_CorrectedLevelsCount - 2);
	}
	sigma += -this->m_CorrectedActivitiesCount + this->m_ProgressiveLevelsActivities[1].size();
	return ( sigma / static_cast<float>(this->m_TotalNumberOfArcs - this->m_CorrectedActivitiesCount + this->m_ProgressiveLevelsActivities[1].size()));
}

float RCPSP::I6() {
	if (this->m_CorrectedLevelsCount == 1 || this->m_CorrectedLevelsCount == this->m_CorrectedActivitiesCount)
		return 0;
	else {
		int sigma = 0;
		for (int activity = 1; activity <= this->m_CorrectedActivitiesCount; activity++) {
			sigma += this->m_ActivitiesRegressiveLevel[activity] - this->m_ActivitiesProgressiveLevel[activity];
		}
		return static_cast<float>(sigma) / static_cast<float>((this->m_CorrectedLevelsCount - 1) * (this->m_CorrectedActivitiesCount - this->m_CorrectedLevelsCount));
	}
}

float RCPSP::CalculateResourceFactor() {
	int sigma = 0;
	for (int activity = 1; activity <= this->m_CorrectedActivitiesCount; activity++) {
		for (int resource = 0; resource < this->m_ResourcesCount; resource++) {
			if (this->m_ActivitiesResourceConsumption[activity][resource] > 0)
				sigma++;
		}
	}

	return static_cast<float>(sigma) / static_cast<float>(this->m_ResourcesCount * this->m_CorrectedActivitiesCount);
}


float RCPSP::CalculateResourceConstrainedness(int resource) {
	float sigma = 0;
	int requiredActivities = 0;
	for (int activity = 1; activity <= this->m_CorrectedActivitiesCount; activity++) {
		if (m_ActivitiesResourceConsumption[activity][resource] > 0) {
			sigma += m_ActivitiesResourceConsumption[activity][resource];
			requiredActivities++;
		}
	}
	return (static_cast<float>(sigma) / static_cast<float>(requiredActivities)) / static_cast<float>(this->m_ResourcesStock[resource]);
}

float RCPSP::CalculateResourceStrength(int resource) {
	//Calculating Rk min
	int rkmin = 0;
	for (int activity = 1; activity <= this->m_CorrectedActivitiesCount; activity++) {
		if (m_ActivitiesResourceConsumption[activity][resource] > rkmin)
			rkmin = m_ActivitiesResourceConsumption[activity][resource];
	}
	//Calculate Rk max
	int rkmax = 0;
	for (int time = 0; time <= this->GetProjectDuration(); time++) {
		int sigma = 0;
		for (int activity = 0; activity < this->m_ActivitiesCount; activity++) {
			if (this->m_ActivitiesEarliestStart[activity] <= time &&  time < this->m_ActivitiesEarliestStart[activity] + this->m_ActivitiesDuration[activity]) {
				sigma += this->m_ActivitiesResourceConsumption[activity][resource];
			}
		}
		if (sigma > rkmax)
			rkmax = sigma;
	}
	return (static_cast<float>(this->m_ResourcesStock[resource]) - static_cast<float>(rkmin)) / (static_cast<float>(rkmax) - static_cast<float>(rkmin));
}

float random() {
	return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}

std::vector<int> RandomSelectSet(std::vector<int> set, int count) {
	std::vector<int> result;

	for (int i = 0; i < count; i++) {
		int randomItem = random() * set.size();
		result.push_back(set.at(randomItem));
		set.erase(set.begin() + randomItem);
	}
	return result;
}

template<typename T>
T RandomSelect(std::vector<T> set) {
	int randomItem = random() * set.size();
	T result = set.at(randomItem);
	return result;
}

class PrecedenceMatrix {
private:
	std::vector<std::vector<int>> m_ActivitiesPredecessors;
	int m_ActivitiesCount;
public:
	PrecedenceMatrix(int activities_count) {
		m_ActivitiesPredecessors.resize(activities_count);
		for (int activity = activities_count - 1; activity >= 0; activity--) {
			for (int predecessor = 0; predecessor < activity; predecessor++) {
				m_ActivitiesPredecessors[activity].push_back(predecessor);
			}
		}
	}

	void RandomReduction() {
		std::vector<int> activitiesThatHasEdge;
		for (int activity = 1; activity <= m_ActivitiesCount - 2; activity++) {
			if (m_ActivitiesPredecessors[activity].size() > 0)
				activitiesThatHasEdge.push_back(activity);
		}
		int randomActivity = RandomSelect<int>(activitiesThatHasEdge);
		int randomActivityPredecessorIndex = random() * m_ActivitiesPredecessors[randomActivity].size();
		m_ActivitiesPredecessors[randomActivity].erase(m_ActivitiesPredecessors[randomActivity].begin() + randomActivityPredecessorIndex);
	}	

};


void RCPSP::SaveToCPlexFile(std::string filename) {
	std::ofstream cplexFile(filename + ".cplex");
	cplexFile << this->m_ActivitiesCount << " " << this->m_ResourcesCount<<std::endl;
	//Print Resources Stock
	for (int i = 0; i < this->m_ResourcesStock.size(); i++) {
		cplexFile << this->m_ResourcesStock[i]<<" ";
	}
	cplexFile << std::endl;
	
	//Print Activities Resource Consumption
	for (int activity = 0; activity < this->m_ActivitiesResourceConsumption.size(); activity++) {
		for (int resource = 0; resource < this->m_ActivitiesResourceConsumption[activity].size(); resource++) {
			cplexFile << this->m_ActivitiesResourceConsumption[activity][resource]<<" ";
		}
		cplexFile << std::endl;
	}

	//Print PredecessorsMatrix

	for (int i = 0; i < this->m_ActivitiesCount; i++) {
		for (int j = 0; j < this->m_ActivitiesCount; j++) {
			if (this->m_PredecessorsMatrix[i][j])
				cplexFile << "1 ";
			else
				cplexFile << "0 ";
		}
		cplexFile << std::endl;
	}

	//Print Earliest Starts of Activities
	for (int activity = 0; activity < this->m_ActivitiesCount; activity++) {
		this->m_ActivitiesEarliestStart[activity];
	}

}