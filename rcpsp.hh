#include <vector>
#include <string>

class RCPSP{
	friend bool operator == (const RCPSP& lhs,const RCPSP& rhs);

	private:
		int m_LevelsCount = -1;
		int m_ResourcesCount = -1;
		int m_ActivitiesCount = -1;
		std::vector<int> m_ActivitiesDuration;
		std::vector<int> m_ResourcesStock;
		std::vector<std::vector<int>> m_ActivitiesSuccessors;
		std::vector<std::vector<int>> m_ActivitiesPredecessors;
		std::vector<std::vector<int>> m_ActivitiesResourceConsumption;
		std::vector<std::vector<int>> m_LevelsActivities;
		std::vector<int> m_ActivitiesLevel;
		bool ** m_PredecessorsMatrix = nullptr;
		int m_TotalNumberOfArcs;
		float m_AverageOfLevelsWidths;
		int m_NPrime;
		//Complexity Measures
		float m_I1;
		float m_I2;
		float m_I3;
		float m_I4;
		float m_I5;
	private:
	void CalculatePredecessors();
	int GetRootActivity();
	int GetActivityResourceConsumption(int activity,int resource);
	std::vector<int> GetLevelActivities(int level);
	int GetResourceStock(int resource);
	int ComputeResourceConsumptionAtSpecifiedLevel(int level,int resource);
	float ComputeEta(int resource);
	float sgn(float num);
	float ComputeKeiP();
	int FindActivityLevel(int activity);
	void CalculateLevels();
	public:
	void GeneratePredecessorsMatrix();
	void PrintPredecessorsMatrix();
	float ComputeTAO();
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
	public:
	RCPSP(std::string pattersonFilename);
};

