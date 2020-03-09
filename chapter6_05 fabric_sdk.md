## Fabric SDK

Fabric SDK 封装了一系列与区块链网络打交道的基本方法，包括发送交易、监听网络事件、查询区块和交易信息等。通过SDK 可以很容易地使用以下四个步骤完成一次完整的程序调用和确认。

* 从CA 获取合法的身份证书；
* 构造合法的交易提案提交给Endorser 节点进行背书；
* 收集到足够多Endorser 支持后，构造合法的交易请求，发给Orderer 节点进行排序； 
* 监听事件，确保交易已经写入账本。

Fabric 目前支持Node.js 、Java的SDK（Python和Go的SDK目前还没有正式发布） 。本教程以Node.js为例。 

官方文档：

https://hyperledger.github.io/fabric-sdk-node/release-1.4/index.html
