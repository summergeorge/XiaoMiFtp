<<<<<<< HEAD
﻿XiaoMiFtp
=========

自动连接局域网中开启小米远程文件管理的手机
=======
﻿miuiftp
=======
>>>>>>> 98246ecd439b00baaf3e66179038f80285d4183f

软件主体不是我写的，第一次用GitHub，这个是学习测试用的。我只是在作者原版上面做了少许修改，感谢原作者。

通过此软件，电脑自动发现统一局域网中开启2121端口的ip地址（这是小米手机远程文件管理默认的端口），然后调用电脑的文件管理器打开扫描到的地址，省去了繁琐的ftp地址输入，好用且实用。

原版：扫描局域网中地址第四段从100开始的后20个ip，如192.168.1.100 - 192.168.1.120 。（国内路由器大多是从100开始分配ip，原版可以满足大多数用户需求）

修改版：因为本人就是爱折腾，且自己的路由器ip是从1开始分配的，原版就扫描不到了。于是就在原版的基础上进行了修改：改变原有的扫描方式，软件运行开始先获得主机ip，然后扫描主机ip第四段加减10的ip范围，如主机ip为192.168.1.103，那么扫描的ip地址就是192.168.1.93 - 192.168.1.113，这个ip范围差不多能够包括全部用户了。如果主机ip为192.168.1.5，这是软件会自动从192.168.1.1开始扫描到192.168.1.20，不会出现5-10等 -5 的逻辑错误。
