//////////////////////////////////////////////////
//
// CPRNGenerator.h
//		랜덤 난수 생성 엔진을 이용한 난수 생성 헤더
// 
//////////////////////////////////////////////////

#pragma once
#include "CPGCStd.h"
#include <random>

class CPRNGenerator
{
private:
	std::random_device rd;
	std::default_random_engine gen;				//mt19937 메르센 트위스터 난수 엔진

	std::uniform_int<int>::param_type			iParam;
	std::uniform_int<long long>::param_type		llParam;
	std::uniform_real<float>::param_type		fParam;
	std::uniform_real<double>::param_type		dParam;

	std::uniform_int_distribution<int>			iDist;
	std::uniform_int_distribution<long long>	llDist;
	std::uniform_real_distribution<float>		fDist;
	std::uniform_real_distribution<double>		dDist;

	std::uniform_int_distribution<int>			iTDist;
	std::uniform_int_distribution<long long>	llTDist;
	std::uniform_real_distribution<float>		fTDist;
	std::uniform_real_distribution<double>		dTDist;

public:
	CPRNGenerator(const CPRNGenerator& other) = delete;
	CPRNGenerator& operator=(const CPRNGenerator& other) = delete;

public:
	CPRNGenerator();
	~CPRNGenerator();

	void setI_Range(int min, int max);
	void setLL_Range(long long min, long long max);
	void setF_Range(float min, float max);
	void setD_Range(double min, double max);

	std::pair<int, int> getI_Range();
	std::pair<long long, long long> getLL_Range();
	std::pair<float, float> getF_Range();
	std::pair<double, double> getD_Range();

	int getI();
	int getI(int min, int max);
	long long getLL();
	long long getLL(long long min, long long max);
	float getF();
	float getF(float min, float max);
	double getD();
	double getD(double min, double max);

	static CPRNGenerator& getInstance()
	{
		static CPRNGenerator singleInst;
		return singleInst;
	}
};

#define CPGC_RNG CPRNGenerator::getInstance()
