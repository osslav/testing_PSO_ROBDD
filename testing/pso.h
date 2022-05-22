#include <vector>
#include "robdd.h"
namespace pso
{
	//получение количества парных покрытий в массиве булевых векторов
	int getCountFitness(const std::vector<BoolVector>& solutions);

	//алгоритм поиска максимально покрытия перебором
	std::vector<BoolVector> maxFitnessSlowMethod(const std::vector<BoolVector>& solutions);
	std::vector<BoolVector> maxFitnessSlowMethodTimer(const std::vector<BoolVector>& solutions, int& resultTimer);

	//проверка - покрывается ли один массив векторов другим массивом
	bool coveredBy(const std::vector<BoolVector>& coveredVector, const std::vector<BoolVector>& originalVector);

	//алгоритм оптимизации роя частиц
	std::vector<BoolVector> psoMethod(const std::vector<BoolVector>& TS, const unsigned int maxIteration, const int m, const int w, const int r, const int mode);
	std::vector<BoolVector> psoMethodTimer(const std::vector<BoolVector>& TS, const unsigned int maxIteration, const int m, const int coeff1, const int coeff2, const int mode, int& resultTimer);
}