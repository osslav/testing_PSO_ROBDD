#include "pso.h"
#include <ctime>

//вспомогательные функции для вычисления арифметических опреаций над булевыми векторами
/*
BoolVector operator +(BoolVector term1, BoolVector term2)
{
	BoolVector remainder = (term1 & term2) << 1;
	term1 ^= term2;

	while (!remainder.isZerosVector())
	{
		BoolVector remainder_last = remainder;
		remainder = (term1 & remainder_last) << 1;
		term1 ^= remainder_last;
	}
	return term1;
}

BoolVector operator -(BoolVector term1, BoolVector term2)
{
	BoolVector one(term2.size(), 0);
	one.setOneInd(term2.size() - 1);

	return term1 + ~term2 + one;
}

BoolVector operator *(int termNum, BoolVector termVector)
{
	int sizeVector = termVector.size();
	BoolVector res(sizeVector);
	BoolVector vectorNumber('f', termNum, sizeVector);
	for (int i = 0; i < sizeVector; i++)
	{
		if (termVector[i] == true)
			res = res + vectorNumber;

		vectorNumber <<= 1;
	}

	return res;
}
*/

//подсчет количества парных покрытий в массиве векторов
//суть работы функции в простом переборе всех пар значений и подсчет покрытий в них
int pso::getCountFitness(const std::vector<BoolVector>& solutions)
{
	if (solutions.size() < 4)					//если веторов меньше 4 то парных покрытий в них быть не может 
		return 0;

	int countOfCoveredPair = 0;					//счетчик покрытых пар
	const int countVar = solutions[0].size();	//количество переменных(для которых изем покрытие)
	int countSolutions = solutions.size();		//количество векторов в массиве

	int indexCurrentSolution = 0;				//индекс текущего вектора в массиве
	int indexFirstVar = 0;						//индекс первой переменной в паре
	int indexSecondVar = 1;						//индекс второй переменной в паре
	BoolVector counterCoveredPair(4);			//булев вектор длины 4, хранит 4 флага для каждого варианта пары переменных:
												// counterCoveredPair[0] - найдена ли пара 0 - 0
												// counterCoveredPair[1] - найдена ли пара 0 - 1
												// counterCoveredPair[2] - найдена ли пара 1 - 0
												// counterCoveredPair[3] - найдена ли пара 1 - 1

	while (indexFirstVar < (countVar - 1))		//пока не вышли за пределы диапазона индексов переменных - перебираем все пары и смотрим покрыты ли они
	{
		int indexPair = 2 * solutions[indexCurrentSolution][indexFirstVar] + solutions[indexCurrentSolution][indexSecondVar];
		counterCoveredPair.setOneInd(indexPair);

		indexCurrentSolution++;
		if (indexCurrentSolution >= countSolutions)
		{
			if (counterCoveredPair.searchWeight() == counterCoveredPair.size())
				countOfCoveredPair++;

			counterCoveredPair.setZero();

			indexCurrentSolution = 0;
			indexSecondVar++;

			if (indexSecondVar >= countVar)
			{				
				indexFirstVar++;
				indexSecondVar = indexFirstVar + 1;
			}
		}
	}

	return countOfCoveredPair;
}

