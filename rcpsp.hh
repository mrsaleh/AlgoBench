#include <vector>
#include <string>


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
	int GetActivityResourceConsumption(int activity,int resource);
	std::vector<int> GetLevelActivities(int level);
	int GetResourceStock(int resource);
	int ComputeResourceConsumptionAtSpecifiedLevel(int level,int resource);
	float ComputeEta(int resource);
	float sgn(float num);
	float ComputeKeiP();
	int FindActivityLevel(int activity);
	void CaluclateLevels();
	public:
	float ComputeTAO();
	public:
	RCPSP(std::string pattersonFilename);
};

