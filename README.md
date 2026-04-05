## 介绍

模板工程

目录结构
```text
.
├── example     // 示例目录，输出的示例
│   └── xxx
├── include     // 头文件目录，包含外部库和内部库
│   └── xxx
├── lib         // 库文件目录，包含外部库和内部库
│   └── xxx
├── output      // 输出文件目录，包括最终程序、测试程序以及临时的中间文件
│   └── xxx
├── res         // 资源文件目录，包含使用的图片、音乐等资源
│   └── xxx
├── src         // 源文件目录，包含各个模块使用的源文件
│   └── xxx
├── test        // 测试文件目录，包含各个模块使用的测试文件
│   └── xxx
├── build.sh    // 编译辅助脚本
├── Makefile    // 编译使用的Makefile
└── README.md   // 说明文档
```

## 使用
使用依赖：
- 需要 Make 环境编译。
- 需要 mingw 环境运行，目前仅支持 windows。

### 编译
```bash
# 编译 main 主程序
./build.sh all
# 编译 arg 测试程序
./build.sh test <arg>
```

### 运行
```bash
# 运行 main 主程序
./build.sh run
# 运行 arg 测试程序
./build.sh run <arg>
```