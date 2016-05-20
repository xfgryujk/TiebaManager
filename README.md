TiebaManager
=========

[百度贴吧](http://tieba.baidu.com/)吧务管理工具  

* 自动扫描帖子并处理违规帖
* 违规规则支持关键词、正则表达式、图片（包括头像和签名）、等级
* 支持插件开发
* 通过GPLv2协议开源


依赖
=========

* [OpenCV](http://opencv.org/) 3.0.0
* [TinyXML2](http://www.grinninglizard.com/tinyxml2/index.html)


链接
=========

* [作者的个人贴吧](http://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD)
* [最新版下载地址](http://sinacloud.net/xfgryujk/TiebaManager/%e8%b4%b4%e5%90%a7%e7%ae%a1%e7%90%86%e5%99%a8.zip)


搭建开发环境
=========

1. 本项目使用VS2013编写，未在其他开发环境中测试
2. 搭建OpenCV 3.0.0，并把环境变量`OpenCV`设置为你的OpenCV目录
3. 下载本项目源码
4. 新建Debug目录，把`%OpenCV%\build\x86\vc12\bin\opencv_world300d.dll`和`Library\bin\Debug\tinyxml2.dll`放进去
5. 新建Release目录，把`%OpenCV%\build\x86\vc12\bin\opencv_world300.dll`和`Library\bin\Release\tinyxml2.dll`放进去
