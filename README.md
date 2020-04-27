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
```

## 4 skynet 嵌入既有框架

# cpp_hproto 的开发环境布置
