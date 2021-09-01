#include "Ref.h"

SSS_USING

CRef::CRef():
	m_iRefCount(1),
	m_bEnable(true),
	m_bDestroy(false)
{
}


CRef::~CRef()
{
}

const string & CRef::GetTag() const
{
	return m_strTag;
}

bool CRef::IsEnable() const
{
	return m_bEnable;
}

bool CRef::IsDestroy() const
{
	return m_bDestroy;
}

void CRef::SetTag(const string & strTag)
{
	m_strTag = strTag;
}

void CRef::Enable(bool bEnable)
{
	m_bEnable = bEnable;
}

void CRef::Destroy(bool bDestroy)
{
	m_bDestroy = bDestroy;
}

void CRef::DestroyImmediate()
{
	delete this;
}

int CRef::AddRef()
{
	++m_iRefCount;
	return m_iRefCount;
}

int CRef::Release()
{
	--m_iRefCount;

	if (m_iRefCount == 0)
	{
		delete this;
		return 0;
	}

	return m_iRefCount;
}
