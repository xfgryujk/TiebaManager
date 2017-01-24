#include "stdafx.h"
#include <TiebaClientHelper.h>

#include <NetworkHelper.h>
#include <Md5.h>


TIEBA_API_API CString TiebaClientHTTPPost(const CString& URL, const CString& data, CString* cookie)
{
	// 客户端POST要带数字签名，参数按字典序排列，去掉&，加上"tiebaclient!!!"，转成UTF-8，取MD5
	CString signData = data;
	signData.Replace(_T("&"), _T(""));
	signData += _T("tiebaclient!!!");
	CString newData = data + _T("&sign=") + GetMD5_UTF8(signData);
	return HTTPPost(URL, newData, cookie);
}
