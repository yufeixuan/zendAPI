<img src="http://www.zendapi.com/statics/images/components/about/images//zendapi-logo.svg" width="300" style="margin:10px auto"/>

### 介绍

该项目是对 Zend Engine 的 C 接口使用 C++11 进行而面向对象的封装，从而屏蔽了底层 Zend Engine API 的接口复杂性，加快开发 PHP 扩展的效率。

项目官网地址：www.zendapi.org

### 设计目标

让 PHP 扩展开发更有趣<br/><br/>
你是否在开发扩展的时候遇到想使用PHP的一个接口，但是就是苦于不知道在哪里定义，就算找到了在哪里定义看到的定义是否让你抓狂，各种指针，各种宏定义
让你的生活变得不美好，哈哈，是时候尝试 zendAPI 啦，我们让你无忧无虑的调用底层的 Zend Engine API, 安装语义对一维的 C API 进行二次组织，精心的
设计每一个接口，并且提供关于接口的详细文档与使用范例，还等什么，赶快到碗里来吧。

### 特性

1. 完全面向对象，对 Zend Engine API 进行二次定义
2. 使用现代的 C++11 语法进行开发，便于维护
3. 最大化屏蔽 PHP 版本对扩展开发的影响，zendAPI 将对 Zend Engine API 不同版本带来的差异屏蔽掉
4. 高覆盖的单元测试，保证代码质量
5. 在封装的时候，尽最大能力保证性能

### 文档相关

用户手册: www.zendapi.org/manual</br>
API手册: www.zendapi.org/api

### 说明

当前 zendAPI 项目处于非常前期阶段，很多接口都不稳定，甚至会经常有大的变化，请不要用于实际项目，现阶段只适合学习用途。
项目参考 
1. PHP-CPP 进行实现，大家要是对PHP-CPP感兴趣，请点击 https://github.com/CopernicaMarketingSoftware/PHP-CPP
2. Qt project 在底层数据结构的接口上借鉴Qt core相关设计，如果您对 Qt 感兴趣，请点击：http://doc.qt.io/

**声明：**</br>
zendAPI 项目第一版中有些部分跟 PHP-CPP 将会高度一样，文件头也会加上 PHP-CPP 信息，一旦完成后 项目将会独立发展，当然适当的时机
我也会将代码反馈给 PHP-CPP 项目。

### 简单入门
#### 项目环境要求
暂时只支持 linux 服务器环境，PHP 版本7.0以上，标准 C++11 编译器, cmake 3.5.0以上版本

#### 安装

TODO

### 贡献者列表

ZZU_SOFTBOY <zzu_softboy@163.com></br>
冰冻三尺，非一日之寒。欢迎大家加入，一起玩。

### License

Apache License Version 2.0 see http://www.apache.org/licenses/LICENSE-2.0.html

### 特别感谢
Unicorn Team，感谢团队的自我驱动和包容的文化，让我开发 zendAPI 项目。

<img style = "width:100px;height:100px;float:left;display:block" src="https://raw.githubusercontent.com/qcoreteam/zendapi/master/assets/images/unicornteam.png"/>
