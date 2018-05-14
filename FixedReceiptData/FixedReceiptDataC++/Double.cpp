#include <deque>
#include <sstream>

#include "Double.h"

#define ACCURACY 100

static const Double ZERO = Double(0);
static const Double ONE = Double(1);
static const Double TEN = Double(10);

Double::Double(int val) {
	if (val >= 0)
		Tag = true;
	else {
		Tag = false;
		val *= (-1);
	}
	do {
		Integer.push_back((char)(val % 10));
		val /= 10;
	} while (val != 0);
	Decimal.push_back(0);
}

Double::Double() {
	Tag = true;
	Integer.push_back(0);
	Decimal.push_back(0);
}

Double::Double(const std::string& def) {
	bool type = true;

	for (std::string::const_iterator iter = def.begin(); iter != def.end(); iter++) {
		char ch = (*iter);
		if (iter == def.begin()) {
			if (ch == '+')
				continue;
			if (ch == '-') {
				Tag = false;
			}
		}

		if (ch == '.') {
			type = false;
			continue;
		}

		if (type)
			Integer.push_back((char)((*iter) - '0')); 
		else 
			Decimal.push_back((char)((*iter) - '0'));
	}

	std::reverse(Integer.begin(), Integer.end());
	std::reverse(Decimal.begin(), Decimal.end());

	Trim();
}

Double::Double(const Double &op) {
	Integer = op.Integer;
	Decimal = op.Decimal;
	Tag = op.Tag;
}

Double Double::operator=(const Double& op) {
	Integer = op.Integer;
	Decimal = op.Decimal;
	Tag = op.Tag;
	return (*this);
}

std::string Double::ToString() const {
	std::ostringstream stringStream;
	if (!Tag)
		stringStream << "-";
	for (std::vector<char>::const_reverse_iterator iter = Integer.rbegin(); iter != Integer.rend(); iter++)
		stringStream << (char)((*iter) + '0');
	stringStream << '.';
	for (std::vector<char>::const_reverse_iterator iter = Decimal.rbegin(); iter != Decimal.rend(); iter++)
		stringStream << (char)((*iter) + '0');
	return stringStream.str();
}

Double operator-(const Double& op) {
	Double temp(op);
	temp.Tag = !temp.Tag;
	return temp;
}

Double operator+=(Double& op1, const Double& op2) {
	if (op1.Tag == op2.Tag) {
		std::vector<char>::iterator iter1;
		std::vector<char>::const_iterator iter2, it;
		//处理小数部分
		int a = (int)op1.Decimal.size();
		int b = (int)op2.Decimal.size();
		char to_add = 0;     //进位
		if (a<b) {
			iter1 = op1.Decimal.begin();
			iter2 = op2.Decimal.begin();
			iter2 = iter2 - (a - b);

			while (iter1 != op1.Decimal.end() && iter2 != op2.Decimal.end()) {
				(*iter1) = (*iter1) + (*iter2) + to_add;
				to_add = ((*iter1) > 9);    // 大于9进一位
				(*iter1) = (*iter1) % 10;
				iter1++; iter2++;
			}
			it = op2.Decimal.begin();
			iter2 = op2.Decimal.end();
			iter2 = iter2 - a - 1;
			while (iter2 != it) {
				op1.Decimal.insert(op1.Decimal.begin(), *iter2);
				iter2--;
			}//此处应该用反向迭代器
			op1.Decimal.insert(op1.Decimal.begin(), *iter2);
			iter1 = op1.Decimal.begin();
		}
		else if (a>b) {
			iter1 = op1.Decimal.begin();
			iter1 = iter1 + (a - b);
			iter2 = op2.Decimal.begin();
			while (iter1 != op1.Decimal.end() && iter2 != op2.Decimal.end()) {
				(*iter1) = (*iter1) + (*iter2) + to_add;
				to_add = ((*iter1) > 9);    // 大于9进一位
				(*iter1) = (*iter1) % 10;
				iter1++; iter2++;
			}
		}
		else {
			iter1 = op1.Decimal.begin();
			iter2 = op2.Decimal.begin();
			while (iter1 != op1.Decimal.end() && iter2 != op2.Decimal.end()) {
				(*iter1) = (*iter1) + (*iter2) + to_add;
				to_add = ((*iter1) > 9);    // 大于9进一位
				(*iter1) = (*iter1) % 10;
				iter1++; iter2++;
			}
		}

		//处理整数部分
		iter1 = op1.Integer.begin();
		iter2 = op2.Integer.begin();
		//进位
		while (iter1 != op1.Integer.end() && iter2 != op2.Integer.end()) {
			(*iter1) = (*iter1) + (*iter2) + to_add;
			to_add = ((*iter1) > 9);    // 大于9进一位
			(*iter1) = (*iter1) % 10;
			iter1++; iter2++;
		}
		while (iter1 != op1.Integer.end()) {   // 
			(*iter1) = (*iter1) + to_add;
			to_add = ((*iter1) > 9);
			(*iter1) %= 10;
			iter1++;
		}
		while (iter2 != op2.Integer.end()) {
			char val = (*iter2) + to_add;
			to_add = (val > 9);
			val %= 10;
			op1.Integer.push_back(val);
			iter2++;
		}
		if (to_add != 0)
			op1.Integer.push_back(to_add);
		return op1;
	}
	else {
		if (op1.Tag)
			return op1 -= (-op2);
		else
			return op1 = op2 - (-op1);
	}
}

