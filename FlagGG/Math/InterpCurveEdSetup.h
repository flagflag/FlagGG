// Copyright Epic Games, Inc. All Rights Reserved.


#pragma once

#include "Core/BaseTypes.h"
#include "Core/Object.h"
#include "Container/Ptr.h"
#include "Container/Str.h"
#include "Math/Color.h"
#include "Math/CurveEdInterface.h"

namespace FlagGG
{

// Information about a particule curve being viewed.
// Property could be an FInterpCurve, a DistributionFloat or a DistributionVector
struct CurveEdEntry
{
	CurveEdEntry()
		: hideCurve_(0)
		, colorCurve_(0)
		, floatingPointColorCurve_(0)
		, clamp_(0)
		, clampLow_(0)
		, clampHigh_(0)
	{
	}

	SharedPtr<Object> curveObject_;

	Color curveColor_;

	String curveName_;

	Int32 hideCurve_;

	Int32 colorCurve_;

	Int32 floatingPointColorCurve_;

	Int32 clamp_;

	float clampLow_;

	float clampHigh_;
};

struct CurveEdTab
{
	CurveEdTab()
		: viewStartInput_(0)
		, viewEndInput_(0)
		, viewStartOutput_(0)
		, viewEndOutput_(0)
	{
	}

	CurveEdTab(const String& InTabName, float InViewStartInput, float InViewEndInput, float InViewStartOutput, float InViewEndOutput)
		: tabName_(InTabName)
		, viewStartInput_(InViewStartInput)
		, viewEndInput_(InViewEndInput)
		, viewStartOutput_(InViewStartOutput)
		, viewEndOutput_(InViewEndOutput)
	{
	}

	String tabName_;

	Vector<CurveEdEntry> curves_;

	// Remember the view setting for each tab.
	float viewStartInput_;

	float viewEndInput_;

	float viewStartOutput_;

	float viewEndOutput_;
};

class InterpCurveEdSetup : public Object
{
	OBJECT_OVERRIDE(InterpCurveEdSetup, Object);
public:
	InterpCurveEdSetup();

	//~ Begin InterpCurveEdSetup Interface
	FlagGG_API static CurveEdInterface* GetCurveEdInterfacePointer(const CurveEdEntry& Entry);

	/** 
	 * Add a new curve property to the current tab.
	 *
	 * @param	OutCurveEntry	The curves which are for this graph node
	 *
	 * @return	true, if new curves were added to the graph, otherwise they were already present
	 */
	FlagGG_API bool AddCurveToCurrentTab(Object* inCurve, const String& curveName, const Color& curveColor,
		CurveEdEntry** outCurveEntry = NULL, bool inColorCurve = false, bool inFloatingPointColor = false,
		bool inClamp = false, float inclampLow_ = 0.f, float inclampHigh_ = 0.f);

	/** Remove a particuclar curve from all tabs. */
	FlagGG_API void RemoveCurve(Object* inCurve);

	/** Replace a particuclar curve */
	FlagGG_API void ReplaceCurve(Object* removeCurve, Object* addCurve);

	/** Create a new tab in the CurveEdSetup. */
	FlagGG_API void CreateNewTab(const String& inTabName);

	/** Remove the tab of the given name from the CurveEdSetup. */
	FlagGG_API void RemoveTab(const String& inTabName);
	
	/** Look through CurveEdSetup and see if any properties of selected object is being shown. */
	FlagGG_API bool ShowingCurve(Object* inCurve);

	/** Change the color of the given curve */
	FlagGG_API void ChangeCurveColor(Object* inCurve, const Color& curveColor);
	
	/** Change the displayed name for a curve in the curve editor. */
	FlagGG_API void ChangeCurveName(Object* inCurve, const String& newCurveName);

	/** Remove all tabs and re-add the 'default' one */
	void ResetTabs();


	Vector<CurveEdTab> tabs_;

	Int32 activeTab_;
};

}
