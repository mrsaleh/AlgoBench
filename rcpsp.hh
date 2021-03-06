#include <vector>
#include <string>
#include <random>
#include <chrono>


template<typename T>
T RandomSelect(std::vector<T> set);


class RCPSP{
	friend bool operator == (const RCPSP& lhs,const RCPSP& rhs);

	private:
		//Random generator
		std::default_random_engine m_RandomGenerator; 
		std::uniform_int_distribution<int> m_Distribution;		

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
		std::vector<int>  m_ActivitiesLatestStart;
		bool ** m_PredecessorsMatrix = nullptr;
		int m_TotalNumberOfArcs;
		float m_AverageOfLevelsWidths;
		int m_NPrime;
		//Topological Complexity Measures (I1 to I6)
		
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
	//Recursive Soution to find ls
	int FindActivityLatestFinishRecursive(int activity);
	void CalculateActivitiesLatestStartRecursive();
	//Non-Recursive Solution to find es
	int FindActivityEearliestStart(int activity);
	int GetProjectDuration();
	float ComputeEarliestStartEta(int resource);
	void CalculateActivitiesEearliestStart();
	int ComputeResourceConstrainednesstSpecifiedTime(int time, int resource);
	float ComputeEarliestStartTAO();
	//Compelxity Measures related functions

	float I1();
	float I2();
	void CalculateAverageOfLevelsWidths();
	float I3();
	int D();
	void CalculateTotalNumberOfArcs();
	int CalculateArcsWithLength(int arcLength);
	void CalculateNPrime();
	float I4();
	float I5();
	float I6();

	float CalculateResourceFactor();
	float CalculateResourceConstrainedness(int resource);
	float CalculateResourceStrength(int resource);
	void RandomReduction();
public:
	//Static Methods

	public:
		//Public Methods
	RCPSP(std::ifstream & pattersonFile);
	RCPSP(int activitiesCount);
	int GetResourcesCount() {
		return m_ResourcesCount;
	}

	void SaveToCPlexFile(std::string filename);


};