Double operator-=(Double& op1, const Double& op2) {
	if (op1.Tag == op2.Tag) {
		if (op1.Tag) {
			if (op1 < op2) {
				Double op(op2 - op1);
				op1 = -op;
				return op1;
			}
		}
		else {
			if (-op1 > -op2)
				return op1 = -((-op1) - (-op2));
			else
				return op1 = (-op2) - (-op1);
		}
		
		char to_substract = 0;
		int a = (int)op1.Decimal.size();
		int b = (int)op2.Decimal.size();
		std::vector<char>::iterator it1 = op1.Decimal.begin();
		std::vector<char>::const_iterator it2 = op2.Decimal.begin();
		if (a>b) {
			a -= b;
			it1 = it1 + a;
		}
		else {
			int number = b - a;
			while (number != 0)
			{
				op1.Decimal.insert(op1.Decimal.begin(), 0);
				number--;
			}
			it1 = op1.Decimal.begin();
		}
		while (it1 != op1.Decimal.end() && it2 != op2.Decimal.end()) {
			(*it1) = (*it1) - (*it2) - to_substract;
			to_substract = 0;
			if ((*it1) < 0) {
				to_substract = 1;
				(*it1) += 10;
			}
			it1++;
			it2++;
		}
		
		std::vector<char>::iterator iter1;
		std::vector<char>::const_iterator iter2;
		iter1 = op1.Integer.begin();
		iter2 = op2.Integer.begin();

		while (iter1 != op1.Integer.end() && iter2 != op2.Integer.end()) {
			(*iter1) = (*iter1) - (*iter2) - to_substract;
			to_substract = 0;
			if ((*iter1) < 0) {
				to_substract = 1;
				(*iter1) += 10;
			}
			iter1++;
			iter2++;
		}
		while (iter1 != op1.Integer.end()) {
			(*iter1) = (*iter1) - to_substract;
			to_substract = 0;
			if ((*iter1) < 0) {
				to_substract = 1;
				(*iter1) += 10;
			}
			else break;
			iter1++;
		}
		op1.Trim();
		return op1;
	}
	else {
		if (op1 > ZERO)
			return op1 += (-op2);
		else
			return op1 = -(op2 + (-op1));
	}
}