//поиск оптимального покрытия из переданного массива векторов 
//цели у него такие же как у pso, но он идет просто перебирая все векторы
std::vector<BoolVector> pso::maxFitnessSlowMethod(const std::vector<BoolVector>& solutions)
{
	/*int countFitness = pso::getCountFitness(solutions);			//количество парных покрытий

	for (int i = solutions.size() - 1; i >= 0 ; i--)		//цикл идет по всем векторам в массиве
	{
		BoolVector currentSolution = solutions[i];			//удаляем i-ый вектор(сохраняя его в другую переменную) 
		solutions[i] = solutions.back();
		solutions.pop_back();

		if (pso::getCountFitness(solutions) < countFitness)		//проверяем - если покрытие из-за этого уменьшилось, то возвращаем обратно удаленный вектор
			solutions.push_back(currentSolution);
	}

	return solutions;*/

	std::vector<BoolVector> optimalSolutions = solutions;

	for (int i = solutions.size() - 1; i >= 0; i--)		//цикл идет по всем векторам в массиве
	{
		BoolVector currentSolution = optimalSolutions[i];			//удаляем i-ый вектор(сохраняя его в другую переменную) 
		optimalSolutions[i] = optimalSolutions.back();
		optimalSolutions.pop_back();

		if (!pso::coveredBy(optimalSolutions, solutions))		//проверяем - если покрытие из-за этого уменьшилось, то возвращаем обратно удаленный вектор
			optimalSolutions.push_back(currentSolution);
	}

	return optimalSolutions;
}

//оболочка с таймером для алгоритма поиска максимально покрытия перебором
std::vector<BoolVector> pso::maxFitnessSlowMethodTimer(const std::vector<BoolVector>& solutions, int& resultTimer)
{
	resultTimer = clock();

	std::vector<BoolVector> res = pso::maxFitnessSlowMethod(solutions);

	resultTimer = clock() - resultTimer;

	return res;
}

//проверка - покрывается ли один массив векторов другим массивом
//функция аналогична функции подсчета парных покрытий - проверяем перебором всех пар переменных
bool pso::coveredBy(const std::vector<BoolVector>& coveredVector, const std::vector<BoolVector>& originalVector)
{
	int countOfCoveredPair = 0;
	int countVar = originalVector[0].size();
	int countSolutions = originalVector.size();

	int indexCurrentSolution = 0;
	int indexFirstVar = 0;
	int indexSecondVar = 1;
	BoolVector counterCoveredPair(4);

	while (indexFirstVar < (countVar - 1))
	{
		int indexPair = 2 * originalVector[indexCurrentSolution][indexFirstVar] + originalVector[indexCurrentSolution][indexSecondVar];
		if (counterCoveredPair[indexPair] == false)
		{
			bool weFindPairInCoveredVector = false;
			for (int i = 0; i < coveredVector.size() && !weFindPairInCoveredVector; i++)
				if (indexPair == 2 * coveredVector[i][indexFirstVar] + coveredVector[i][indexSecondVar])
					weFindPairInCoveredVector = true;

			if (!weFindPairInCoveredVector)
				return false;
		}
		counterCoveredPair.setOneInd(indexPair);

		indexCurrentSolution++;
		if (indexCurrentSolution >= countSolutions)
		{
			if (counterCoveredPair.searchWeight() == counterCoveredPair.size())
				countOfCoveredPair++;

			counterCoveredPair.setZero();

			indexCurrentSolution = 0;
			indexSecondVar++;

			if (indexSecondVar >= countVar)
			{
				indexFirstVar++;
				indexSecondVar = indexFirstVar + 1;
			}
		}
	}

	return true;
}

std::vector<BoolVector> sum(std::vector<BoolVector> a, std::vector<BoolVector> b)
{
	while (!a.empty())								//записываем полученное глобальное лучшее в оптимальное покрытие
	{
		b.push_back(a.back());
		a.pop_back();
	}
	return b;
}

bool weFindInVector(std::vector<BoolVector> vector, BoolVector elem)
{
	int countElem = vector.size();
	for (int i = 0; i < countElem; i++)
		if (vector[i] == elem)
			return true;

	return false;
}

