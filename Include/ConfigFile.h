/*
Copyright (C) 2011-2017  xfgryujk
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
#include <vector>
#include <set>
#include <map>


class HELPER_API COptionBase
{
public:
	const CStringA m_name;

	COptionBase(const CStringA& name) : m_name(name) { }
	virtual ~COptionBase() = default;

	virtual void UseDefault() = 0;
	virtual void Read(const tinyxml2::XMLElement& root) = 0;
	virtual void Write(tinyxml2::XMLElement& root) const = 0;
};

template <class T>
class COptionTBase : public COptionBase
{
public:
	const T m_default;
	T m_value;
	typedef bool(*IsValidFunc)(const T& value);
	const IsValidFunc IsValid;

	COptionTBase(const CStringA& name, IsValidFunc _isValid)
		: COptionBase(name), m_default(), IsValid(_isValid) { }
	COptionTBase(const CStringA& name, const T& _default, IsValidFunc _isValid)
		: COptionBase(name), m_default(_default), IsValid(_isValid) { }
	virtual ~COptionTBase() = default;

	virtual bool operator == (const COptionTBase<T>&) const = delete;
	virtual operator const T& () const{ return m_value; }
	virtual operator T& (){ return m_value; }
	virtual const T& operator * () const{ return m_value; }
	virtual T& operator * (){ return m_value; }
	virtual const T* operator -> () const{ return &m_value; }
	virtual T* operator -> (){ return &m_value; }
	virtual void UseDefault(){ m_value = m_default; }
};

template <class T>
class COption : public COptionTBase<T>
{
public:
	COption(const CStringA& name, IsValidFunc _isValid = [](const T&){ return true; })
		: COptionTBase<T>(name, _isValid) { }
	COption(const CStringA& name, const T& _default, IsValidFunc _isValid = [](const T&){ return true; })
		: COptionTBase<T>(name, _default, _isValid) { }
	virtual ~COption() = default;

	// 貌似全部用dllexport也可以？
	__declspec(dllexport) virtual void Read(const tinyxml2::XMLElement& root);
	__declspec(dllexport) virtual void Write(tinyxml2::XMLElement& root) const;
};


// 用来声明特化读写的宏

// 读Option
#define DECLEAR_READ(T) void COption<T >::Read(const tinyxml2::XMLElement& root)
// 写Option
#define DECLEAR_WRITE(T) void COption<T >::Write(tinyxml2::XMLElement& root) const


// 对容器读写的特化

template <class T>
class COption<std::vector<T> > : public COptionTBase<std::vector<T> >
{
public:
	COption(const CStringA& name, IsValidFunc _isValid = [](const std::vector<T>&){ return true; })
		: COptionTBase<std::vector<T> >(name, _isValid) { }
	COption(const CStringA& name, const std::vector<T>& _default, IsValidFunc _isValid = [](const std::vector<T>&){ return true; })
		: COptionTBase<std::vector<T> >(name, _default, _isValid) { }
	virtual ~COption() = default;

	virtual void Read(const tinyxml2::XMLElement& root)
	{
		const tinyxml2::XMLElement* optionNode = root.FirstChildElement(m_name); 
		if (optionNode == NULL)
		{
			UseDefault(); 
			return; 
		}
		
		m_value.clear(); 
		COption<T> value("value"); 
		for (const tinyxml2::XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
		{
			value.Read(*item); 
			m_value.push_back(value); 
		}
		if (!IsValid(m_value))
			UseDefault(); 
	}

	virtual void Write(tinyxml2::XMLElement& root) const
	{
		tinyxml2::XMLDocument* doc = root.GetDocument(); 
		tinyxml2::XMLElement* optionNode = doc->NewElement(m_name); 
		root.LinkEndChild(optionNode); 
			
		COption<T> value("value"); 
		for (const T& i : m_value)
		{
			tinyxml2::XMLElement* item = doc->NewElement("item"); 
			optionNode->LinkEndChild(item); 
			*value = i; 
			value.Write(*item); 
		}
	}
};

template <class T>
class COption<std::set<T> > : public COptionTBase<std::set<T> >
{
public:
	COption(const CStringA& name, IsValidFunc _isValid = [](const std::set<T>&){ return true; })
		: COptionTBase<std::set<T> >(name, _isValid) { }
	COption(const CStringA& name, const std::set<T>& _default, IsValidFunc _isValid = [](const std::set<T>&){ return true; })
		: COptionTBase<std::set<T> >(name, _default, _isValid) { }
	virtual ~COption() = default;

	virtual void Read(const tinyxml2::XMLElement& root)
	{
		const tinyxml2::XMLElement* optionNode = root.FirstChildElement(m_name); 
		if (optionNode == NULL)
		{
			UseDefault(); 
			return; 
		}
		
		m_value.clear(); 
		COption<T> value("value"); 
		for (const tinyxml2::XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
		{
			value.Read(*item); 
			m_value.insert(value); 
		}
		if (!IsValid(m_value))
			UseDefault(); 
	}

	virtual void Write(tinyxml2::XMLElement& root) const
	{
		tinyxml2::XMLDocument* doc = root.GetDocument(); 
		tinyxml2::XMLElement* optionNode = doc->NewElement(m_name); 
		root.LinkEndChild(optionNode); 
			
		COption<T> value("value"); 
		for (const T& i : m_value)
		{
			tinyxml2::XMLElement* item = doc->NewElement("item"); 
			optionNode->LinkEndChild(item); 
			*value = i; 
			value.Write(*item); 
		}
	}
};

template <class T1, class T2>
class COption<std::map<T1, T2> > : public COptionTBase<std::map<T1, T2> >
{
public:
	COption(const CStringA& name, IsValidFunc _isValid = [](const std::map<T1, T2>&){ return true; })
		: COptionTBase<std::map<T1, T2> >(name, _isValid) { }
	COption(const CStringA& name, const std::map<T1, T2>& _default, IsValidFunc _isValid = [](const std::map<T1, T2>&){ return true; })
		: COptionTBase<std::map<T1, T2> >(name, _default, _isValid) { }
	virtual ~COption() = default;

	virtual void Read(const tinyxml2::XMLElement& root)
	{
		const tinyxml2::XMLElement* optionNode = root.FirstChildElement(m_name); 
		if (optionNode == NULL)
		{
			UseDefault(); 
			return; 
		}
		
		m_value.clear(); 
		COption<T1> key("key"); 
		COption<T2> value("value"); 
		for (const tinyxml2::XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
		{
			key.Read(*item); 
			value.Read(*item); 
			m_value[key] = value; 
		}
		if (!IsValid(m_value))
			UseDefault(); 
	}

	virtual void Write(tinyxml2::XMLElement& root) const
	{
		tinyxml2::XMLDocument* doc = root.GetDocument(); 
		tinyxml2::XMLElement* optionNode = doc->NewElement(m_name); 
		root.LinkEndChild(optionNode); 
		
		COption<T1> key("key"); 
		COption<T2> value("value"); 
		for (const auto& i : m_value)
		{
			tinyxml2::XMLElement* item = doc->NewElement("item"); 
			optionNode->LinkEndChild(item); 
			*key = i.first; 
			key.Write(*item); 
			*value = i.second; 
			value.Write(*item); 
		}
	}
};


class HELPER_API CConfigBase
{
protected:
	std::vector<COptionBase*> m_options;

public:
	const CStringA m_name;

	CConfigBase(CStringA name) : m_name(name) { }
	virtual ~CConfigBase() { }

	virtual BOOL Load(const tinyxml2::XMLDocument& doc);
	virtual BOOL Load(const CString& path);
	virtual BOOL LoadFromString(LPCSTR str, size_t length = (size_t)-1);
	virtual BOOL Save(const CString& path) const;
	virtual void UseDefault();
	virtual void OnChange() { }
	virtual void PostChange() { }
};


// IsValidFunc

template<class T, T min, T max>
bool InRange(const T& value)
{
	return min <= value && value <= max;
}

template<class T, T min>
bool GreaterThan(const T& value)
{
	return value >= min;
}

template<class T, T max>
bool LessThan(const T& value)
{
	return value <= max;
}
