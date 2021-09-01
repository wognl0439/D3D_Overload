#pragma once
#include "Engine.h"

SSS_BEGIN

class SSS_DLL CRef
{
protected:
	CRef();
	virtual ~CRef() = 0;

public:
	int m_iRefCount;
	string m_strTag;
	bool m_bEnable;
	bool m_bDestroy;

public:
	const string& GetTag() const;
	bool IsEnable() const;
	bool IsDestroy() const;

public:
	void SetTag(const string& strTag);
	void Enable(bool bEnable);
	void Destroy(bool bDestroy);
	void DestroyImmediate();
	int AddRef();
	int Release();


};

SSS_END;
