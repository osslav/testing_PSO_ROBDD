#ifndef ROBDD_H
#define ROBDD_H

#include "boolean_vector.h"
#include <iostream>
#include <vector>
#include <list>


// класс ROBDD
class ROBDD
{
private:
	//класс узла ROBDD, содержит данные об узле и рекурсивные алгоритмы ROBDD
	class Node
	{
	private:
		int countReuse = 0;					//количество входящих ребер в этот узел 

		Node* left = nullptr;				//узел, к которому ведет исходящее ребро, случай false 
		Node* right = nullptr;				//узел, к которому ведет исходящее ребро, случай true

		bool isEndNode = true;				//флаг - явялется ли узел концевым
		unsigned int indexVar = NULL;		//индекс переменной в узле(используется если вершина не концевая) 
		bool value;							//константное значение в узле(используется если вершина концевая) 

		//вспомногательные функции для рекурсивного конструктора(подробнее о них в robdd.cpp)
		void insertEqualNode(const BoolVector& expr, int startBit, int lastBit);
		static Node* getEqualVector(const BoolVector& expr, int startBit, int lastBit, Node* headTree, int indexVar_);
		static bool checkEqualVector(const BoolVector& expr, int startBit, int lastBit, Node* headTree, int indexVar_);
		static void skipEqualSubtree(const BoolVector& expr, const int& startBit, int& lastBit, int& indexVar_);
	public:
		Node(bool value_ = true) { value = value_; };									//простой конструткор(создает один узел)
		Node(const BoolVector& expr, int startBit, int lastBit, int indexVar_ = 1);		//рекурсивный конструктор ROBDD

		~Node();

		int getCountWay();																//количество различных путей от корневой вершины к концевой

		std::vector<BoolVector>& getVectors(std::vector<BoolVector>& currentVectors, int& indexCurrentVector, int& indexPosition);		//получение всех путей от корневой вершины к концевой в виде булева вектора

		void output(std::ostream& out);				//вывод узла со всеми его дочерними вершинами в консоль
	};

	Node* head = nullptr;														//корневая вершина дерева
	int countVar = 0;															//количество переменных в функции, которую реализует граф  
public:
	ROBDD() {};																	//конструктор пустого графа 
	ROBDD(const BoolVector& expr);												//конструткор ROBDDD из вектора результатов булевой функции
	//в данном случае вектор представляет собой столбец значений функции из таблицы истинности
	//например для функции y = x1 & x2 получим таблицу
	//x1 x2 y
	//0  0  0
	//0  1  0
	//1  0  0
	//1  1  1
	//=> ветор expr должен быть равен для этой функции [0, 0, 0, 1] 

	~ROBDD() { if (head) delete head; }											//деструктор robdd

	int getCountWay() { return (head ? head->getCountWay() : 0); };				//методы вызывающие соответствующие методы в Node
	std::vector<BoolVector> getVectors();

	friend std::ostream& operator <<(std::ostream& out, ROBDD& graph);
};



#endif