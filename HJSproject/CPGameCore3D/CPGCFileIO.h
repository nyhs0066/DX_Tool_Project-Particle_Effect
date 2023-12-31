#pragma once
#include "CPGCStd.h"

static const char checkCharSet[] = { '\\', '/', '?', '%', '*', '|', '\"', ':', '<', '>', '.', '\''};
static const char checkWcharSet[] = { L'\\', L'/', L'?', L'%', L'*', L'|', L'\"', L':', L'<', L'>', L'.', L'\'' };

#define MAX_NUMBER_OF_BYTE_TO_READ			1024
#define MAX_NUMBER_OF_BYTE_TO_WRITE			1024

class CPGCFileIOManager
{
private:
	static BYTE*				m_pfileBuf;				//파일 버퍼
	static LARGE_INTEGER		m_fileSize;
	static DWORD				m_numberOfFragment;
	static DWORD				m_curOPCount;

	//이벤트 핸들
	static HANDLE				m_hOPStart;
	static HANDLE				m_hProgressPrint;

	CPGCFileIOManager();
	~CPGCFileIOManager();
public:
	CPGCFileIOManager(const CPGCFileIOManager& other) = delete;
	CPGCFileIOManager& operator=(const CPGCFileIOManager& other) = delete;

public:
	bool init();
	bool release();

	size_t loadFile(std::wstring wszFilePath, void* destLoadData);
	size_t saveFile(std::wstring wszFilePath, void* srcSaveData, UINT iBytes);

	static UINT WINAPI asyncReadProc(LPVOID argList);
	static UINT WINAPI asyncWriteProc(LPVOID argList);

	static bool asyncReadSub(HANDLE handle, DWORD dwOffset, LARGE_INTEGER& readBytes, OVERLAPPED& readOV);
	static bool asyncWriteSub(HANDLE handle, DWORD dwOffset, LARGE_INTEGER& writeBytes, OVERLAPPED& writeOV);

	static CPGCFileIOManager& getInstance()
	{
		static CPGCFileIOManager singleInst;
		return singleInst;
	}
};

#define CPGC_FILEIO_MGR CPGCFileIOManager::getInstance()

bool getSplitName(std::string& szFullPath, std::wstring& wszOutfileName);
bool getSplitName(std::wstring& wszFullPath, std::wstring& wszOutfileName);

bool getSplitDir(std::string& szFullPath, std::wstring& wszOutDirName, bool bContainFileName = false);
bool getSplitDir(std::wstring& wszFullPath, std::wstring& wszOutDirName, bool bContainFileName = false);

bool getSplitName(std::string& szFullPath, std::vector<std::wstring>* pWszInExtArr, std::wstring& szOutfileName);
bool getSplitName(std::wstring& wszFullPath, std::vector<std::wstring>* pWszInExtArr, std::wstring& szOutfileName);

bool loadDir(std::wstring wszDirName,
	std::vector<std::wstring>* pWszInExtArr,
	std::vector<std::wstring>* pWszOutPathArr,
	std::vector<std::string>* pSzOutNameArr);

bool loadDir(std::wstring wszDirName,
	std::vector<std::wstring>* pWszInExtArr,
	std::vector<std::wstring>* pWszOutPathArr,
	std::vector<std::wstring>* pWszOutNameArr);

bool isValidName(std::string szName);
bool isValidName(std::wstring wszName);