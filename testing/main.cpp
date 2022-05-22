#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include "robdd.h"
#include "pso.h"
#include <math.h>

const unsigned int COUNT_VAR = 15;

void printVectors(std::vector<BoolVector> outVectors)
{
	for (int i = 0; i < outVectors.size(); i++)
	{
		for(int j = outVectors[i].size() - 1; j >= 0; j--)
			std::cout << outVectors[i][j];

		std::cout << '\n';
	}
};

void printVector(BoolVector outVector)
{
	for (int i = outVector.size() - 1; i >= 0; i--)
		std::cout << outVector[i];

	std::cout << '\n';
};

void outputVector(BoolVector outVector, const char* fileName)
{
	ofstream outFile;
	outFile.open(fileName);
	for (int i = 0; i < outVector.size(); i++)
		if (outVector[i])
			outFile << '1';
		else
			outFile << '0';

	outFile << '\n';

	outFile.close();
};

BoolVector inputVector(const char* fileName)
{	
	ifstream inFile;
	inFile.open(fileName);
	std::string str;
	
	int i = 0;
	char currentSymbol;
	while (inFile >> currentSymbol)
	{		
		str += currentSymbol;
		i++;
	}

	char* cstr = new char[str.length() + 1];
	for (int i = 0; i < str.length(); i++)
		cstr[i] = str[i];
	cstr[str.length()] = '\0';

	BoolVector newVector(cstr);

	delete[] cstr;

	inFile.close();
	return newVector;
};



int main()
{
	std::srand(time(NULL));
	int timer = 0;
	//задаем булево выражение как вектор его решений и краткий вывод информации о нем
	BoolVector testExpr(pow(2, COUNT_VAR), 50); //= inputVector("BoolVector var = 9, percentOnes = 30.txt");
	std::cout << "Test expression contains " << COUNT_VAR << " vatiables => " << pow(2, COUNT_VAR) << " test cases\n";
	//printVector(testExpr);

	//построение ROBDD графа
	timer = clock();
	ROBDD testGraph(testExpr);
	timer = clock() - timer;
	std::cout << "ROBDD built in " << timer << " ms\n";
	//std::cout << testGraph;
	timer = 0;
	//получение сокращенного массива тестовых случаев из графа
	std::vector<BoolVector> ts = testGraph.getVectors();
	std::cout << "Result robdd contains " << ts.size() << " vectors\n";
	//printVectors(ts);
	
	//используя медленную функцию с перебором получаем оптимальный набор тестовых случаев(с выводом краткой информации и проверкой на правильность)	
	/*
	std::vector<BoolVector> maxFitness = maxFitnessSlowMethod(ts, timer);
	std::cout << "Max fitness slow method returned " << maxFitness.size() << " vectors in " << timer << " ms\n";

	if (coveredBy(maxFitness, ts))
		std::cout << "Good.\n";
	else
		std::cout << "Error! Max fitness slow method NOT covered result robdd\n";
	printVectors(maxFitness);
	*/

	//используя PSO в классическом варианте получаем оптимальный набор тестовых случаев, задавая различные коэффициенты
	//имеется вывод краткой информации и проверка на правильность результата
	std::cout << "\nPSO:\n";
	for (int w = 5; w < 6; w += 1)
	{
		std::cout << "w = " << w << '\n';
		for (int r = 5; r < 6; r += 1)
		{
			std::vector<BoolVector> psoRes = pso::psoMethodTimer(ts, 12, 1, w, r, 0, timer);
			std::cout << "r = " << r << " : " << timer << "ms and " << psoRes.size() << " vectors\n";
			if (pso::coveredBy(psoRes, ts))
				std::cout << "Good.\n";
			else
				std::cout << "Error! Result NOT covered robdd\n";
			//printVectors(psoRes);
		}
		std::cout << "\n";
	}
	

	return 0;
};

