﻿#include <Config/LJSONFile.h>
#include <Log.h>

#include <Container/Variant.h>
#include <Memory/MemoryHook.h>

#include <functional>
#include <tuple>

#define TAB "    "

using namespace FlagGG;

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
		const String& content = root.GetString();
		bool flag = content.Find('\n');
		printf("%s\"%s\"%s,\n", flag ? "(" : "", content.CString(), flag ? ")" : "");
	}
}

void Run()
{
	LJSONFile jsonFile;
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
	FVariant v1(100);
	FVariant v2 = 110u;
	FVariant v3(String("2333"));
	FVariant v4(0.1f);

	printf("%d %u %s %f\n", v1.Get<int>(), v2.Get<unsigned>(), v3.Get<String>().CString(), v4.Get<float>());

	const FVariant& cv3 = v3;
	String t = cv3.Get<String>();

	FVariant v5(v1);
	FVariant v6 = v2;
	FVariant v7;
	FVariant v8;
	v7 = v3;
	v8 = FVariant(0.1f);

	printf("%d %u %s %f\n", v5.Get<int>(), v6.Get<unsigned>(), v7.Get<String>().CString(), v8.Get<float>());

	v1 = 0.1f;
	v2 = String("2333");
	v3.Set(110u);
	v4.Set(100);

	printf("%f %s %u %d\n", v1.Get<float>(), v2.Get<String>().CString(), v3.Get<unsigned>(), v4.Get<int>());

	FVariant test;
	test = Vector2(100, 100);

	printf("(%f, %f)\n", test.Get<Vector2>().x_, test.Get<Vector2>().y_);

	auto temp = [](const Vector2& value)
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

IMPLEMENT_MODULE_USD("Test");
