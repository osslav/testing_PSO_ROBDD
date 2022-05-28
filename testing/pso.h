#include <vector>
#include "robdd.h"
namespace pso
{
	//��������� ���������� ������ �������� � ������� ������� ��������
	int getCountFitness(const std::vector<BoolVector>& solutions);

	//�������� ������ ����������� �������� ���������
	std::vector<BoolVector> maxFitnessSlowMethod(const std::vector<BoolVector>& solutions);
	std::vector<BoolVector> maxFitnessSlowMethodTimer(const std::vector<BoolVector>& solutions, int& resultTimer);

	//�������� - ����������� �� ���� ������ �������� ������ ��������
	bool coveredBy(const std::vector<BoolVector>& coveredVector, const std::vector<BoolVector>& originalVector);

	//�������� ����������� ��� ������
	std::vector<BoolVector> psoMethod(const std::vector<BoolVector>& TS, const unsigned int maxIteration, const int m, const int w, const int r);
	std::vector<BoolVector> psoMethodTimer(const std::vector<BoolVector>& TS, const unsigned int maxIteration, const int m, const int coeff1, const int coeff2, int& resultTimer);
}