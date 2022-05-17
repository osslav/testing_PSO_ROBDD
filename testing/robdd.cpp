#include "robdd.h"

//конструктор узла ROBDD, содержит основной алгоритм построения ROBDD
//принимает вектор решений функции, указание на текущий отрезок в этой функции и индекс текущей переменной 
Node::Node(const BoolVector& expr, int startBit, int lastBit, int indexVar_)		
{
	static Node* currentHead = nullptr;											//указатель на корень дерева статический, записывается в начале реурсии
	if (!currentHead)
		currentHead = this;
	
	skipEqualSubtree(expr, startBit, lastBit, indexVar_);						//пропускаем повторяющиеся поддеревья
	
	if (checkEqualVector(expr, startBit, lastBit, currentHead, indexVar_))		//проверка есть ли в дереве уже созданное такое же поддерево
	{
		isEndNode = false;														//если такое найдено - текущая вершина временная, на это будет указывать противоречие: 
		indexVar = indexVar_;													//фактически вершина будет концевой, но флаг isEndNode будет указывать обратное
	}
	else
	{																			//иначе, создаем новую вершину
		indexVar = indexVar_;
		int lenth = lastBit - startBit + 1;
		if (startBit != lastBit)												//определяем концевая будет вершина или нет 
		{																		//если концевая, то продолжаем рекурсию
			isEndNode = false;
			left = new Node(expr, startBit, lenth / 2 + startBit - 1, indexVar + 1);

			right = new Node(expr, lenth / 2 + startBit, lastBit, indexVar + 1);
		}
		else
		{																		//если нет, то создаем концевую вершину и все
			isEndNode = true;
			value = expr[startBit];
		}
	}
	if (currentHead == this)													//если построение дерева закончено
	{
		currentHead = nullptr;													//обнуляем статический указатель на корень дерева
		insertEqualNode(expr, startBit, lastBit);								//вызов метода, который вставит вместо временных вершин равные им уже созданные вершины
	}
}
//рекурсивный метод, который вставит вместо временных вершин равные им уже созданные вершины
void Node::insertEqualNode(const BoolVector& expr, int startBit, int lastBit)
{
	if (this->isEndNode)														//если текущая вершины конечная, то выходим из функции
		return;

	static Node* currentHead = nullptr;											//сохраняем в статический указатель адрес корня дерева 
	if (!currentHead)
		currentHead = this;
	
	int newLenth = lastBit - startBit + 1;										//пропускаем одинаковые поддеревья для левой вершины  
	for (int i = left->indexVar - indexVar; i > 0; i--)
		newLenth /= 2;

	if (!left->isEndNode && !left->right && !left->left)						//если левая вершина является временной
	{
		Node* temp = left;														//то находим вершину, которая должна быть на ее месте и вставляем ее
		left = getEqualVector(expr, startBit, newLenth + startBit - 1, currentHead, temp->indexVar);
		left->countReuse++;
		delete temp;
	}
	else
		left->insertEqualNode(expr, startBit, newLenth + startBit - 1);			//иначе - продолжаем рекурсию для этой вершины
	
	startBit += (lastBit - startBit + 1)/2;										//аналогично обрабатывается правая вершина
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

	if (currentHead == this)													//если все дерево обработано, обнуляем статический указатель на корень дерева
		currentHead = nullptr;
}

//функция получения уже созданной вершины для фрагмента вектора 
Node* Node::getEqualVector(const BoolVector& expr, int startBit, int lastBit, Node* headTree, int indexVar_)
{
	int lenth = lastBit - startBit + 1;								//проверка - существует ли такой же подвектор с нахождением его начальной позиции и длины(если такой существует)
	
	bool findEqualVector = false;
	int positionEqualVector = 0;
	while ((positionEqualVector < startBit) && !findEqualVector)																										
	{		
		findEqualVector = true;
		for (int i = positionEqualVector, j = startBit; findEqualVector && j <= lastBit; i++, j++)
			if (expr[i] != expr[j]) findEqualVector = false;

		if (!findEqualVector) positionEqualVector += lenth;
	} 


	if (findEqualVector)										//если такой подвектор найден, то ищем соответсвующую ему вершину
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
	{														//иначе - возвращаем пустой указатель
		return nullptr;
	}
}

//функция проверки - есть ли уже созданная вершина для фрагмента вектора 
bool Node::checkEqualVector(const BoolVector& expr, int startBit, int lastBit, Node* headTree, int indexVar_)
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

//пропуск повторяющихся подвекторов в векторе (для дерева это случай right(u) = left(u))
void Node::skipEqualSubtree(const BoolVector& expr, const int& startBit, int& lastBit, int& indexVar_)
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

//деструктор вершины(с удалением всех поддеревьев)
Node::~Node() 
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

//подсчет количества всех различных путей из текущей вершины до любой концевой вершины
int Node::getCountWay()
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

//получение всех решений ROBDD в виде векторов путем обхода слева
std::vector<BoolVector>& Node::getVectors(std::vector<BoolVector>& currentVectors, int& indexCurrentVector, int& indexPosition)
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

//конструктор ROBDD
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

//получение всех решений ROBDD
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

//методы вывода в консоль
std::ostream& operator <<(std::ostream& out, Node &node)
{
	static int level = -1;

	level++;
	if (node.isEndNode)
	{
		for (int i = 0; i < level; i++)
			out << "  ";
		out << '\"' << node.value << '\"' << "                      adress: " << &node << '\n';
	}
	else
	{		
		out << *(node.left);
		for (int i = 0; i < level; i++)
			out << "  ";
		out << node.indexVar << "                      adress: " << &node << '\n';
		out << *(node.right);

	}
	level--;
	return out;
}

std::ostream& operator <<(std::ostream& out, ROBDD &graph)
{	
	out << *(graph.head);
	return out;
}
