#pragma once



class EffectFileIOManager
{
private:
	EffectFileIOManager();
	~EffectFileIOManager();
public:
	EffectFileIOManager(const EffectFileIOManager& other) = delete;
	EffectFileIOManager& operator=(const EffectFileIOManager& other) = delete;

public:
	static EffectFileIOManager& getInstance()
	{
		static EffectFileIOManager singleInst;
		return singleInst;
	}
};

#define FX_IO_MGR EffectFileIOManager::getInstance()
