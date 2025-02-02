#pragma once

#include "Export.h"
#include "Core/Context.h"
#include "Core/EventDefine.h"
#include "Core/Subsystem.h"
#include "Math/Vector2.h"
#include "Container/RefCounted.h"

namespace FlagGG
{

class Context;
class Window;

class FlagGG_API RawMsgParam final
{
public:
	RawMsgParam();

	void Clear();

	void AddRawParam(UInt32 paramValue);

	UInt32 GetParamCount() const;

	UInt64 GetRawParam(UInt32 index);

private:
	UInt64 rawParams_[10];
	
	UInt32 rawParamCount_;
};

class FlagGG_API KeyState
{
public:
	KeyState(Window* sender, bool ctrl, bool alt, bool shift, RawMsgParam* rawMsgParam);

	Window* GetSender() const { return sender_; }

	bool GetCtrlPressed() const { return ctrl_; }

	bool GetAltPressed() const { return alt_; }

	bool GetShiftPressed() const { return shift_; }

	RawMsgParam* GetRawMsgParam() const { return rawMsgParam_; }

private:
	Window* sender_;

	bool ctrl_;

	bool alt_;

	bool shift_;

	RawMsgParam* rawMsgParam_;
};

class FlagGG_API Input : public Subsystem<Input>
{
public:
	Input();

	~Input() override = default;

	IntVector2 GetMousePos() const;

	void ShowMouse();

	void HideMouse();

	bool IsMouseShow() const;

	void OnKeyDown(KeyState* keyState, UInt32 keyCode);

	void OnKeyUp(KeyState* keyState, UInt32 keyCode);

	void OnChar(KeyState* keyState, UInt32 keyCode);

	void OnMouseDown(KeyState* keyState, MouseKey mouseKey, const IntVector2& mousePos);

	void OnMouseUp(KeyState* keyState, MouseKey mouseKey, const IntVector2& mousePos);

	void OnMouseMove(KeyState* keyState, const IntVector2& mousePos, const Vector2& delta);

	void OnMouseWheel(KeyState* keyState, Int32 delta);

	void OnSetFocus(Window* window);

	void OnKillFocus(Window* window);

private:
	bool isMouseShow_;
};

}
