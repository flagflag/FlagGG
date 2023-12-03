#include "Math/InterpCurveEdSetup.h"
#include "Math/Distributions/DistributionFloat.h"
#include "Math/Distributions/DistributionVector.h"

namespace FlagGG
{

InterpCurveEdSetup::InterpCurveEdSetup()
{
	tabs_.Push(CurveEdTab("Default", 0.0f, 1.0f, -1.0f, 1.0f));
}

CurveEdInterface* InterpCurveEdSetup::GetCurveEdInterfacePointer(const CurveEdEntry& entry)
{
	DistributionFloat* FloatDist = Cast<DistributionFloat>(entry.curveObject_);
	if(FloatDist)
	{
		return FloatDist;
	}

	DistributionVector* VectorDist = Cast<DistributionVector>(entry.curveObject_);
	if(VectorDist)
	{
		return VectorDist;
	}

	// TODO
	//InterpTrack* InterpTrack = Cast<InterpTrack>(Entry.curveObject_);
	//if(InterpTrack)
	//{
	//	return InterpTrack;
	//}

	return NULL;
}

bool InterpCurveEdSetup::AddCurveToCurrentTab(
	Object* inCurve, const String& curveName, const Color& curveColor,
	CurveEdEntry** outCurveEntry, bool inColorCurve, bool inFloatingPointColor,
	bool inClamp, float inClampLow, float inClampHigh)
{
	CurveEdTab& tab = tabs_[activeTab_];

	// See if curve is already on tab. If so, do nothing.
	for (Int32 i = 0; i < tab.curves_.Size(); i++)
	{
		CurveEdEntry& Curve = tab.curves_[i];
		if (Curve.curveObject_ == inCurve)
		{
			if (outCurveEntry)
			{
				*outCurveEntry = &Curve;
			}
			return false;
		}
	}

	// Curve not there, so make new entry and record details.
	CurveEdEntry* newCurve = &(tab.curves_.EmplaceBack());

	newCurve->curveObject_ = inCurve;
	newCurve->curveName_ = curveName;
	newCurve->curveColor_ = curveColor;
	newCurve->colorCurve_ = inColorCurve;
	newCurve->floatingPointColorCurve_ = inFloatingPointColor;
	newCurve->clamp_ = inClamp;
	newCurve->clampLow_ = inClampLow;
	newCurve->clampHigh_ = inClampHigh;

	if (outCurveEntry)
	{
		*outCurveEntry = newCurve;
	}
	return true;
}

void InterpCurveEdSetup::RemoveCurve(Object* inCurve)
{
	for (Int32 i = 0; i < tabs_.Size(); i++)
	{
		CurveEdTab& tab = tabs_[i];
		for (Int32 j = tab.curves_.Size() - 1; j >= 0; j--)
		{
			if (tab.curves_[j].curveObject_ == inCurve)
			{
				tab.curves_.Erase(j);
			}
		}
	}
}
	
void InterpCurveEdSetup::ReplaceCurve(Object* removeCurve, Object* addCurve)
{
	ASSERT(removeCurve);
	ASSERT(addCurve);

	for (Int32 i = 0; i < tabs_.Size(); i++)
	{
		CurveEdTab& tab = tabs_[i];
		for (Int32 j = 0; j < tab.curves_.Size(); j++)
		{
			if (tab.curves_[j].curveObject_ == removeCurve)
			{
				tab.curves_[j].curveObject_ = addCurve;
			}
		}
	}
}


void InterpCurveEdSetup::CreateNewTab(const String& inTabName)
{
	CurveEdTab tab;

	tab.tabName_ = inTabName;
	tab.viewStartInput_ = 0.0f;
	tab.viewEndInput_ = 1.0f;
	tab.viewStartOutput_ = -1.0;
	tab.viewEndOutput_ = 1.0;

	tabs_.Push(tab);
}


void InterpCurveEdSetup::RemoveTab(const String& inTabName)
{
	for (Int32 i = 0; i < tabs_.Size(); i++)
	{
		CurveEdTab& tab = tabs_[i];
		if (tab.tabName_ == inTabName)
		{
			tabs_.Erase(i);
			break;
		}
	}
}

bool InterpCurveEdSetup::ShowingCurve(Object* inCurve)
{
	for (Int32 i = 0; i < tabs_.Size(); i++)
	{
		CurveEdTab& tab = tabs_[i];

		for (Int32 j = 0; j < tab.curves_.Size(); j++)
		{
			if (tab.curves_[j].curveObject_ == inCurve)
				return true;
		}
	}

	return false;
}

void InterpCurveEdSetup::ChangeCurveColor(Object* inCurve, const Color& curveColor)
{
	for (Int32 i = 0; i < tabs_.Size(); i++)
	{
		CurveEdTab& tab = tabs_[i];
		for (Int32 j = 0; j < tab.curves_.Size(); j++)
		{
			if (tab.curves_[j].curveObject_ == inCurve)
			{
				tab.curves_[j].curveColor_ = curveColor;
			}
		}
	}
}

void InterpCurveEdSetup::ChangeCurveName(Object* inCurve, const String& newCurveName)
{
	for (Int32 i = 0; i < tabs_.Size(); i++)
	{
		CurveEdTab& tab = tabs_[i];
		for (Int32 j = 0; j < tab.curves_.Size(); j++)
		{
			if (tab.curves_[j].curveObject_ == inCurve)
			{
				tab.curves_[j].curveName_ = newCurveName;
			}
		}
	}
}


void InterpCurveEdSetup::ResetTabs()
{
	tabs_.Clear();

	CurveEdTab tab;
	
	tab.tabName_		 =  "Default";
	tab.viewStartInput_	 =  0.0f;
	tab.viewEndInput_	 =  1.0f;
	tab.viewStartOutput_ = -1.0;
	tab.viewEndOutput_	 =  1.0;

	tabs_.Push(tab);
}

}
