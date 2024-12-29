#include <Container/Vector.h>
#include <Container/List.h>
#include <Container/HashMap.h>
#include <Container/HashSet.h>
#include <Container/Str.h>
#include <Memory/Malloc/MallocStomp.h>
#include <Memory/MemoryHook.h>

using namespace FlagGG;

void ContainerMemoryTest()
{
	{
		Vector<String> strVec;
		for (UInt32 i = 0; i < 10; ++i)
		{
			strVec.Push(String(i * 233));
		}
	}

	{
		HashMap<String, String> strMap;
		for (UInt32 i = 0; i < 10; ++i)
		{
			strMap.Insert(MakePair(String(i * 666), String(i * 111)));
		}
	}

	{
		HashSet<String> strSet;
		for (UInt32 i = 0; i < 10; ++i)
		{
			strSet.Insert(String(i * 555));
		}
	}

	{
		List<String> strList;
		for (UInt32 i = 0; i < 10; ++i)
		{
			strList.Push(String(i * 999));
		}
	}
}

void MallocStompTest()
{
	const UInt32 size = 100;
	char* tempMemory = new char[size];
	// tempMemory[-15] = '\0';
	// 注意：内存分配是16字节对齐，因此需要反问>112的地址才能触发异常
	tempMemory[size + 12] = '\0';
}

int main()
{
	ContainerMemoryTest();

	MallocStompTest();

	return 0;
}

IMPLEMENT_MODULE_USD("MemoryTest");
