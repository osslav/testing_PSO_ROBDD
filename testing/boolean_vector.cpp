#include "boolean_vector.h"
#include <cstdlib>

BoolVector::BoolVector(int count, int percentOnes)
{
	if (count < 0) throw errorCountLessZero;
	count_ = count;
	memory_ = (count + 7) / 8;

	vector_ = new unsigned char[memory_];

	for (int i = 0; i < memory_; i++) vector_[i] = 0;
	
	if (percentOnes <= 0)
	{
		for (int i = 0; i < memory_; i++) vector_[i] = 0;
		return;
	}

	if (percentOnes >= 100)
	{
		for (int i = 0; i < memory_ - 1; i++) vector_[i] = ~0;
		vector_[memory_ - 1] = ~0 >> (8 - count_ % 8);
		return;
	}

	if (percentOnes < 100 && percentOnes > 0)
	{
		for (int i = 0; i < memory_; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				vector_[i] += bool((rand() % 100) <= percentOnes);
				vector_[i] <<= 1;
			}
		}
		unsigned char mask = ~0 >> (8 - count_ % 8);
		vector_[memory_ - 1] &= mask;
		return;
	}
}

BoolVector::BoolVector(char c, unsigned long long num, int count)
{
	if (count < 0) throw errorCountLessZero;
	count_ = count;
	memory_ = (count + 7) / 8;

	vector_ = new unsigned char[memory_];
	unsigned char mask = ~0;
	for (int i = memory_ - 1; i >= 0; i--)
	{
		vector_[i] = num & mask;
		num >>= 8;
	}

}

BoolVector::BoolVector(int count, unsigned char* string, int countString)
{
	count_ = count;
	memory_ = (count_ + 7) / 8;

	vector_ = new unsigned char[memory_];

	for (int i = 0; i < memory_; i++)
	{
		vector_[i] = string[countString - memory_ + i];
	}

	unsigned char mask = ~0;
	mask >>= (memory_ * 8 - count_);
	vector_[0] &= mask;
}

BoolVector::BoolVector(const char* stringVector)
{
	count_ = 0;
	while (stringVector[count_] != '\0')
	{		
		if ((stringVector[count_] != '0') && (stringVector[count_] != '1')) throw errorSymbolString;
		count_++;
	}
	memory_ = (count_ + 7)/ 8;
	vector_ = new unsigned char[memory_];

	int indexVector = -1;
	for (int i = 0; i < count_; i++)
	{ 
		if (i % 8 == 0)
		{
			indexVector++;
			vector_[indexVector] = 0;
		}
		vector_[indexVector] <<= 1;
		vector_[indexVector] += stringVector[i] - '0';
	}
}

BoolVector::BoolVector(const BoolVector& copy)
{
	count_ = copy.count_;
	memory_ = copy.memory_;

	vector_ = new unsigned char[memory_];
	for (int i = 0; i < memory_; i++) vector_[i] = copy.vector_[i];
}

int BoolVector::size() const
{
	return count_;
}

void BoolVector::invertVector()
{
	for (int i = 0; i < memory_; i++) vector_[i] = ~vector_[i];
	unsigned char mask = ~0;
	mask = mask >> (memory_ * 8 - count_);
	vector_[0] = vector_[0] & mask;
}

void BoolVector::invertInd(int index)
{
	if ((index >= count_) || (index < 0)) throw errorIndexOutVector;
	
	int byte = memory_ - 1 - index / 8;
	unsigned char mask = 1 << (index % 8);

	if ((vector_[byte] & mask) != 0) vector_[byte] = vector_[byte] & (~mask);
	else vector_[byte] = vector_[byte] | mask;

}

void BoolVector::setZero()
{
	for (int i = 0; i < memory_; i++) vector_[i] = 0;
}


void BoolVector::setOne()
{
	for (int i = 0; i < memory_; i++) vector_[i] = ~0;
	unsigned char mask = ~0;
	mask = mask >> (memory_ * 8 - count_);
	vector_[0] = vector_[0] & mask;
}

void BoolVector::setZeroInd(int index, int count)
{
	if (count < 0)
	{
		index += count + 1;
		count *= -1;
	}
	if ((index < 0) || (index + count > count_)) throw errorRangeIndexOutVector;


	int byte = memory_ - 1 - index / 8;
	unsigned char mask = 1 << (index % 8);

	for (int i = 0; i < count; i++)
	{
		vector_[byte] = vector_[byte] & (~mask);

		mask = mask << 1;
		if (mask == 0)
		{
			mask = 1;
			byte--;
		}
	}
}


