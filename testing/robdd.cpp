#include "robdd.h"

//����������� ���� ROBDD, �������� �������� �������� ���������� ROBDD
//��������� ������ ������� �������, �������� �� ������� ������� � ���� ������� � ������ ������� ���������� 
ROBDD::Node::Node(const BoolVector& expr, int startBit, int lastBit, int indexVar_)		
{
	static Node* currentHead = nullptr;											//��������� �� ������ ������ �����������, ������������ � ������ �������
	if (!currentHead)
		currentHead = this;
	
	skipEqualSubtree(expr, startBit, lastBit, indexVar_);						//���������� ������������� ����������
	
	if (checkEqualVector(expr, startBit, lastBit, currentHead, indexVar_))		//�������� ���� �� � ������ ��� ��������� ����� �� ���������
	{
		isEndNode = false;														//���� ����� ������� - ������� ������� ���������, �� ��� ����� ��������� ������������: 
		indexVar = indexVar_;													//���������� ������� ����� ��������, �� ���� isEndNode ����� ��������� ��������
	}
	else
	{																			//�����, ������� ����� �������
		indexVar = indexVar_;
		int lenth = lastBit - startBit + 1;
		if (startBit != lastBit)												//���������� �������� ����� ������� ��� ��� 
		{																		//���� ��������, �� ���������� ��������
			isEndNode = false;
			left = new Node(expr, startBit, lenth / 2 + startBit - 1, indexVar + 1);

			right = new Node(expr, lenth / 2 + startBit, lastBit, indexVar + 1);
		}
		else
		{																		//���� ���, �� ������� �������� ������� � ���
			isEndNode = true;
			value = expr[startBit];
		}
	}
	if (currentHead == this)													//���� ���������� ������ ���������
	{
		currentHead = nullptr;													//�������� ����������� ��������� �� ������ ������
		insertEqualNode(expr, startBit, lastBit);								//����� ������, ������� ������� ������ ��������� ������ ������ �� ��� ��������� �������
	}
}
//����������� �����, ������� ������� ������ ��������� ������ ������ �� ��� ��������� �������
void ROBDD::Node::insertEqualNode(const BoolVector& expr, int startBit, int lastBit)
{
	if (this->isEndNode)														//���� ������� ������� ��������, �� ������� �� �������
		return;

	static Node* currentHead = nullptr;											//��������� � ����������� ��������� ����� ����� ������ 
	if (!currentHead)
		currentHead = this;
	
	int newLenth = lastBit - startBit + 1;										//���������� ���������� ���������� ��� ����� �������  
	for (int i = left->indexVar - indexVar; i > 0; i--)
		newLenth /= 2;

	if (!left->isEndNode && !left->right && !left->left)						//���� ����� ������� �������� ���������
	{
		Node* temp = left;														//�� ������� �������, ������� ������ ���� �� �� ����� � ��������� ��
		left = getEqualVector(expr, startBit, newLenth + startBit - 1, currentHead, temp->indexVar);
		left->countReuse++;
		delete temp;
	}
	else
		left->insertEqualNode(expr, startBit, newLenth + startBit - 1);			//����� - ���������� �������� ��� ���� �������
	
	startBit += (lastBit - startBit + 1)/2;										//���������� �������������� ������ �������
	newLenth = lastBit - startBit + 1;
	for (int i = right->indexVar - indexVar - 1; i > 0; i--)
		newLenth /= 2;

	if (!right->isEndNode && !right->right && !right->left)
	{
		Node* temp = right;
		right = getEqualVector(expr, startBit, startBit + newLenth - 1, currentHead, temp->indexVar);
		right->countReuse++;
		delete temp;
	}
	else
		right->insertEqualNode(expr, startBit, startBit + newLenth - 1);

	if (currentHead == this)													//���� ��� ������ ����������, �������� ����������� ��������� �� ������ ������
		currentHead = nullptr;
}

//������� ��������� ��� ��������� ������� ��� ��������� ������� 
ROBDD::Node* ROBDD::Node::getEqualVector(const BoolVector& expr, int startBit, int lastBit, Node* headTree, int indexVar_)
{
	int lenth = lastBit - startBit + 1;								//�������� - ���������� �� ����� �� ��������� � ����������� ��� ��������� ������� � �����(���� ����� ����������)
	
	bool findEqualVector = false;
	int positionEqualVector = 0;
	while ((positionEqualVector < startBit) && !findEqualVector)																										
	{		
		findEqualVector = true;
		for (int i = positionEqualVector, j = startBit; findEqualVector && j <= lastBit; i++, j++)
			if (expr[i] != expr[j]) findEqualVector = false;

		if (!findEqualVector) positionEqualVector += lenth;
	} 


	if (findEqualVector)										//���� ����� ��������� ������, �� ���� �������������� ��� �������
	{		
		Node* equalNode = headTree;

		int startCurrentVector = 0;
		int lenthCurrentVector = expr.size();

		for (int i = 1; i < headTree->indexVar; i++)
			lenthCurrentVector /= 2;

		while (!equalNode->isEndNode && (equalNode->indexVar < indexVar_))
		{
			
			if (positionEqualVector < (startCurrentVector + lenthCurrentVector / 2))
			{			
				if (!equalNode->left->isEndNode)
					for (int i = 0; i < (equalNode->left->indexVar - equalNode->indexVar); i++)
						lenthCurrentVector /= 2;

				equalNode = equalNode->left;
			}
			else
			{
				startCurrentVector += lenthCurrentVector / 2;
				
				if (!equalNode->right->isEndNode)
					for (int i = 0; i < (equalNode->right->indexVar - equalNode->indexVar); i++)
						lenthCurrentVector /= 2;

				equalNode = equalNode->right;
			}

		}
		equalNode->countReuse++;
		return equalNode;
	}
	else
	{														//����� - ���������� ������ ���������
		return nullptr;
	}
}

