#include "stdafx.h"
#include <ConfigFile.h>
using namespace tinyxml2;
#include <StringHelper.h>


// COption实现 ///////////////////////////////////////////////////////////////////////////
#pragma region COption

// 读基本类型

DECLEAR_READ(int)
{
	const XMLElement* optionNode = root.FirstChildElement(thiz.m_name);
	if (optionNode == NULL)
	{
	UseDefault:
		thiz.UseDefault();
		return root;
	}
	const XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	thiz.m_value = atoi(value);
	if (!thiz.IsValid(thiz.m_value))
		thiz.UseDefault();
	return root;
}

DECLEAR_READ(float)
{
	const XMLElement* optionNode = root.FirstChildElement(thiz.m_name);
	if (optionNode == NULL)
	{
	UseDefault:
		thiz.UseDefault();
		return root;
	}
	const XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	thiz.m_value = (float)atof(value);
	if (!thiz.IsValid(thiz.m_value))
		thiz.UseDefault();
	return root;
}

DECLEAR_READ(double)
{
	const XMLElement* optionNode = root.FirstChildElement(thiz.m_name);
	if (optionNode == NULL)
	{
	UseDefault:
		thiz.UseDefault();
		return root;
	}
	const XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	thiz.m_value = atof(value);
	if (!thiz.IsValid(thiz.m_value))
		thiz.UseDefault();
	return root;
}

DECLEAR_READ(__int64)
{
	const XMLElement* optionNode = root.FirstChildElement(thiz.m_name);
	if (optionNode == NULL)
	{
	UseDefault:
		thiz.UseDefault();
		return root;
	}
	const XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	thiz.m_value = _atoi64(value);
	if (!thiz.IsValid(thiz.m_value))
		thiz.UseDefault();
	return root;
}

DECLEAR_READ(CString)
{
	const XMLElement* optionNode = root.FirstChildElement(thiz.m_name);
	if (optionNode == NULL)
	{
	UseDefault:
		thiz.UseDefault();
		return root;
	}
	const XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	thiz.m_value = GBK2W(value);
	if (!thiz.IsValid(thiz.m_value))
		thiz.UseDefault();
	return root;
}

// 读vector

DEFINE_READ_VECTOR(CString)

// 读set

DEFINE_READ_SET(__int64)
DEFINE_READ_SET(CString)

// 读map

DEFINE_READ_MAP(__int64, int)
DEFINE_READ_MAP(CString, int)
DEFINE_READ_MAP(__int64, CString)


// 写基本类型

DECLEAR_WRITE(int)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(thiz.m_name);
	root.LinkEndChild(optionNode);

	char buffer[15];
	_itoa_s(thiz.m_value, buffer, _countof(buffer), 10);
	optionNode->LinkEndChild(doc->NewText(buffer));
	return root;
}

DECLEAR_WRITE(float)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(thiz.m_name);
	root.LinkEndChild(optionNode);

	char buffer[25];
	sprintf_s(buffer, "%f", thiz.m_value);
	optionNode->LinkEndChild(doc->NewText(buffer));
	return root;
}

DECLEAR_WRITE(double)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(thiz.m_name);
	root.LinkEndChild(optionNode);

	char buffer[50];
	sprintf_s(buffer, "%f", thiz.m_value);
	optionNode->LinkEndChild(doc->NewText(buffer));
	return root;
}

DECLEAR_WRITE(__int64)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(thiz.m_name);
	root.LinkEndChild(optionNode);

	char buffer[25];
	_i64toa_s(thiz.m_value, buffer, _countof(buffer), 10);
	optionNode->LinkEndChild(doc->NewText(buffer));
	return root;
}

DECLEAR_WRITE(CString)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(thiz.m_name);
	root.LinkEndChild(optionNode);

	optionNode->LinkEndChild(doc->NewText(W2GBK(thiz.m_value)));
	return root;
}

// 写vector

DEFINE_WRITE_VECTOR(CString)

// 写set

DEFINE_WRITE_SET(__int64)
DEFINE_WRITE_SET(CString)

// 写map

DEFINE_WRITE_MAP(__int64, int)
DEFINE_WRITE_MAP(CString, int)
DEFINE_WRITE_MAP(__int64, CString)

#pragma endregion

// 配置读写实现 ///////////////////////////////////////////////////////////////////////////

HELPER_API BOOL CConfigBase::Load(const CString& path)
{
	FILE* f = NULL;
	if (_tfopen_s(&f, path, _T("rb")) != 0 || f == NULL)
	{
		UseDefault();
		return FALSE;
	}

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(f) != XML_NO_ERROR)
	{
		fclose(f);
		UseDefault();
		return FALSE;
	}
	fclose(f);

	XMLElement* root = doc.FirstChildElement(m_name);
	if (root == NULL)
	{
		UseDefault();
		return FALSE;
	}

	OnChange();
	for (COptionBase* i : m_options)
		i->Read(*root);
	PostChange();
	return TRUE;
}

HELPER_API BOOL CConfigBase::Save(const CString& path) const
{
	FILE* f = NULL;
	if (_tfopen_s(&f, path, _T("wb")) != 0 || f == NULL)
		return FALSE;

	tinyxml2::XMLDocument doc;
	doc.LinkEndChild(doc.NewDeclaration("xml version=\"1.0\" encoding=\"GBK\""));
	tinyxml2::XMLElement* root = doc.NewElement(m_name);
	doc.LinkEndChild(root);

	for (const COptionBase* i : m_options)
		i->Write(*root);

	BOOL res = doc.SaveFile(f) == XML_NO_ERROR;
	fclose(f);
	return res;
}

HELPER_API void CConfigBase::UseDefault()
{
	OnChange();
	for (COptionBase* i : m_options)
		i->UseDefault();
	PostChange();
}
