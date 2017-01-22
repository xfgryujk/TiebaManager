TiebaManager
=========

[百度贴吧](http://tieba.baidu.com/)吧务管理工具  

* 自动扫描帖子并处理违规帖
* 违规规则支持关键词、正则表达式、图片（包括头像和签名）、等级
* 支持插件开发
* 通过GPLv2协议开源

![截图](https://github.com/xfgryujk/TiebaManager/blob/master/.wiki/image/snapshot.png)


依赖
---------

* [OpenCV](http://opencv.org/) 3.0.0
* [TinyXML2](http://www.grinninglizard.com/tinyxml2/index.html)
* [libcurl](https://curl.haxx.se/libcurl/) 7.40.0
* [rapidjson](http://rapidjson.org/)


链接
---------

* [作者的个人贴吧](http://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD)
* [最新版下载地址](https://github.com/xfgryujk/TiebaManager/wiki/%E5%B8%B8%E8%A7%81%E9%97%AE%E9%A2%98#%E5%9C%A8%E5%93%AA%E9%87%8C%E5%8F%AF%E4%BB%A5%E4%B8%8B%E8%BD%BD%E6%9C%80%E6%96%B0%E7%89%88)
* [使用方法](https://github.com/xfgryujk/TiebaManager/wiki/%E5%A6%82%E4%BD%95%E4%BD%BF%E7%94%A8)


搭建开发环境
---------

1. 本项目使用VS2013编写，未在其他开发环境中测试
2. 安装OpenCV 3.0.0
3. 下载本项目源码
4. 把`CommonProperty.props.example`复制一份并重命名为`CommonProperty.props`
5. 用文本编辑器（或在VS2013里）打开CommonProperty.props，把里面的OpenCV目录设置为你安装的OpenCV目录
6. 运行`SetupEnvironment.bat`
7. 把`%OpenCV%\build\x86\vc12\bin\opencv_world300d.dll`放到`Debug`目录，把`%OpenCV%\build\x86\vc12\bin\opencv_world300.dll`放到`Release`目录和`贴吧管理器`目录