void BoolVector::setOneInd(int index, int count)
{
	if (count < 0)
	{
		index += count + 1;
		count *= -1;
	}
	if ((index < 0) || (index + count > count_)) throw errorRangeIndexOutVector;


	int byte = memory_ - 1 - index / 8;
	unsigned char mask = 1 << (index % 8);

	for (int i = 0; i < count; i++)
	{
		vector_[byte] = vector_[byte] | mask;

		mask = mask << 1;
		if (mask == 0)
		{
			mask = 1;
			byte--;
		}
	}
}

void BoolVector::set(int index, bool val)
{
	if (val)
		setOneInd(index);
	else
		setZeroInd(index);
}


int BoolVector::searchWeight() const
{
	int weight = 0;
	int index = 8 * memory_ - count_;
	unsigned char mask = 1 << (7 - index);
	while (index < memory_ * 8)
	{
		if ((mask & vector_[index / 8]) != 0) weight++;

		index++;
		mask >>= 1;
		if (mask == 0) mask = 1 << 7;
	}

	return weight;
}

bool BoolVector::isZerosVector() const
{
	int index = 8 * memory_ - count_;
	unsigned char mask = 1 << (7 - index);
	while (index < memory_ * 8)
	{
		if ((mask & vector_[index / 8]) != 0) return false;

		index++;
		mask >>= 1;
		if (mask == 0) mask = 1 << 7;
	}

	return true;
}

bool BoolVector::isOnesVector() const
{
	int index = 8 * memory_ - count_;
	unsigned char mask = 1 << (7 - index);
	while (index < memory_ * 8)
	{
		if ((mask & vector_[index / 8]) == 0) return false;

		index++;
		mask >>= 1;
		if (mask == 0) mask = 1 << 7;
	}

	return true;
}

BoolVector BoolVector::operator &(BoolVector term) const
{
	if (memory_ > term.memory_)
	{
		BoolVector result(*this);
		for (int i = 0; i < term.memory_; i++) result.vector_[i + memory_ - term.memory_] &= term.vector_[i];
		for (int i = memory_ - term.memory_ - 1; i >= 0; i--) result.vector_[i] = 0;
		return result;
	}
	else
	{
		BoolVector result(term);
		for (int i = 0; i < memory_; i++) result.vector_[i + term.memory_ - memory_] &= vector_[i];
		for (int i = term.memory_ - memory_ - 1; i >= 0; i--) result.vector_[i] = 0;
		return result;
	}
}

void BoolVector::operator &=(BoolVector term)
{
	if (memory_ > term.memory_)
	{
		for (int i = 0; i < memory_ - term.memory_; i++) vector_[i] = 0;
		for (int i = 0; i < term.memory_; i++) vector_[i + memory_ - term.memory_] &= term.vector_[i];
	}
	else
	{
		unsigned char* result;
		result = new unsigned char[term.memory_];
		
		for (int i = 0; i < memory_ - term.memory_; i++) result[i] = 0;
		for (int i = term.memory_ - memory_; i < term.memory_; i++) result[i] = term.vector_[i];
		for (int i = 0; i < memory_; i++) result[i + term.memory_ - memory_] &= vector_[i];

		delete vector_;
		vector_ = result;
		memory_ = term.memory_;
		count_ = term.count_;
	}
}

BoolVector BoolVector::operator |(BoolVector term) const
{
	if (memory_ > term.memory_)
	{
		BoolVector result(*this);
		for (int i = 0; i < term.memory_; i++) result.vector_[i + memory_ - term.memory_] |= term.vector_[i];
		return result;
	}
	else
	{
		BoolVector result(term);
		for (int i = 0; i < memory_; i++) result.vector_[i + term.memory_ - memory_] |= vector_[i];
		return result;
	}
}


void BoolVector::operator |=(BoolVector term)
{
	if (memory_ > term.memory_)  for (int i = 0; i < term.memory_; i++) vector_[i + memory_ - term.memory_] |= term.vector_[i];
	else
	{
		unsigned char* result;
		result = new unsigned char[term.memory_];
		for (int i = 0; i < term.memory_; i++) result[i] = term.vector_[i];
		for (int i = 0; i < memory_; i++) result[i + term.memory_ - memory_] |= vector_[i];

		delete vector_;
		vector_ = result;
		memory_ = term.memory_;
		count_ = term.count_;
	}
}