Double operator*=(Double& op1, const Double& op2) {
	Double result(0);
	if (op1 == ZERO || op2 == ZERO)
		result = ZERO;
	else {
		int size = 0;
		std::vector<char> op_temp1(op1.Integer.begin(), op1.Integer.end());
		if (op1.Decimal.size() != 1 || (op1.Decimal.size() == 1 && (*op1.Decimal.begin()) != 0)) {
			op_temp1.insert(op_temp1.begin(), op1.Decimal.begin(), op1.Decimal.end());
			size += (int)op1.Decimal.size();
		}
		std::vector<char> op_temp2(op2.Integer.begin(), op2.Integer.end());
		if (op2.Decimal.size() != 1 || (op2.Decimal.size() == 1 && (*op2.Decimal.begin()) != 0)) {
			op_temp2.insert(op_temp2.begin(), op2.Decimal.begin(), op2.Decimal.end());
			size += (int)op2.Decimal.size();
		}
		std::vector<char>::const_iterator iter2 = op_temp2.begin();
		while (iter2 != op_temp2.end()) {
			if (*iter2 != 0) {
				std::deque<char> temp(op_temp1.begin(), op_temp1.end());
				char to_add = 0;
				std::deque<char>::iterator iter1 = temp.begin();
				while (iter1 != temp.end()) {
					(*iter1) *= (*iter2);
					(*iter1) += to_add;
					to_add = (*iter1) / 10;
					(*iter1) %= 10;
					iter1++;
				}
				if (to_add != 0)
					temp.push_back(to_add);
				int64_t num_of_zeros = iter2 - op_temp2.begin();
				while (num_of_zeros--)
					temp.push_front(0);
				Double temp2;
				temp2.Integer.clear();
				temp2.Integer.insert(temp2.Integer.end(), temp.begin(), temp.end());
				temp2.Trim();
				result = result + temp2;
			}
			iter2++;
		}
		result.Tag = ((op1.Tag && op2.Tag) || (!op1.Tag && !op2.Tag));
		if (size != 0) {
			result.Decimal.clear();
			result.Decimal.insert(result.Decimal.begin(), result.Integer.begin(), result.Integer.begin() + size);
			result.Integer.erase(result.Integer.begin(), result.Integer.begin() + size);
		}
	}
	op1 = result;
	return op1;
}

Double operator/=(Double& op1, const Double& op2) {
	if (op2 == ZERO)
		throw std::exception("Divided By Zero Exception");

	if (op1 == ZERO)
		return op1;

	Double op_temp2 = op2;
	Double op_temp1 = op1;
	int Integer_Size = 0;

	if ((op_temp2.Decimal.size() == 1) && (*(op_temp2.Decimal.begin()) == 0)) {}
	else {
		int t = (int)op_temp2.Decimal.size();
		while (t--) {
			op_temp1 = op_temp1 * TEN;
			op_temp2 = op_temp2 * TEN;
		}
	}

	if (op_temp1<op_temp2) {
		while (op_temp1>op_temp2) {
			op_temp1 *= TEN;
			Integer_Size--;
		}
	}
	else {
		while (op_temp1>op_temp2) {
			op_temp1.Decimal.push_back(*op_temp1.Integer.begin());
			op_temp1.Integer.erase(op_temp1.Integer.begin());
			Integer_Size++;
		}
	}

	int k = ACCURACY;
	Double re(0);

	while (k--) {
		if (op_temp1<op_temp2) {
			op_temp1 = op_temp1 * TEN;
			re = re * TEN;
		}
		else {
			int i;
			Double compare;
			for (i = 2; i<10; i++) {
				Double BF(i);
				compare = op_temp2 * BF;
				if (compare>op_temp1)
					break;
			}
			compare -= op_temp2;
			op_temp1 -= compare;
			Double index(i - 1);
			re = re + index;
		}
	}

	if (re.Integer.size()>Integer_Size) {
		std::vector<char> temp(re.Integer.begin(), re.Integer.end());
		re.Integer.assign(temp.end() - Integer_Size, temp.end());
		re.Decimal.insert(re.Decimal.begin(), temp.begin(), temp.end() - Integer_Size);
	}
	else {
		int t = Integer_Size - (int)re.Integer.size();
		while (t--) {
			re = re * TEN;
		}
	}

	op1 = re;
	op1.Trim();
	op1.Tag = ((op1.Tag && op2.Tag) || (!op1.Tag && !op2.Tag));

	return op1;
}

Double operator+(const Double& op1, const Double& op2) {
	Double temp(op1);
	temp += op2;
	temp.Trim();
	return temp;
}

