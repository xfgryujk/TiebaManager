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

	// Ã²ËÆÈ«²¿ÓÃdllexportÒ²¿ÉÒÔ£¿
	__declspec(dllexport) void Read(const tinyxml2::XMLElement& root);
	__declspec(dllexport) void Write(tinyxml2::XMLElement& root) const;
};

// ¶ÁOption
#define DECLEAR_READ(T) void COption<T >::Read(const tinyxml2::XMLElement& root)
// Ð´Option
#define DECLEAR_WRITE(T) void COption<T >::Write(tinyxml2::XMLElement& root) const

// ¶Ávector
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

// ¶Áset
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

// ¶Ámap
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

// Ð´ÈÝÆ÷
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

// Ð´vector
#define DEFINE_WRITE_VECTOR(T) DEFINE_WRITE_CONTAINER(T, vector)

// Ð´set
#define DEFINE_WRITE_SET(T) DEFINE_WRITE_CONTAINER(T, set)

// Ð´map
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
