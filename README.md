# skynet_sketch

# 克隆(git clone)

因为包含了子组件(Submodules)skynet，需下面命令克隆。否则，skynet 不会被克隆下来。
``` bash
git clone --recursive https://github.com/uhziel/skynet_sketch.git 
```

# 演示

## 1 编译和启动 skynet，演示其功能
``` bash
$ cd skynet
$ make macosx
$ ./skynet ../login/config # server
$ ./3rd/lua/lua ../login/client.lua # client
```

## 2 skynet 调用外部 api 和解析 json
``` bash
$ ./skynet ../request_external_api/config
...
[:00000009] request GET: http://www.weather.com.cn/data/sk/101200101.html
[:00000008] result: {"time":"17:00","isRadar":"1","njd":"暂无实况","sm":"1.7","WS":"小于3级","temp":"19.5","WD":"北风","AP":"1001.7hPa","cityid":"101200101","WSE":"<3","city":"武汉","SD":"95%","Radar":"JC_RADAR_AZ9270_JB"}
...
```

## 3 skynet 动态读取 xml 配置
``` bash
# xmlconfig.lua: xml 读取服务
# reader.lua: 订阅 books.xml
#    xmlconfig = skynet.queryservice("xmlconfig")
#    local ret = skynet.call(xmlconfig, "lua", "subscribe", "books", "../xmlconfig/books.xml")
# reader2.lua: 订阅 people.xml
$ ./skynet ../xmlconfig/config 
$ touch ../xmlconfig/books.xml # 更新文件时间，reader.lua 可以动态收到自己订阅 xml的内容
[:0000000a] notify_xml_content config_name: books
```

## 4 skynet 嵌入既有框架
``` bash
# 启动了监听端口 26667，提供查询信用分服务，可以用既有框架方式方式访问它
$ ./skynet ../netpack/config_x51
$ nc localhost 8000 # 登录到 skynet 调试终端，可以输入help查询可用指令
# 加入的自定义命令这里也被看到，本人加了loadtest指令，用来压力测试查询信用分接口
# loadtest	loadtest count duration interval: default loadtest 10 100(s) 2(s)
loadtest 2 1 1
start load_test count:2 duration:1 interval:1
# 注意：
# 查询信用分依赖外部 api 服务，配置位置在 ../netpack/config_x51 的 ccx_host 中。要想本地演示，得先配置个假的 api 服务，完成下面功能。
# 打开 http://localhost:8082/cgi-bin/auth/query_common_credit.json，返回
# {"ret":0, "data":{"mtime":123456, "score":552, "level":1}}
```

# cpp_hproto - 消息协议代码生成工具
cpp_hproto 是个帮助把头文件中定义的消息转化成解析成lua对象的代码。

它利用 LLVM clang 的 AST(抽象语法树)来刮削出头文件中我们关心的信息(先找到包含CEvent的结构体/类，再找成员变量中使用的结构体/类，这么递归下去)，再利用这个信息生成解析代码。

## 1 开发环境布置

``` bash
# 机器：mac os x 10.14.6
$ git clone https://github.com/llvm/llvm-project.git # 可能会非常慢，请耐心等待
$ cd llvm-project
$ git checkout origin/release/9.x # 切换到 9.x 分支
$ ln -s ../skynet_sketch/cpp_hproto clang-tools-extra/cpp_hproto # 符号链接 cpp_hproto 到 llvm-project
$ vim clang-tools-extra/CMakeLists.txt
+add_subdirectory(cpp_hproto)
$ mkdir build
$ cd build
$ cmake -G Ninja -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" ../llvm/
$ ninja # 编译时间会比较长。本机请装好 build 工具 ninja，类似 make，但比它快。
$ ./bin/cpp_hproto --version

```

## 2 演示生成

``` bash
$ ./bin/cpp_hproto ../../skynet_sketch/netpack/example_events_header.h > ../../skynet_sketch/netpack/generated_hproto.h
$ less ../../skynet_sketch/netpack/generated_hproto.h
```
