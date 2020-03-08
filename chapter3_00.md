# 第三章 Hyperledger Fabric架构详解

在[区块链技术选型](./chapter2_05%20selection_of_technology.md)章节，我们讨论了超级账本与比特币、以太坊的差别，超级账本中的Fabric项目在效率和隐私保护都远远超过了公链的设计。

其实要做到这点也不难，联盟链的做法是限制成员加入——必须是经过认证通过的节点才能加入。由于节点需要认证，因此Hyperledger Fabric在设计上弱化了恶意攻击，在提高效率的同时还可以有效保护隐私，使得联盟链在企业实施变成可能。

Hyperledger Fabric的最大的优势是支持共识机制、权限管理、密钥、账本机制等模块的可插拔。Fabric早期被很多人诟病，效率低下，有中心化嫌疑，现在的说法是不符合国产密码算法要求。提出这些问题的人一方面是没有产品研发能力，另一方面是没有用发展的眼光看待新生事物。

现在业内很多知名区块链公司包括趣链、纸贵科技等都是基于Fabric进行的产品研发，研发的方向主要是集中在共识机制、密码算法等方面。Fabric目前已经提供了1.4稳定版本，这个版本在很多方面都有了大的提升。

这个章节我们深入学习Hyperledger Fabric的设计架构。与一般只是理论学习的方式不一样，这个章节也会详细讲解Fabric网络的搭建和启动过程，让大家在Fabric实际网络中体会联盟链设计的精妙。

在正式阅读本章之前，默认你已经掌握了区块链工程师的必备工具：
* 熟悉linux操作系统常用命令
* 熟悉Docker常用命令
* 熟悉Git基本知识

本章主要内容：
* [Hperledger 项目简介](./chapter3_01%20hyperledger_project_overview.md)
* [Hyperledger Fabric 网络体验——系统环境配置](./chapter3_02_part1%20hyperledger_fabric_network_prerequisites.md)
* [Hyperledger Fabric 网络体验——Fabric网络环境](./chapter3_02_part2%20hyperledger_fabric_network_installing.md)
* [Hyperledger Fabric 网络体验——网络启动过程概览](./chapter3_02_part3%20hyperledger_fabric_network_bring_up.md)
* [Hyperledger Fabric系统架构](./chapter3_03%20hyperledger_fabric_architecture.md)
* [Hyperledger Fabric核心组件](./chapter3_04%20hyperledger_fabric_core_components.md)
* [Hyperledger Fabric 典型交易流程](./chapter3_05%20hyperledger_fabric_workflow_of_transaction.md)
* [Hyperledger Fabric 架构详解知识总结](./chapter3_06%20summary.md)
  
注意：如果对以上必备工具还不熟悉，可以通过第三方资料先进行学习，或者参照[《大话区块链》](https://item.jd.com/12719282.html)一书中第二篇**区块链工程师基本能力**4.6章节**区块链工程师必备工具**。



