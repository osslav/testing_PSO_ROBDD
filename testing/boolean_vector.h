#ifndef BOOLEAN_VECTOR_H
#define BOOLEAN_VECTOR_H

using namespace std;

int const DEFAULT_COUNT = 8;

enum ErrorCode
{
	errorCountLessZero = 1,
	errorStringEmpty,
	errorSymbolString,

	errorIndexOutVector,
	errorRangeIndexOutVector,
};

enum OptionCreateVector
{
	createOfZeroVector = 0,
	createOfOneVector = 100,
	createRandVector = 50,
};

//  ласс булев вектор, дл€ хранени€ одного элемента вектора использует один бит пам€ти
// ¬ектор 1110110001 хранитс€ в пам€ти как:  00000011 10110001 

class BoolVector
{
	unsigned char* vector_ = nullptr;
	int count_ = 0;        //длина вектора
	int memory_ = 0;       //количество выделенных байт

public:
	BoolVector(int count = DEFAULT_COUNT, int percentOnes = 0);
	//BoolVector(char c, unsigned long long num, int count); //убрать
	BoolVector(int cout, unsigned char* string, int countString);
	BoolVector(const char* stringVector);
	BoolVector(const BoolVector& copy);
	~BoolVector() { if (vector_) delete vector_; }

	int size() const;

	void invertVector();
	void invertInd(int index);

	void setZero();
	void setOne();
	void setZeroInd(int index, int count = 1);
	void setOneInd(int index, int count = 1);
	void set(int index, bool val);

	int searchWeight() const;
	bool isZerosVector() const;
	bool isOnesVector() const;

	BoolVector operator &(const BoolVector& term) const;
	void operator &=(const BoolVector& term);

	BoolVector operator |(const BoolVector& term) const;
	void operator |=(const BoolVector& term);

	BoolVector operator ^(const BoolVector& term) const;
	void operator ^=(const BoolVector& term);

	BoolVector& operator =(const BoolVector& copy);

	bool operator [](int index) const;

	BoolVector operator <<(int count) const;

	BoolVector operator >>(int count) const;

	void operator <<=(int count);

	void operator >>=(int count);

	BoolVector operator ~() const;

	bool operator ==(const BoolVector& temp) const;

	//unsigned long long getNum(); //убрать

	BoolVector operator +(const BoolVector& term);

	BoolVector operator -(const BoolVector& term);

	BoolVector operator *(int termNum);

	BoolVector operator *(const BoolVector& term);

};

#endif