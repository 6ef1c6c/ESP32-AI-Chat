# ESP 32 AI Chat

![License](https://img.shields.io/badge/License-MIT-yellow.svg)
![Architecture](https://img.shields.io/badge/Architecture-RISC--V-blue.svg?logo=riscv&logoColor=white)

本项目是为ESP32 c6开发板所制作的ai工具，未来可能支持更多的开发板

## 实现
本工具无需依赖任何外设，正常构建情况下不会熔写任何qfuse，通过连接ap热点访问<br>
192.168.4.1进行控制<br>
默认热点密码为ESP123456<br>

此工具为apikey调用来实现聊天功能，进入后台界面后，需进入设置连接网络，配置apikey才可正常使用，默认情况下为ap+sta双模运行

此工具的其他功能 RGB灯开关（红，蓝，绿，渐变） 日志（一共4个等级，off，error，info，debug） 开发板状态（包括cpu占用，内存占用，Flash占用）

## 环境
框架 ESP-IDF 6.1<br>

## License
本项目的源代码完全公开，欢迎任何人审查，学习，修改和分发<br>

本项目仅供个人学习与交流使用，作者不对因使用本代码造成的任何直接或间接损失负责，请自行承担设备调试与操作风险<br>

另外我们希望你的衍生项目也能保持开源<br>
