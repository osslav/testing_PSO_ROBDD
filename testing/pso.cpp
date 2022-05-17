#include "pso.h"
#include <ctime>

//��������������� ������� ��� ���������� �������������� �������� ��� �������� ���������
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


//������� ���������� ������ �������� � ������� ��������
//���� ������ ������� � ������� �������� ���� ��� �������� � ������� �������� � ���
int pso::getCountFitness(const std::vector<BoolVector>& solutions)
{
	if (solutions.size() < 4)					//���� ������� ������ 4 �� ������ �������� � ��� ���� �� ����� 
		return 0;

	int countOfCoveredPair = 0;					//������� �������� ���
	const int countVar = solutions[0].size();	//���������� ����������(��� ������� ���� ��������)
	int countSolutions = solutions.size();		//���������� �������� � �������

	int indexCurrentSolution = 0;				//������ �������� ������� � �������
	int indexFirstVar = 0;						//������ ������ ���������� � ����
	int indexSecondVar = 1;						//������ ������ ���������� � ����
	BoolVector counterCoveredPair(4);			//����� ������ ����� 4, ������ 4 ����� ��� ������� �������� ���� ����������:
												// counterCoveredPair[0] - ������� �� ���� 0 - 0
												// counterCoveredPair[1] - ������� �� ���� 0 - 1
												// counterCoveredPair[2] - ������� �� ���� 1 - 0
												// counterCoveredPair[3] - ������� �� ���� 1 - 1

	while (indexFirstVar < (countVar - 1))		//���� �� ����� �� ������� ��������� �������� ���������� - ���������� ��� ���� � ������� ������� �� ���
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

//����� ������������ �������� �� ����������� ������� �������� 
//���� � ���� ����� �� ��� � pso, �� �� ���� ������ ��������� ��� �������
std::vector<BoolVector> pso::maxFitnessSlowMethod(std::vector<BoolVector> solutions)
{
	int countFitness = pso::getCountFitness(solutions);			//���������� ������ ��������

	for (int i = solutions.size() - 1; i >= 0 ; i--)		//���� ���� �� ���� �������� � �������
	{
		BoolVector currentSolution = solutions[i];			//������� i-�� ������(�������� ��� � ������ ����������) 
		solutions[i] = solutions.back();
		solutions.pop_back();

		if (pso::getCountFitness(solutions) < countFitness)		//��������� - ���� �������� ��-�� ����� �����������, �� ���������� ������� ��������� ������
			solutions.push_back(currentSolution);
	}

	return solutions;
}

//�������� � �������� ��� ��������� ������ ����������� �������� ���������
std::vector<BoolVector> pso::maxFitnessSlowMethodTimer(std::vector<BoolVector> solutions, int& resultTimer)
{
	resultTimer = clock();

	std::vector<BoolVector> res = pso::maxFitnessSlowMethod(solutions);

	resultTimer = clock() - resultTimer;

	return res;
}

//�������� - ����������� �� ���� ������ �������� ������ ��������
//������� ���������� ������� �������� ������ �������� - ��������� ��������� ���� ��� ����������
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

std::vector<BoolVector> summ(std::vector<BoolVector> a, std::vector<BoolVector> b)
{
	while (!a.empty())								//���������� ���������� ���������� ������ � ����������� ��������
	{
		b.push_back(a.back());
		a.pop_back();
	}
	return b;
}

//�������� ����������� ��� ������
//� �������� ���������� ��������:
//	������ ������� ��������(�������� ��������), ������� ����� �������������� ��� ������������
//	maxIteration - ����������� ��������� ���������� �������� ��� ����������� ������ � ���������
//	m - ����������� ����� �������, ������� ����� ��������� ����������� �������� ��������
//	w - ����������� ������� �������� �������
//	k -  ����������� ������������ ����������� �������� ������� �� ���������� �������� ��������� �������
std::vector<BoolVector> pso::psoMethod(const std::vector<BoolVector>& TS, const unsigned int maxIteration, const int m, const int w, const int r, const int mode)
{
	int countArg = TS[0].size();				//���������� ���������� � ������������				
	std::vector<BoolVector> optimalTS;			//����������� �������� ��������(��������� ��������� ��������� ����)
	bool weFindOptimalTs = false;				//���� ���������� ��������� - ������� �� ����������� ��������
	int counter = 0;
	while (!weFindOptimalTs)					//���� �� ������� �� ����������� �������� - ���������� ��������
	{
		if (pso::coveredBy(optimalTS, TS))			//���� ��������� ����������� �������� ��������� �������� ������ �� ������� �� ���������
		{
			weFindOptimalTs = true;
			break;
		}
		counter++;
		std::vector<BoolVector> X(m), V(m);								//������� ����� ������� ������(�� ��������� �������) � �� ���������(��������� �������)
		for (int i = 0; i < m; i++)
		{
			X[i] = TS[rand() % countArg];
			V[i] = BoolVector(countArg, createRandVector);
		}
		std::vector<BoolVector> localBest = X;									//��������������� ������ ��������� ����� ������(������� ����� ����� �������, ��������� �� ���� ��� ������������) 
		std::vector<BoolVector> globalBest = pso::maxFitnessSlowMethod(X);		//��������������� ������ ���������� ����� ������(������� ����� ����� ������������� �������� � ������ ������)
		if (pso::coveredBy(summ(globalBest, optimalTS), TS))					//���� ������� ����� ������ ��������� �������� ����� �� �������� ��������
			weFindOptimalTs = true;

		for (int i = 0; i < maxIteration && !weFindOptimalTs; i++)			//� ����� ��������� �������� ������
		{

			for (int i = 0; i < m; i++)										//��� ������ ������� � ������ ��������� �� �������� � ��������� � ������������
			{								
				//��������� ������ ������� � ����� � ��������� ��� ���� �������� ��� ���������� ������� �������� � ��������� ������
				//V[i] = BoolVector('f', unsigned long long(w * V[i].getNum() + 2 * (double(rand() % 101) / 100) * r * (localBest[i].getNum() - X[i].getNum())), countArg);
				//X[i] = BoolVector('f', unsigned long long(V[i].getNum() + X[i].getNum()), countArg);

				//��� �������� ��� �������� ��������� �������� ��������������� ��� ����, ��� �������� �� � �����
				V[i] = w * V[i] + int(2 * (double(rand() % 101) / 100) * r) * (localBest[i] - X[i]);
				X[i] = V[i] + X[i];
			}

			if (pso::getCountFitness(X) > pso::getCountFitness(localBest))			//���� �������� �������� ������ ����� ��� ����� ������ ������ - ��������� ��������� � ���������� ������
			{
				localBest = X;
				globalBest = pso::maxFitnessSlowMethod(X);

				if (pso::coveredBy(summ(globalBest, optimalTS), TS))					//���� ������� ����� ������ ��������� �������� ����� �� �������� ��������
					weFindOptimalTs = true;
			}
		}

		while (!globalBest.empty())								//���������� ���������� ���������� ������ � ����������� ��������
		{
			optimalTS.push_back(globalBest.back());
			globalBest.pop_back();
		}
	}

	return optimalTS;		//������� ����������
}

// �������� ��������� ����������� ��� ������ � ��������
std::vector<BoolVector> pso::psoMethodTimer(const std::vector<BoolVector>& TS, const unsigned int maxIteration, const int m, const int coeff1, const int coeff2, const int mode, int& resultTimer)
{
	resultTimer = clock();

	std::vector<BoolVector> optimalTS = pso::psoMethod(TS, maxIteration, m, coeff1, coeff2, mode);
	
	resultTimer = clock() - resultTimer;

	return optimalTS;
}