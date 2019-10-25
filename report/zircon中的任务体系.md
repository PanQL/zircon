### zircon中的任务体系  

----

在zircon中，将任务管理的层次分为三个等级，分别为thread、process、job。这三个等级之间的关系如下：

* 一个process：管理多个thread
* 一个job管理：多个process、若干个child job、application
* 各个job之间呈现以root job为根的树状结构，即除root job外每一个job都有且仅有一个parent job。