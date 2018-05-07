#pragma once
#include <string>
#include <vector>
#include <iostream>

class Double {
public:
	Double(int);
	Double(std::string&);
	Double();
	Double(const Double&);
	Double operator=(const Double& op);

	std::string ToString() const;

	friend Double operator+(const Double&, const Double&);
	friend Double operator-(const Double&, const Double&);
	friend Double operator*(const Double&, const Double&);
	friend Double operator/(const Double&, const Double&);

	friend Double operator-(const Double&);   //negative

	friend bool operator==(const Double&, const Double&);
	friend bool operator==(const Double&, const Double&);
	friend bool operator<(const Double&, const Double&);
	friend bool operator<=(const Double&, const Double&);
	friend bool operator>(const Double&, const Double&);
	friend bool operator>=(const Double&, const Double&);

	friend Double operator+=(Double&, const Double&);
	friend Double operator-=(Double&, const Double&);
	friend Double operator*=(Double&, const Double&);
	friend Double operator/=(Double&, const Double&);

private:
	void trim();

private:
	std::vector<char> Integer;
	std::vector<char> Decimal;
	bool Tag = true;
};