BoolVector BoolVector::operator ^(BoolVector term) const
{
	if (memory_ > term.memory_)
	{
		BoolVector result(*this);
		for (int i = 0; i < term.memory_; i++) result.vector_[i + memory_ - term.memory_] ^= term.vector_[i];
		return result;
	}
	else
	{
		BoolVector result(term);
		for (int i = 0; i < memory_; i++) result.vector_[i + term.memory_ - memory_] ^= vector_[i];
		return result;
	}

}


void BoolVector::operator ^=(BoolVector term)
{
	if (memory_ > term.memory_)  for (int i = 0; i < term.memory_; i++) vector_[i + memory_ - term.memory_] ^= term.vector_[i];
	else
	{
		unsigned char* result;
		result = new unsigned char[term.memory_];
		for (int i = 0; i < term.memory_; i++) result[i] = term.vector_[i];
		for (int i = 0; i < memory_; i++) result[i + term.memory_ - memory_] ^= vector_[i];

		delete vector_;
		vector_ = result;
		memory_ = term.memory_;
		count_ = term.count_;
	}
}



bool BoolVector::operator [](int index) const
{
	if ((index < 0) || (index >= count_)) throw errorRangeIndexOutVector;

	int byte = memory_ - index / 8 - 1;
	unsigned char mask = 1 << (index % 8);

	if ((mask & vector_[byte]) != 0) return true;
	else 
		return false;
}

BoolVector& BoolVector::operator =(const BoolVector& copy)
{
	if (this != &copy)
	{
		count_ = copy.count_;
		if (memory_ != copy.memory_)
		{
			memory_ = copy.memory_;
			delete vector_;
			vector_ = new unsigned char[memory_];
		}
		for (int i = 0; i < memory_; i++) vector_[i] = copy.vector_[i];
	}	
	return *this;
}


BoolVector BoolVector::operator >>(int count) const
{
	BoolVector result(count_);

	int step = count / 8;
	count %= 8;

	unsigned char mask = ~0;
	mask >>= (8 - count);
	unsigned char saveBits = 0;

	for (int i = result.memory_ - 1; i >= step; i--)
	{
		result.vector_[i] = vector_[i - step] >> count;
		if (i != result.memory_ - 1)
		{
			saveBits = (vector_[i + step] & mask) << (8 - count);
			result.vector_[i + 1] |= saveBits;
		}
	}

	return result;
}

BoolVector BoolVector::operator <<(int count) const
{
	BoolVector result(count_);

	int step = count / 8;
	count %= 8;

	unsigned char mask = ~0;
	mask <<= (8 - count);
	unsigned char saveBits = 0;

	for (int i = 0; i < (result.memory_ - step); i++)
	{
		result.vector_[i] = vector_[i + step] << count;
		if (i != 0)
		{
			saveBits = (vector_[i - step] & mask) >> (8 - count);
			result.vector_[i - 1] |= saveBits;
		}
		
	}

	mask = ~0;
	mask >>= (memory_ * 8 - count_);
	result.vector_[0] &= mask;

	return result;
}

void BoolVector::operator <<=(int count)
{
	BoolVector oldVector(*this);

	int step = count / 8;
	count %= 8;

	unsigned char mask = ~0;
	mask <<= (8 - count);
	unsigned char saveBits = 0;

	for (int i = 0; i < (memory_ - step); i++)
	{
		vector_[i] = oldVector.vector_[i + step] << count;
		if (i != 0)
		{
			saveBits = (oldVector.vector_[i - step] & mask) >> (8 - count);
			vector_[i - 1] |= saveBits;
		}

	}

	mask = ~0;
	mask >>= (memory_ * 8 - count_);
	vector_[0] &= mask;

	for (int i = memory_ - step; i < memory_; i++) vector_[i] = 0;
}

void BoolVector::operator >>=(int count)
{
	BoolVector oldVector(*this);

	int step = count / 8;
	count %= 8;

	unsigned char mask = ~0;
	mask >>= (8 - count);
	unsigned char saveBits = 0;

	for (int i = memory_ - 1; i >= step; i--)
	{
		vector_[i] = oldVector.vector_[i - step] >> count;
		if (i != memory_ - 1)
		{
			saveBits = (oldVector.vector_[i + step] & mask) << (8 - count);
			vector_[i + 1] |= saveBits;
		}
	}

	for (int i = step - 1; i >= 0; i--) vector_[i] = 0;
}

BoolVector BoolVector::operator ~() const
{
	BoolVector invVector = *this;
	invVector.invertVector();
	return invVector;
}

unsigned long long BoolVector::getNum()
{
	unsigned long long res = 0;
	for (int i = 0; i < memory_; i++)
	{
		res <<= 8;
		res += vector_[i];
	}
	return res;
}