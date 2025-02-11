//
// 保存shader的一些原始信息，用于调试定位行号
//

#pragma once

#include "Core/BaseTypes.h"
#include "Container/Str.h"
#include "Container/Vector.h"
#include "Container/RefCounted.h"
#include "Container/ArrayPtr.h"

namespace FlagGG
{

struct PreProcessFileInfo
{
	String filePath_;    // 文件路径
	String shaderSource_;
};

struct PreProcessLineInfo
{
	UInt32 originFileIndex_;  // 原始文件名下标
	UInt32 originLineNumber_; // 原始shader行号
};

struct PreProcessShaderInfo : public RefCounted
{
	SharedArrayPtr<char> buffer_;
	USize bufferSize_{};
	Vector<PreProcessFileInfo> fileInfos_;
	PODVector<PreProcessLineInfo> lineInfos_;
};

}
