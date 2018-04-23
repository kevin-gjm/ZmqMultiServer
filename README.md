# ZmqMultiServer
### 多线线程ZMQ服务
内部使用ZMQ inproc实现线程服务。接收用户信息后将信息扔到线程池中，处理结果返回指定客户端(通过前导的身份帧来确定相应的客户端)

### 备注
- 若需要多进程、负载均衡的处理参考：

  Woker参考地址 https://github.com/kevin-gjm/ZmqWorker.git
  
  Proxy参考地址 https://github.com/kevin-gjm/ZmqMultiServer.git

- 使用C++11
- 依赖库:glog zmq
- 用户自定义数据为最后一帧,且仅有一帧,若多帧用户自己修改即可

  
