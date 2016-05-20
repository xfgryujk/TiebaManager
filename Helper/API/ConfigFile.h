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

#pragma once
#include "HelperCommon.h"
#include <tinyxml2.h>


class COptionBase
{
public:
	const CStringA m_name;

	COptionBase(const CStringA& name) : m_name(name) { }
	virtual ~COptionBase() { }

	virtual void UseDefault() = 0;
	virtual void Read(const tinyxml2::XMLElement& root) = 0;
	virtual void Write(tinyxml2::XMLElement& root) const = 0;
};

template <class T>
class COption : public COptionBase
{
public:
	const T m_default;
	T m_value;
	typedef BOOL(*IsValidFunc)(const T& value);
	const IsValidFunc IsValid;

	COption(const CStringA& name, IsValidFunc _isValid = [](const T&){ return TRUE; })
		: COptionBase(name), m_default(), IsValid(_isValid) { }
	COption(const CStringA& name, const T& _default, IsValidFunc _isValid = [](const T&){ return TRUE; })
		: COptionBase(name), m_default(_default), IsValid(_isValid) { }

	bool operator == (const COption<T>&) const = delete;
	operator const T& () const{ return m_value; }
	operator T& (){ return m_value; }
	const T& operator * () const{ return m_value; }
	T& operator * (){ return m_value; }
	const T* operator -> () const{ return &m_value; }
	T* operator -> (){ return &m_value; }
	void UseDefault(){ m_value = m_default; }

	// 貌似全部用dllexport也可以？
	__declspec(dllexport) void Read(const tinyxml2::XMLElement& root);
	__declspec(dllexport) void Write(tinyxml2::XMLElement& root) const;
};

// 读Option
#define DECLEAR_READ(T) void COption<T >::Read(const tinyxml2::XMLElement& root)
// 写Option
#define DECLEAR_WRITE(T) void COption<T >::Write(tinyxml2::XMLElement& root) const

// 读vector
#define DEFINE_READ_VECTOR(T) \
DECLEAR_READ(vector<T>)\
{\
	const tinyxml2::XMLElement* optionNode = root.FirstChildElement(m_name); \
	if (optionNode == NULL)\
	{\
		UseDefault(); \
		return; \
	}\
	\
	m_value.clear(); \
	COption<T> value("value"); \
	for (const tinyxml2::XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))\
	{\
		value.Read(*item); \
		m_value.push_back(value); \
	}\
	if (!IsValid(m_value))\
		UseDefault(); \
}

// 读set
#define DEFINE_READ_SET(T) \
DECLEAR_READ(set<T>)\
{\
	const tinyxml2::XMLElement* optionNode = root.FirstChildElement(m_name); \
	if (optionNode == NULL)\
	{\
		UseDefault(); \
		return; \
	}\
	\
	m_value.clear(); \
	COption<T> value("value"); \
	for (const tinyxml2::XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))\
	{\
		value.Read(*item); \
		m_value.insert(value); \
	}\
	if (!IsValid(m_value))\
		UseDefault(); \
}

// 读map
#define COMMA ,

#define DEFINE_READ_MAP(T1, T2) \
DECLEAR_READ(map<T1 COMMA T2>)\
{\
	const tinyxml2::XMLElement* optionNode = root.FirstChildElement(m_name); \
	if (optionNode == NULL)\
	{\
		UseDefault(); \
		return; \
	}\
	\
	m_value.clear(); \
	COption<T1> key("key"); \
	COption<T2> value("value"); \
	for (const tinyxml2::XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))\
	{\
		key.Read(*item); \
		value.Read(*item); \
		m_value[key] = value; \
	}\
	if (!IsValid(m_value))\
		UseDefault(); \
}

// 写容器
#define DEFINE_WRITE_CONTAINER(T, CONTAINER) \
DECLEAR_WRITE(CONTAINER<T>)\
{\
	tinyxml2::XMLDocument* doc = root.GetDocument(); \
	tinyxml2::XMLElement* optionNode = doc->NewElement(m_name); \
	root.LinkEndChild(optionNode); \
	\
	COption<T> value("value"); \
	for (const T& i : m_value)\
	{\
		tinyxml2::XMLElement* item = doc->NewElement("item"); \
		optionNode->LinkEndChild(item); \
		*value = i; \
		value.Write(*item); \
	}\
}

// 写vector
#define DEFINE_WRITE_VECTOR(T) DEFINE_WRITE_CONTAINER(T, vector)

// 写set
#define DEFINE_WRITE_SET(T) DEFINE_WRITE_CONTAINER(T, set)

// 写map
#define DEFINE_WRITE_MAP(T1, T2) \
DECLEAR_WRITE(map<T1 COMMA T2>)\
{\
	tinyxml2::XMLDocument* doc = root.GetDocument(); \
	tinyxml2::XMLElement* optionNode = doc->NewElement(m_name); \
	root.LinkEndChild(optionNode); \
	\
	COption<T1> key("key"); \
	COption<T2> value("value"); \
	for (const auto& i : m_value)\
	{\
		tinyxml2::XMLElement* item = doc->NewElement("item"); \
		optionNode->LinkEndChild(item); \
		*key = i.first; \
		key.Write(*item); \
		*value = i.second; \
		value.Write(*item); \
	}\
}


class CConfigBase
{
protected:
	vector<COptionBase*> m_options;

public:
	const CStringA m_name;

	CConfigBase(CStringA name) : m_name(name) { }
	virtual ~CConfigBase() { }

	HELPER_API virtual BOOL Load(const CString& path);
	HELPER_API virtual BOOL Save(const CString& path) const;
	HELPER_API virtual void UseDefault();
	virtual void OnChange() { }
	virtual void PostChange() { }
};
