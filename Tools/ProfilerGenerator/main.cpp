#include <string>
#include <fstream>
#include <list>
#include <map>

#include <windows.h>

static bool ReadFileToLineVector(const std::string& strFilePath, std::list<std::string>& vecOut)
{
	std::ifstream stream;
	stream.open(strFilePath, std::ios::in);
	if (!stream.is_open())
	{
		return false;
	}

	while (!stream.eof())
	{
		std::string line;
		std::getline(stream, line);
		vecOut.push_back(std::move(line));
	}

	stream.close();

	return true;
}

static bool WriteLineVectorToFile(const std::list<std::string>& VecIn, const std::string& strFilePath)
{
	std::ofstream stream;
	stream.open(strFilePath, std::ios::trunc | std::ios::out);
	if (!stream.is_open())
	{
		return false;
	}

	for (const auto& line : VecIn)
	{
		stream << line << "\n";
	}

	stream.close();

	return true;
}

static bool GetProfilerInfo(const std::string& strLineCode, std::string& strOutName, bool& bHasLeft)
{
	const char* st = strLineCode.c_str();
	const char* ed = st + strLineCode.length();
	while (st != ed && ((*st) == ' ' || (*st) == '\t')) ++st;
	while (st != ed && (*st) != ' ' && (*st) != '\t') ++st;
	while (st != ed && ((*st) == ' ' || (*st) == '\t')) ++st;

	strOutName = "";
	while (st != ed && (*st) != '(')
	{
		strOutName += (*st);
		++st;
	}

	if (st != ed && (*st) == '(' && !strOutName.empty() && strOutName.find("::") != std::string::npos)
	{
		while (st != ed && (*st) != '{') ++st;
		bHasLeft = st != ed && (*st) == '{';
		return true;
	}

	return false;
}

static bool IsLeft(const std::string& strLineCode)
{
	const char* st = strLineCode.c_str();
	const char* ed = st + strLineCode.length();
	while (st != ed && ((*st) == ' ' || (*st) == '\t')) ++st;
	return st != ed && (*st) == '{';
}

struct ProfilerInfo
{
	std::string strVariName;
	std::string strShowName;
	std::string strTypeName;
};

static void InsertProfiler(const std::string& strFilePath, const std::map<std::string, ProfilerInfo>& mapInfo)
{
	std::list<std::string> vecLine;
	if (!ReadFileToLineVector(strFilePath, vecLine))
	{
		return;
	}

	std::map<std::string, int> mapFuncAppearNum;
	for (auto it = vecLine.begin(); it != vecLine.end(); ++it)
	{
		std::string strFuncName;
		bool bHasLeft = false;
		if (GetProfilerInfo(*it, strFuncName, bHasLeft))
		{
			auto itInfo = mapInfo.find(strFuncName);
			if (itInfo != mapInfo.end())
			{
				auto info = itInfo->second;

				if (mapFuncAppearNum.find(strFuncName) == mapFuncAppearNum.end())
				{
					mapFuncAppearNum[strFuncName] = 1;
				}
				else
				{
					int nNum = ++mapFuncAppearNum[strFuncName];
					std::string strNum = std::to_string(nNum);
					info.strVariName += strNum;
					info.strShowName += strNum;
				}

				std::string strProfilerDefine;
				strProfilerDefine += "PROFILER_INFORMATION(";
				strProfilerDefine += info.strVariName + ", \"";
				strProfilerDefine += info.strShowName + "\", ";
				strProfilerDefine += info.strTypeName + ");";

				vecLine.insert(it, strProfilerDefine);

				++it;

				std::string strProfilerNode;
				strProfilerNode += "    PROFILER_AUTO(";
				strProfilerNode += info.strVariName;
				strProfilerNode += ", nullptr);";
				if (bHasLeft)
				{
					vecLine.insert(it, strProfilerNode);
				}
				else if(it != vecLine.end())
				{
					++it;
					vecLine.insert(it, strProfilerNode);
				}

				if (it == vecLine.end())
					break;
			}
		}
	}

	if (!WriteLineVectorToFile(vecLine, strFilePath))
	{
		printf("Failed to WriteLineVectorToFile(%s).\n", strFilePath.c_str());
	}
}

static void Run(const char* pszFilePath)
{
	char szWorkDir[MAX_PATH] = { 0 };
	::GetPrivateProfileString("FilePath", "WorkDir", "", szWorkDir, MAX_PATH, pszFilePath);
	std::string strWorkDir(szWorkDir);

	int nNumFile = ::GetPrivateProfileInt("FilePath", "Size", 0, pszFilePath);
	for (int i = 1; i <= nNumFile; ++i)
	{
		char szIndex[20] = { 0 };
		sprintf(szIndex, "%d", i);

		char szRelativeFilePath[MAX_PATH] = { 0 };
		int nFileLen = ::GetPrivateProfileStringA("FilePath", szIndex, "", szRelativeFilePath, MAX_PATH, pszFilePath);
		if (nFileLen > 0)
		{
			std::string strAbsolutePath = strWorkDir + szRelativeFilePath;
			int nNumProfiler = ::GetPrivateProfileInt(szRelativeFilePath, "Size", 0, pszFilePath);
			if (nNumProfiler > 0)
			{
				std::map<std::string, ProfilerInfo> mapInfo;
				for (int j = 1; j <= nNumProfiler; ++j)
				{
					std::string strFuncName;
					{
						char szIndex2[20] = { 0 };
						char szValue[128] = { 0 };
						sprintf(szIndex2, "%d_FuncName", j);
						::GetPrivateProfileStringA(szRelativeFilePath, szIndex2, "", szValue, 128, pszFilePath);
						strFuncName = szValue;
					}

					ProfilerInfo& info = mapInfo[strFuncName];

					{
						char szIndex2[20] = { 0 };
						char szValue[128] = { 0 };
						sprintf(szIndex2, "%d_VariName", j);
						::GetPrivateProfileStringA(szRelativeFilePath, szIndex2, "", szValue, 128, pszFilePath);
						info.strVariName = szValue;
					}

					{
						char szIndex2[20] = { 0 };
						char szValue[128] = { 0 };
						sprintf(szIndex2, "%d_ShowName", j);
						::GetPrivateProfileStringA(szRelativeFilePath, szIndex2, "", szValue, 128, pszFilePath);
						info.strShowName = szValue;
					}

					{
						char szIndex2[20] = { 0 };
						char szValue[128] = { 0 };
						sprintf(szIndex2, "%d_TypeName", j);
						::GetPrivateProfileStringA(szRelativeFilePath, szIndex2, "", szValue, 128, pszFilePath);
						info.strTypeName = szValue;
					}
				}

				InsertProfiler(strAbsolutePath, mapInfo);
			}
		}
	}
}

int main(int argc, const char* argv[])
{
	const char* pszFilePath = "E:/FlagGG/bin/Debug/config.ini";
	if (argc > 1)
	{
		pszFilePath = argv[1];
	}

	Run(pszFilePath);

	return 0;
}
