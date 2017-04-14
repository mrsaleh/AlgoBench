#include <vector>
#include <string>

class RCPSP{
	friend bool operator == (const RCPSP& lhs,const RCPSP& rhs);

	private:
		int m_LevelsCount = -1;
		int m_CorrectedLevelsCount = -1;
		int m_ResourcesCount = -1;
		int m_ActivitiesCount = -1;
		int m_CorrectedActivitiesCount = -1;
		std::vector<int> m_ActivitiesDuration;
		std::vector<int> m_ResourcesStock;
		std::vector<std::vector<int>> m_ActivitiesSuccessors;
		std::vector<std::vector<int>> m_ActivitiesPredecessors;
		std::vector<std::vector<int>> m_ActivitiesResourceConsumption;
		std::vector<std::vector<int>> m_ProgressiveLevelsActivities;
		std::vector<int> m_ActivitiesRegressiveLevel;
		std::vector<int> m_ActivitiesProgressiveLevel;
		std::vector<int> m_ActivitiesEarliestStart;
		bool ** m_PredecessorsMatrix = nullptr;
		int m_TotalNumberOfArcs;
		float m_AverageOfLevelsWidths;
		int m_NPrime;
		//Topological Complexity Measures
		float m_I1;
		float m_I2;
		float m_I3;
		float m_I4;
		float m_I5;
		float m_I6; 
		//Resource-Wise Complexity Measures
		float m_ResourceFactor;
		std::vector<float> m_ResourceConstrainedness;
	private:
	void CalculatePredecessors();
	std::vector<int> GetLevelActivities(int level);
	int FindActivityRegressiveLevel(int activity);
	void CalculateRegressiveLevels();
	int GetResourceStock(int resource);
	int ComputeResourceConstrainednesstSpecifiedLevel(int level,int resource);
	float ComputeEta(int resource);
	float sgn(float num);
	int FindActivityProgressiveLevel(int activity);
	void CalculateProgressiveLevels();
	public:
	void GeneratePredecessorsMatrix();
	void PrintPredecessorsMatrix();
	float ComputeTAO();
	//Earliest Start TAO
	//Recursive Solution to find es
	int FindActivityEarliestFinishRecursive(int activity);
	void CalculateActivitiesEearliestStartRecursive();
	//Non-Recursive Solution to find es
	int FindActivityEearliestStart(int activity);
	int GetProjectDuration();
	float ComputeEarliestStartEta(int resource);
	void CalculateActivitiesEearliestStart();
	int ComputeResourceConstrainednesstSpecifiedTime(int time, int resource);
	float ComputeEarliestStartTAO();
	//Compelxity Measures related functions

	void CalculateI1();
	void CalculateI2();
	void CalculateAverageOfLevelsWidths();
	void CalculateI3();
	int D();
	void CalculateTotalNumberOfArcs();
	int CalculateArcsWithLength(int arcLength);
	void CalculateNPrime();
	void CalculateI4();
	void CalculateI5();
	void CalculateI6();

	float CalculateResourceFactor();
	float CalculateResourceConstrainedness(int resource);
	float CalculateResourceStrength(int resource);

public:
	//Static Methods

	public:
		//Public Methods
	RCPSP(std::string pattersonFilename);
	float I1() {
		return this->m_I1;
	}

	float I2() {
		return this->m_I2;
	}

	float I3() {
		return this->m_I3;
	}

	float I4() {
		return this->m_I4;
	}

	float I5() {
		return this->m_I5;
	}

	float I6() {
		return this->m_I6;
	}

	int GetResourcesCount() {
		return m_ResourcesCount;
	}


};