//алгоритм оптимизации роя частиц
//в качестве аргументов получаем:
//	массив булевых вектором(тестовое покрытие), который будем оптимизировать для тестирования
//	maxIteration - максимально возможное количество итераций для перемещения частиц в алгоритме
//	m - минимальное число веторов, которое может содержать оптимальное тестовое покрытие
//	w - коэффициент инерции скорости частицы
//	k -  коэффициент определяющий зависимость скорости частицы от наилучшего текущего положения частицы
std::vector<BoolVector> pso::psoMethod(const std::vector<BoolVector>& TS, const unsigned int maxIteration, const int m, const int w, const int r)
{
	int countArg = TS[0].size();				//количество переменных в тестировании
	long countVectors = TS.size();
	std::vector<BoolVector> optimalTS;			//оптимальное тестовое покрытие(результат алгоритма заносится сюда)
	bool weFindOptimalTs = false;				//флаг завершения алгоритма - найдено ли оптимальное покрытие
	while (!weFindOptimalTs)					//пока не найдено ли оптимальное покрытие - продолжаем алгоритм
	{
		if (pso::coveredBy(optimalTS, TS))			//если найденное оптимальное покрытие покрывает исходный массив то выходим из алгоритма
		{
			weFindOptimalTs = true;
			break;
		}

		std::vector<BoolVector> X(m), V(m);								//создаем набор позиций частиц(из исходного массива) и их скоростей(случайным образом)
		for (int i = 0; i < m; i++)
		{
			X[i] = TS[rand() % countVectors];
			V[i] = BoolVector(countArg, createRandVector);
		}
		std::vector<BoolVector> localBest = X;									//инициаллизируем лучший локальный набор частиц(который будет равен первому, поскольку он пока что единственный) 
		std::vector<BoolVector> globalBest = pso::maxFitnessSlowMethod(X);		//инициаллизируем лучший глобальный набор частиц(который будет равен максимальному покрытию в первом наборе)
		if (pso::coveredBy(sum(globalBest, optimalTS), TS))					//если текущий набор частиц покрывает исходный набор то алгоритм закончен
			weFindOptimalTs = true;

		for (int counter = 0; counter < maxIteration && !weFindOptimalTs; counter++)			//в цикле запускаем движение частиц
		{

			for (int i = 0; i < m; i++)										//для каждой частицы в наборе обновляем ее скорость и положение в пространстве
			{								
				//переводим булевы вектора в числа и совершаем над ними операции для нахождения текущей скорости и положения частиц
				//V[i] = BoolVector('f', unsigned long long(w * V[i].getNum() + 2 * (double(rand() % 101) / 100) * r * (localBest[i].getNum() - X[i].getNum())), countArg);
				//X[i] = BoolVector('f', unsigned long long(V[i].getNum() + X[i].getNum()), countArg);

				//все опреации над булевыми векторами проводим непосредственно над ними, без перевода их в числа
				V[i] = V[i] * w + (localBest[i] - X[i]) * int(2 * (double(rand() % 101) / 100) * r);
				X[i] = V[i] + X[i];
			}

			if (pso::getCountFitness(X) > pso::getCountFitness(localBest))			//если покрытие текущего набора лучше чем любые старые наборы - обновляем локальное и глобальное лучшее
			{
				localBest = X;
				globalBest = pso::maxFitnessSlowMethod(X);

				if (pso::coveredBy(sum(globalBest, optimalTS), TS))					//если текущий набор частиц покрывает исходный набор то алгоритм закончен
					weFindOptimalTs = true;
			}
		}

		while (!globalBest.empty())								//записываем полученное глобальное лучшее в оптимальное покрытие
		{
			if (true)//!weFindInVector(optimalTS, globalBest.back()))
				optimalTS.push_back(globalBest.back());
			globalBest.pop_back();
		}
	}

	return optimalTS;		//возврат результата
}

// оболочка алгоритма оптимизации роя частиц с таймером
std::vector<BoolVector> pso::psoMethodTimer(const std::vector<BoolVector>& TS, const unsigned int maxIteration, const int m, const int coeff1, const int coeff2, int& resultTimer)
{
	resultTimer = clock();

	std::vector<BoolVector> optimalTS = pso::psoMethod(TS, maxIteration, m, coeff1, coeff2);
	
	resultTimer = clock() - resultTimer;

	return optimalTS;
}