Double operator-(const Double& op1, const Double& op2) {
	Double temp(op1);
	temp -= op2;
	temp.Trim();
	return temp;
}

Double operator*(const Double& op1, const Double& op2) {
	Double temp(op1);
	temp *= op2;
	temp.Trim();
	return temp;
}

Double operator/(const Double& op1, const Double& op2) {
	Double temp(op1);
	temp /= op2;
	temp.Trim();
	return temp;
}

bool operator<(const Double& op1, const Double& op2) {
	bool sign;
	if (op1.Tag != op2.Tag) {
		sign = !op1.Tag;
		return sign;
	}
	else {
		if (op1.Integer.size() != op2.Integer.size()) {
			if (op1.Tag) {
				sign = op1.Integer.size()<op2.Integer.size();
				return sign;
			}
			else {
				sign = op1.Integer.size()>op2.Integer.size();
				return sign;
			}
		}

		std::vector<char>::const_reverse_iterator iter1, iter2;
		iter1 = op1.Integer.rbegin();
		iter2 = op2.Integer.rbegin();

		while (iter1 != op1.Integer.rend()) {
			if (op1.Tag &&  *iter1 < *iter2) return true;
			if (op1.Tag &&  *iter1 > *iter2) return false;
			if (!op1.Tag &&  *iter1 > *iter2) return true;
			if (!op1.Tag &&  *iter1 < *iter2) return false;
			iter1++;
			iter2++;
		}

		std::vector<char>::const_reverse_iterator it1, it2;
		it1 = op1.Decimal.rbegin();
		it2 = op2.Decimal.rbegin();

		while (it1 != op1.Decimal.rend() && it2 != op2.Decimal.rend()) {
			if (op1.Tag &&  *it1 < *it2) return true;
			if (op1.Tag &&  *it1 > *it2) return false;
			if (!op1.Tag &&  *it1 > *it2) return true;
			if (!op1.Tag &&  *it1 < *it2) return false;
			it1++;
			it2++;
		}
		return (op1.Tag && it2 != op2.Decimal.rend())
			|| (!op1.Tag && it1 != op1.Decimal.rend());
	}
}

bool operator>(const Double& op1, const Double& op2) {
	bool tag = !(op1 <= op2);
	return tag;
}

bool operator==(const Double& op1, const Double& op2) {
	if (op1.Tag == (!op2.Tag)) {
		return false;
	}

	if (op1.Integer.size() != op2.Integer.size()) {
		return false;
	}

	if (op1.Decimal.size() != op2.Decimal.size()) {
		return false;
	}

	std::vector<char>::const_iterator iter1, iter2;
	iter1 = op1.Decimal.begin();
	iter2 = op2.Decimal.begin();

	while (iter1 != op1.Decimal.end()) {
		if (*iter1 != *iter2)  return false;
		iter1++;
		iter2++;
	}

	iter1 = op1.Integer.begin();
	iter2 = op2.Integer.begin();

	while (iter1 != op1.Integer.end()) {
		if (*iter1 != *iter2)  return false;
		iter1++;
		iter2++;
	}

	return true;
}

bool operator!=(const Double& op1, const Double& op2) {
	return !(op1 == op2);
}

bool operator>=(const Double& op1, const Double& op2) {
	bool tag = (op1>op2) || (op1 == op2);
	return tag;
}

bool operator<=(const Double& op1, const Double& op2) {
	bool tag = (op1<op2) || (op1 == op2);
	return tag;
}

void Double::Trim() {
	std::vector<char>::reverse_iterator iter = Integer.rbegin();
	while (!Integer.empty() && (*iter) == 0) {
		Integer.pop_back();
		iter = Integer.rbegin();
	}

	if (Integer.size() == 0) {
		Integer.push_back(0);
	}

	std::vector<char>::const_iterator it = Decimal.begin();
	while (!Decimal.empty() && (*it) == 0) {
		it = Decimal.erase(it);
	}

	if (Decimal.size() == 0) {
		Decimal.push_back(0);
	}
}
