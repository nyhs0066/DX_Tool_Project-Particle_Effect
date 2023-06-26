#include "CPRNGenerator.h"

CPRNGenerator::CPRNGenerator() : gen(rd())
{
	setI_Range(0, 1);
	setLL_Range(0, 1);
	setF_Range(0.0f, 1.0f);
	setD_Range(0.0f, 1.0f);
}

CPRNGenerator::~CPRNGenerator()
{
}

void CPRNGenerator::setI_Range(int min, int max)
{
	if (min > max)
	{
		auto temp = min;
		min = max;
		max = temp;
	}

	iParam._Init(min, max);
	iDist.param(iParam);
}

void CPRNGenerator::setLL_Range(long long min, long long max)
{
	if (min > max)
	{
		auto temp = min;
		min = max;
		max = temp;
	}

	llParam._Init(min, max);
	llDist.param(llParam);
}

void CPRNGenerator::setF_Range(float min, float max)
{
	if (min > max)
	{
		auto temp = min;
		min = max;
		max = temp;
	}

	fParam._Init(min, max);
	fDist.param(fParam);
}

void CPRNGenerator::setD_Range(double min, double max)
{
	if (min > max)
	{
		auto temp = min;
		min = max;
		max = temp;
	}

	dParam._Init(min, max);
	dDist.param(dParam);
}

std::pair<int, int> CPRNGenerator::getI_Range()
{
	return std::pair<int, int>(iParam._Min, iParam._Max);
}

std::pair<long long, long long> CPRNGenerator::getLL_Range()
{
	return std::pair<long long, long long>(llParam._Min, llParam._Max);
}

std::pair<float, float> CPRNGenerator::getF_Range()
{
	return std::pair<float, float>(fParam._Min, fParam._Max);
}

std::pair<double, double> CPRNGenerator::getD_Range()
{
	return std::pair<double, double>(dParam._Min, dParam._Max);
}

int CPRNGenerator::getI()
{
	return iDist(gen);
}

int CPRNGenerator::getI(int min, int max)
{
	if (min > max)
	{
		auto temp = min;
		min = max;
		max = temp;
	}

	std::uniform_int<int>::param_type temp(min, max);
	iTDist.param(temp);
	return iTDist(gen);
}

long long CPRNGenerator::getLL()
{
	return llDist(gen);
}

long long CPRNGenerator::getLL(long long min, long long max)
{
	if (min > max)
	{
		auto temp = min;
		min = max;
		max = temp;
	}

	std::uniform_int<long long>::param_type temp(min, max);
	llTDist.param(temp);
	return llTDist(gen);
}

float CPRNGenerator::getF()
{
	return fDist(gen);
}

float CPRNGenerator::getF(float min, float max)
{
	if (min > max)
	{
		auto temp = min;
		min = max;
		max = temp;
	}

	std::uniform_real<float>::param_type temp(min, max);
	fTDist.param(temp);
	return fTDist(gen);
}

double CPRNGenerator::getD()
{
	return dDist(gen);
}

double CPRNGenerator::getD(double min, double max)
{
	if (min > max)
	{
		auto temp = min;
		min = max;
		max = temp;
	}

	std::uniform_real<double>::param_type temp(min, max);
	dTDist.param(temp);
	return dTDist(gen);
}