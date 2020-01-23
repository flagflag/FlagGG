#include <Config/LJSONFile.h>
#include <Log.h>

#include <Container/Variant.h>

#include <functional>
#include <tuple>

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
		for (UInt32 i = 0; i < root.Size(); ++i)
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

void Run2()
{
	FlagGG::Container::DefaultVariant v1(100);
	FlagGG::Container::DefaultVariant v2 = 110u;
	FlagGG::Container::DefaultVariant v3(FlagGG::Container::String("2333"));
	FlagGG::Container::DefaultVariant v4(0.1f);

	printf("%d %u %s %f\n", v1.Get<int>(), v2.Get<unsigned>(), v3.Get<FlagGG::Container::String>().CString(), v4.Get<float>());

	FlagGG::Container::DefaultVariant v5(v1);
	FlagGG::Container::DefaultVariant v6 = v2;
	FlagGG::Container::DefaultVariant v7;
	FlagGG::Container::DefaultVariant v8;
	v7 = v3;
	v8 = FlagGG::Container::DefaultVariant(0.1f);

	printf("%d %u %s %f\n", v5.Get<int>(), v6.Get<unsigned>(), v7.Get<FlagGG::Container::String>().CString(), v8.Get<float>());

	v1 = 0.1f;
	v2 = FlagGG::Container::String("2333");
	v3.Set(110u);
	v4.Set(100);

	printf("%f %s %u %d\n", v1.Get<float>(), v2.Get<FlagGG::Container::String>().CString(), v3.Get<unsigned>(), v4.Get<int>());

	FlagGG::Container::DefaultVariant test;
	test = FlagGG::Math::Vector2(100, 100);

	printf("(%f, %f)\n", test.Get<FlagGG::Math::Vector2>().x_, test.Get<FlagGG::Math::Vector2>().y_);

	auto temp = [](const FlagGG::Math::Vector2& value)
	{
		printf("Visit ==> Vector2(%f, %f)\n", value.x_, value.y_);
	};

	test.Visit(temp);
}

int main()
{
	// Run();

	Run2();

	getchar();

	return 0;
}

