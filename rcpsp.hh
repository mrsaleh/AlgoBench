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
	public:
	RCPSP(std::string pattersonFilename);
};

