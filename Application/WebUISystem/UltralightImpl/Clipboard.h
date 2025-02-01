#pragma once

#include <Ultralight/platform/Clipboard.h>

namespace ultralight
{

class UltralightClipboard : public Clipboard
{
public:
	///
	/// Clear the clipboard.
	///
	virtual void Clear() override;

	///
	/// Read plain text from the clipboard
	/// 
	/// This is called when the library wants to read text from the OS clipboard.
	///
	virtual String ReadPlainText() override;

	///
	/// Write plain text to the clipboard.
	/// 
	/// This is called when the library wants to write text to the OS clipboard.
	///
	virtual void WritePlainText(const String& text) override;
};

}
