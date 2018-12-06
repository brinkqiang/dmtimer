
# dmtimer的使用方法

## 执行timer
在dmtimer中，核心的类为**CDMTimerModule**，定义在`include/dmtimermodule.h`中。
它是一个单例类，我们通过`CDMTimerModule::Instance()`来获取它的实例指针。
调用`CDMTimerModule::Instance()->Run()`时，它就会执行所有过期的timer。
一般的用法就是在主循环中的某个地方调用`Run()`方法，来执行所有过期的timer。

### 注册/注销timer
在dmtimer中要注册timer需要通过继承**CDMTimerNode**类，并且实现它的`OnTimer`方法。
`OnTimer`即超时的时候我们希望进行的回调函数。  
`OnTimer`的定义如下：
```
    virtual void OnTimer(uint64_t qwIDEvent);
```

唯一的参数`qwIDEvent`即我们注册timer时用的eventid。这个eventid在一个`CDMTimerNode`对象内必须是唯一的，
也就是说我们在一个`CDMTimerNode`中注册的所有timer的id是不会重复的，
如果注册的id已经存在，则会**覆盖那个timer对应的定时器**。

注册timer的接口：
```
    bool CDMTimerNode::SetTimer(uint64_t qwIDEvent, uint64_t qwElapse);
```

*    第一个参数`qwIDEvent`代表timer的id，当这个timer超时，回调用户实现的OnTimer时，参数就会和这个`qwIDEvent`一致。
*    第二个参数`qwElapse`代表超时时间，即从现在开始过多少时间这个timer超时，单位为**毫秒**;
*    它的返回值是一个bool类型，表示这个timer是否设置成功。

注销timer的接口：
```
    bool CDMTimerNode::KillTimer(uint64_t qwIDEvent);
```

*    第一个参数`qwIDEvent`代表要取消的timer的id。
*    返回值是一个bool类型，表示这个timer是否成功注销。

### 更多接口
获取timer已过去的时间：
```
    uint64_t CDMTimerNode::GetTimerElapse(uint64_t qwIDEvent);
```

*    第一个参数`qwIDEvent`代表timer的id。
*    返回值是一个unsigned long long类型，代表这个timer从启动到现在已经过去了这么长时间，单位为毫秒。

获取timer还有多少时间超时：
``` 
    uint64_t CDMTimerNode::GetTiemrRemain(uint64_t qwIDEvent);
```

*    第一个参数`qwIDEvent`代表timer的id。
*    返回值是一个uint64_t类型，代表这个timer剩余多少时间超时。


