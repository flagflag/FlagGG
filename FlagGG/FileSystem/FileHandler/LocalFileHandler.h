//
// �����ļ�������ڲ�ʵ��ΪC��׼��
//

#pragma once

#include "FileSystem/FileHandler.h"

namespace FlagGG
{

/// �����ļ����
/// Using C standard IO functions.
class FlagGG_API LocalFileHandler : public IFileHandler
{
public:
	LocalFileHandler();

	~LocalFileHandler() override;

	/// ���ļ����ɹ�����true
	bool Open(const String& fileName, FileMode mode) override;

	/// �����ļ��Ƿ񱻴�
	bool IsOpen() const override;

	/// ���ļ���ȡ�ֽ���������ʵ�ʶ�ȡ���ֽڴ�С
	bool Read(void* buffer, USize size) override;

	/// д�ֽ������ļ�������ʵ��д���ֽڴ�С
	bool Write(const void* buffer, USize size) override;

	/// �����ļ�ָ��λ��
	bool Seek(SSize offset, SeekFrom origin) override;

	/// �ر��ļ�
	void Close() override;

	/// ˢ���ļ�������
	void Flush() override;

	/// �����ļ�ָ��λ��
	USize Position() const override;

	/// �����ļ���С
	USize Size() const override;

	/// �����ļ���ԭ�����
	void* NativePtr() const override { return handle_; }

private:
	void* handle_;

	mutable USize size_;

	mutable bool sizeDirty_;
};

}
