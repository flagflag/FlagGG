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
			const LJSONValue& value = it->second_;
			printf("%s" TAB "%s = ", t.c_str(), it->first_.CString());
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
		const FlagGG::Container::String& content = root.GetString();
		bool flag = content.Find('\n');
		printf("%s\"%s\"%s,\n", flag ? "(" : "", content.CString(), flag ? ")" : "");
	}
}

void Run()
{
	FlagGG::Core::Context context;
	LJSONFile jsonFile(&context);
	if (!jsonFile.LoadFile("../../../Samples/Test/TestConfig.ljson"))
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

