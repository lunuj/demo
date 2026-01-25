# demo 示例

这是一个 C 的示例项目，包含 `Makefile` 与 `build.sh`，用于展示一个小型跨平台构建与运行流程。

## 主要内容
- 项目类型：C 语言，基于 Make 构建
- 支持平台：Windows（MinGW / MSYS2）、Linux、macOS
- src 目录下主程序编译
- test 目录下测试程序编译

## 计划
- 增加编译动态库、静态库功能

## 目录结构（摘要）
```text
.  
├── include           头文件
├── lib               第三方或本地库
├── output            构建输出与脚本
├── src               源代码
├── test              测试代码
└── tool              工具
```

## 构建工具
- C 编译器（gcc / clang / MinGW）
- `make`（Windows 上可使用 `mingw32-make` 或 MSYS2 提供的 `make`）

## 构建

Linux / macOS:
```bash
make all
```

Windows (MinGW/MSYS2):
```powershell
# 在 MinGW/MSYS2 shell 中
mingw32-make all
```

项目也提供构建脚本 `build.sh`：
```bash
# 查看帮助
./build.sh help
# 使用脚本构建并运行
./build.sh run
# 使用脚本重新构建
./build.sh all
# 编译test文件夹下%.c
./build.sh test %
```

## 运行
- 构建后可执行文件位于项目根或 `output/` 目录（依据 Makefile 配置），示例：
  - Linux/macOS: `./output/demo` 或 `./build.sh run`
  - Windows: `output\demo.exe`