//������� �������� - ���� �� ��� ��������� ������� ��� ��������� ������� 
bool ROBDD::Node::checkEqualVector(const BoolVector& expr, int startBit, int lastBit, Node* headTree, int indexVar_)
{
	int lenth = lastBit - startBit + 1;

	bool findEqualVector = false;
	int positionEqualVector = 0;
	while ((positionEqualVector < startBit) && !findEqualVector)
	{
		findEqualVector = true;
		for (int i = positionEqualVector, j = startBit; findEqualVector && j <= lastBit; i++, j++)
			if (expr[i] != expr[j]) findEqualVector = false;

		if (!findEqualVector) positionEqualVector += lenth;
	}

	return findEqualVector;
}

//������� ������������� ����������� � ������� (��� ������ ��� ������ right(u) = left(u))
void ROBDD::Node::skipEqualSubtree(const BoolVector& expr, const int& startBit, int& lastBit, int& indexVar_)
{
	int lenth = lastBit - startBit + 1;
	bool equalSubVector = true;
	while (equalSubVector && startBit != lastBit)
	{
		for (int i = startBit, j = lenth / 2 + startBit; j <= lastBit && equalSubVector; i++, j++)
			if (expr[i] != expr[j]) equalSubVector = false;

		if (equalSubVector)
		{
			indexVar_++;
			lenth /= 2;
			lastBit = lenth + startBit - 1;
		}
	}
}

//���������� �������(� ��������� ���� �����������)
ROBDD::Node::~Node()
{ 
	if (left)
		if (!left->countReuse)
			delete left;
		else
			left->countReuse--;
		
	if (right)
		if (!right->countReuse)
			delete right;
		else
			right->countReuse--;
}

//������� ���������� ���� ��������� ����� �� ������� ������� �� ����� �������� �������
int ROBDD::Node::getCountWay()
{
	static int count = 0;
	bool isStartNode = false;
	if (count == 0)
	{
		count = 1;
		isStartNode = true;
	}

	if (!isEndNode)
	{
		count++;
		left->getCountWay();
		right->getCountWay();
	}

	int result = count;
	if (isStartNode)
	{
		count = 0;
	}
	return result;
}

//��������� ���� ������� ROBDD � ���� �������� ����� ������ �����
std::vector<BoolVector>& ROBDD::Node::getVectors(std::vector<BoolVector>& currentVectors, int& indexCurrentVector, int& indexPosition)
{	
	if (!isEndNode)
	{
		int lenghtEdge = 0;
		if (indexCurrentVector == 0 && indexPosition == 0)
		{
			lenghtEdge = indexVar;
			for (int i = 1; i < lenghtEdge; i++)
			{
				currentVectors[indexCurrentVector].set(indexPosition, rand() % 2);
				indexPosition++;
			}
		}

		lenghtEdge = left->indexVar - indexVar;
		
		currentVectors[indexCurrentVector].set(indexPosition, false);
		indexPosition++;
		
		for (int i = 1; i < lenghtEdge; i++)
		{
			currentVectors[indexCurrentVector].set(indexPosition, rand() % 2);
			indexPosition++;
		}

		left->getVectors(currentVectors, indexCurrentVector, indexPosition);
		indexPosition -= lenghtEdge;

		currentVectors[indexCurrentVector + 1] = currentVectors[indexCurrentVector];
		indexCurrentVector++;

		lenghtEdge = right->indexVar - indexVar;
		
		currentVectors[indexCurrentVector].set(indexPosition, true);
		indexPosition++;
		
		for (int i = 1; i < lenghtEdge; i++)
		{
			currentVectors[indexCurrentVector].set(indexPosition, rand() % 2);
			indexPosition++;
		}

		right->getVectors(currentVectors, indexCurrentVector, indexPosition);
		indexPosition -= lenghtEdge;
	}
	return currentVectors;
}

//����������� ROBDD
ROBDD::ROBDD(const BoolVector& expr)
{ 
	if (expr.size() != 0)
	{
		countVar = 0;
		for (int i = expr.size(); i > 1; i /= 2)
		{
			if (i % 2 != 0)
				throw 0;

			countVar++;
		}
	}

	head = new Node(expr, 0, expr.size() - 1); 
};

//��������� ���� ������� ROBDD
std::vector<BoolVector> ROBDD::getVectors()
{ 
	int lengthVector = head->getCountWay();
	std::vector<BoolVector> solutions (lengthVector);
	for (int i = 0; i < lengthVector; i++)
		solutions[i] = BoolVector(countVar);
	int indexCurrentVector = 0; 
	int indexPosition = 0;
	return head->getVectors(solutions, indexCurrentVector, indexPosition);
};

//������ ������ � �������
void ROBDD::Node::output(std::ostream& out)
{
	static int level = -1;

	level++;
	if (isEndNode)
	{
		for (int i = 0; i < level; i++)
			out << "  ";
		out << '\"' << value << '\"' << "                      adress: " << this << '\n';
	}
	else
	{		
		left->output(out);
		for (int i = 0; i < level; i++)
			out << "  ";
		out << indexVar << "                      adress: " << this << '\n';
		right->output(out);

	}
	level--;
}

std::ostream& operator <<(std::ostream& out, ROBDD &graph)
{	
	graph.head->output(out);
	return out;
}
