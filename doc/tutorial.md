
# dmtimer的使用方法

## 执行timer
在dmtimer中，核心的类为**CDMTimerModule**，定义在`include/dmtimermodule.h`中
它是一个单例类，我们通过`CDMTimerModule::Instance()`来获取它的实例指针
调用`CDMTimerModule::Instance()->Run()`时，它就会执行所有过期的timer
一般的用法就是在主循环中的某个地方调用`Run()`方法，来执行所有过期的timer

### 注册/注销timer
在dmtimer中要注册timer需要通过继承**CDMTimerNode**类，并且实现它的`OnTimer`方法
`OnTimer`即超时的时候我们希望进行的回调函数
`OnTimer`的定义如下：
```
    virtual void OnTimer(uint64_t qwIDEvent);
    virtual void OnTimer(uint64_t qwIDEvent, const dm::any& oAny);
```

第一个参数`qwIDEvent`即我们注册timer时用的ID. 这个ID在一个对象内必须是唯一的
第二个参数`const dm::any& oAny`代表该timer额外绑定数据


注册timer的接口：
```
    bool CDMTimerNode::SetTimer(uint64_t qwIDEvent, uint64_t qwElapse);

    bool CDMTimerNode::SetTimer(uint64_t qwIDEvent, uint64_t qwElapse, const dm::any& oAny, bool bExact);

    bool CDMTimerNode::SetTimer(uint64_t qwIDEvent, uint64_t qwElapse, uint64_t qwFirst);
```

```
    bool CDMTimerNode::SetTimer(uint64_t qwIDEvent, uint64_t qwElapse, uint64_t qwFirst, const dm::any& oAny, bool bExact);
```

*    第一个参数`uint64_t qwIDEvent`代表timer的ID，当这个timer超时，回调用户实现的OnTimer时，参数就会和这个`qwIDEvent`一致. 这个ID在一个对象内必须是唯一的
*    第二个参数`uint64_t qwElapse`代表超时时间，即从现在开始过多少时间这个timer超时，单位为**毫秒**
*    第三个参数`uint64_t qwFirst`代表第一次触发时间, 正常使用应该是 qwFirst 与 qwElapse 相等
*    第四个参数`const dm::any& oAny`代表该timer所需要绑定额外数据
*    第五个参数`bool bExact`代表该timer调用机制, false(默认) 漏调(用于忙的状态 防止恶性循环) true 补调(某些逻辑需求 依赖于timer调用次数)

*    我们在一个对象中注册的所有timer的ID是不会重复的
*    如果注册的id已经存在，则会**覆盖那个timer对应的定时器**

注销timer的接口：
```
    void CDMTimerNode::KillTimer(uint64_t qwIDEvent);
```

*    第一个参数`qwIDEvent`代表要取消的timer的id
*    如果timer的ID不存在会忽略
```
    void CDMTimerNode::KillTimer();
```

*    删除当前对象所有timer, 使用的是延迟删除标记方式

### 更多接口
获取timer已启动的时间：
```
    uint64_t CDMTimerNode::GetTimerElapse(uint64_t qwIDEvent);
```

*    第一个参数`uint64_t qwIDEvent`代表timer的ID
*    返回值是一个uint64_t 类型，代表这个timer从启动到现在已经过去了这么长时间，单位为毫秒

获取timer剩余多少时间会被调用
``` 
    uint64_t CDMTimerNode::GetTimerRemain(uint64_t qwIDEvent);
```

*    第一个参数`uint64_t qwIDEvent`代表timer的ID
*    返回值是一个uint64_t类型，代表这个timer剩余多少时间会被调用，单位为毫秒


