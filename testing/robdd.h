#ifndef ROBDD_H
#define ROBDD_H

#include "boolean_vector.h"
#include <iostream>
#include <vector>
#include <list>


//����� ROBDD
class ROBDD
{
private:
	//����� ���� ROBDD, �������� ������ �� ���� � ����������� ��������� ROBDD
	class Node
	{
	private:
		int countReuse = 0;					//���������� �������� ����� � ���� ���� 

		Node* left = nullptr;				//����, � �������� ����� ��������� �����, ������ false 
		Node* right = nullptr;				//����, � �������� ����� ��������� �����, ������ true

		bool isEndNode = true;				//���� - �������� �� ���� ��������
		unsigned int indexVar = 0;			//������ ���������� � ����(������������ ���� ������� �� ��������) 
		bool value;							//����������� �������� � ����(������������ ���� ������� ��������) 

		//���������������� ������� ��� ������������ ������������(��������� � ��� � robdd.cpp)
		void insertEqualNode(const BoolVector& expr, int startBit, int lastBit);
		static Node* getEqualVector(const BoolVector& expr, int startBit, int lastBit, Node* headTree, int indexVar_);
		static bool checkEqualVector(const BoolVector& expr, int startBit, int lastBit, Node* headTree, int indexVar_);
		static void skipEqualSubtree(const BoolVector& expr, const int& startBit, int& lastBit, int& indexVar_);
	public:
		Node(bool value_ = true) { value = value_; };									//������� �����������(������� ���� ����)
		Node(const BoolVector& expr, int startBit, int lastBit, int indexVar_ = 1);		//����������� ����������� ROBDD

		~Node();

		int getCountWay();																//���������� ��������� ����� �� �������� ������� � ��������

		std::vector<BoolVector>& getVectors(std::vector<BoolVector>& currentVectors, int& indexCurrentVector, int& indexPosition);		//��������� ���� ����� �� �������� ������� � �������� � ���� ������ �������

		void output(std::ostream& out);				//����� ���� �� ����� ��� ��������� ��������� � �������
	};

	Node* head = nullptr;														//�������� ������� ������
	int countVar = 0;															//���������� ���������� � �������, ������� ��������� ����  
public:
	ROBDD() {};																	//����������� ������� ����� 
	ROBDD(const BoolVector& expr);												//����������� ROBDDD �� ������� ����������� ������� �������
	//� ������ ������ ������ ������������ ����� ������� �������� ������� �� ������� ����������
	//�������� ��� ������� y = x1 & x2 ������� �������
	//x1 x2 y
	//0  0  0
	//0  1  0
	//1  0  0
	//1  1  1
	//=> ����� expr ������ ���� ����� ��� ���� ������� [0, 0, 0, 1] 

	~ROBDD() { if (head) delete head; }											//���������� robdd

	int getCountWay() { return (head ? head->getCountWay() : 0); };				//������ ���������� ��������������� ������ � Node
	std::vector<BoolVector> getVectors();

	friend std::ostream& operator <<(std::ostream& out, ROBDD& graph);
};

#endif