/*
Copyright (C) 2015  xfgryujk
http://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "stdafx.h"
#include <ConfigFile.h>
using namespace tinyxml2;
#include <StringHelper.h>


// COption实现 ///////////////////////////////////////////////////////////////////////////
#pragma region COption

// 读基本类型

HELPER_API DECLEAR_READ(int)
{
	const XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
	UseDefault:
		UseDefault();
		return;
	}
	const XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	m_value = atoi(value);
	if (!IsValid(m_value))
		UseDefault();
}

HELPER_API DECLEAR_READ(float)
{
	const XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
	UseDefault:
		UseDefault();
		return;
	}
	const XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	m_value = (float)atof(value);
	if (!IsValid(m_value))
		UseDefault();
}

HELPER_API DECLEAR_READ(double)
{
	const XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
	UseDefault:
		UseDefault();
		return;
	}
	const XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	m_value = atof(value);
	if (!IsValid(m_value))
		UseDefault();
}

HELPER_API DECLEAR_READ(__int64)
{
	const XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
	UseDefault:
		UseDefault();
		return;
	}
	const XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	m_value = _atoi64(value);
	if (!IsValid(m_value))
		UseDefault();
}

HELPER_API DECLEAR_READ(CString)
{
	const XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
	UseDefault:
		UseDefault();
		return;
	}
	const XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	m_value = GBK2W(value);
	if (!IsValid(m_value))
		UseDefault();
}

// 读vector

HELPER_API DEFINE_READ_VECTOR(CString)

// 读set

HELPER_API DEFINE_READ_SET(__int64)
HELPER_API DEFINE_READ_SET(CString)

// 读map

HELPER_API DEFINE_READ_MAP(__int64, int)
HELPER_API DEFINE_READ_MAP(CString, int)
HELPER_API DEFINE_READ_MAP(__int64, CString)


// 写基本类型

HELPER_API DECLEAR_WRITE(int)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	char buffer[15];
	_itoa_s(m_value, buffer, _countof(buffer), 10);
	optionNode->LinkEndChild(doc->NewText(buffer));
}

HELPER_API DECLEAR_WRITE(float)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	char buffer[25];
	sprintf_s(buffer, "%f", m_value);
	optionNode->LinkEndChild(doc->NewText(buffer));
}

HELPER_API DECLEAR_WRITE(double)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	char buffer[50];
	sprintf_s(buffer, "%f", m_value);
	optionNode->LinkEndChild(doc->NewText(buffer));
}

HELPER_API DECLEAR_WRITE(__int64)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	char buffer[25];
	_i64toa_s(m_value, buffer, _countof(buffer), 10);
	optionNode->LinkEndChild(doc->NewText(buffer));
}

HELPER_API DECLEAR_WRITE(CString)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	optionNode->LinkEndChild(doc->NewText(W2GBK(m_value)));
}

// 写vector

HELPER_API DEFINE_WRITE_VECTOR(CString)

// 写set

HELPER_API DEFINE_WRITE_SET(__int64)
HELPER_API DEFINE_WRITE_SET(CString)

// 写map

HELPER_API DEFINE_WRITE_MAP(__int64, int)
HELPER_API DEFINE_WRITE_MAP(CString, int)
HELPER_API DEFINE_WRITE_MAP(__int64, CString)

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
