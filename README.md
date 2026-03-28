# foo_lyric_source

Foobar2000 歌词插件，支持网易云音乐、QQ音乐歌词源和本地歌词文件。

## 功能特性

- 多歌词源支持：本地、网易云音乐、QQ音乐
- 自动歌词源切换：找不到自动尝试下一个源
- 同步歌词显示：实时高亮当前行
- 本地歌词缓存：下载的歌词自动保存到本地
- 歌词源优先级：本地 > 网易云 > QQ音乐

## 编译说明

### 前置要求

- Visual Studio 2019/2022
- CMake 3.15+
- Windows 10/11

### 编译步骤

```bash
# 克隆或下载 foobar2000 SDK（已包含）
# 构建项目
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

### 输出

编译成功后，`build/Release/foo_lyric_source.dll` 将生成在构建目录中。

## 安装

1. 将 `foo_lyric_source.dll` 复制到 foobar2000 的 `components` 目录
2. 重启 foobar2000
3. 通过 `View -> Layout` 添加歌词面板
4. 或通过菜单 `View -> Show Lyrics Panel` 打开

## 使用方法

1. 播放歌曲时，插件会自动搜索歌词
2. 支持本地 `.lrc` 文件：文件名格式为 `Artist - Song Name.lrc`
3. 如果本地找不到，会自动尝试在线源
4. 支持鼠标滚轮滚动歌词

## 本地歌词

歌词会自动保存在以下位置之一：
- foobar2000 目录下的 `lyrics` 文件夹
- 系统临时目录下的 `foobar_lyrics` 文件夹

## 技术栈

- C++17
- GDI+ 用于渲染
- WinHTTP 用于网络请求

## 许可证

MIT License
