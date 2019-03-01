#include <Config/LJSONFile.h>
#include <Log.h>

#define TAB "    "

using namespace FlagGG::Config;

void Print(const LJSONValue& root, const std::string& t = "")
{
	if (root.IsObject())
	{
		printf("\n%s{\n", t.c_str());
		for (auto it = root.Begin(); it != root.End(); ++it)
		{
			const LJSONValue& value = it->second;
			printf("%s" TAB "%s = ", t.c_str(), it->first.c_str());
			Print(value, t + TAB);
		}
		printf("%s}\n", t.c_str());
	}
	else if (root.IsArray())
	{
		printf("\n%s[\n", t.c_str());
		for (uint32_t i = 0; i < root.Size(); ++i)
		{
			const LJSONValue& value = root[i];
			printf("%s" TAB, t.c_str());
			Print(value);
		}
		printf("%s]\n", t.c_str());
	}
	else if (root.IsBool())
	{
		printf("%s,\n", root.GetBool() ? "true" : "false");
	}
	else if (root.IsNumber())
	{
		printf("%lf,\n", root.GetDouble());
	}
	else if (root.IsString())
	{
		const std::string& content = root.GetString();
		bool flag = content.find('\n');
		printf("%s\"%s\"%s,\n", flag ? "(" : "", content.c_str(), flag ? ")" : "");
	}
}

void Run()
{
	LJSONFile jsonFile;
	if (!jsonFile.LoadFile(L"../../../Samples/Test/TestConfig.ljson"))
	{
		FLAGGG_LOG_DEBUG("fuck.");

		return;
	}

	const LJSONValue& root = jsonFile.GetRoot();
	Print(root);
}

int main()
{
	Run();

	getchar();

	return 0;
}

