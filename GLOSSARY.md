# 六边形架构词汇表

只收用户已通过测验/练习证明理解的术语（与 DDD 课同一规则）。DDD 侧术语见
`~/learning/learning-ddd/GLOSSARY.md`，此处不重复。

---

### 端口 Port
一段「有目的的对话」的边界接口；凡遵守其协议者皆可插入（名字取自操作系统端口）。按**意图**
命名（`OrderRepository`、`PlaceOrder`），不按技术命名。典型应用只有 2–4 个端口。
**歧义澄清**：名字里有 `Sql` / `Kafka` / `InMemory` **不等于**「按技术命名（坏）」——
技术词出现在**适配器**类名上是合格的（`SqlOrderRepository`、`InMemoryOrderStore`）；
出现在**端口**名上才坏（`ForMysqlOrders`、`KafkaProducerPort`）。把两个品牌拆成两个「端口」
（`PostgresGateway` + `RedisGateway`）同样是把适配器抬成了端口。
*（第 2 课试一试证明意图命名；第 3 课回顾复验「技术词贴适配器/组装根合格」，见 learning-records/0004。）*

### 适配器 Adapter
在端口协议与某种具体技术之间做双向翻译的组件。一个端口可插多个适配器；**测试替身也是
适配器**，与生产适配器同级。
*（第 1 课两道陷阱题——测试用例、InMemoryRepo——均判对，2026-07-19。）*

### 驱动侧 / 被驱动侧 Driving / Driven（primary / secondary）
唯一判据：**谁先开口**。发起对话、把应用从静止态唤醒的在**驱动侧**（UI、CLI、测试、cron）；
被应用调用的在**被驱动侧**——要么给答案（仓储），要么**只是通知**（邮件、事件）。
**歧义澄清**：方向 ≠ 谁发起。数据往外走（通知/写库）不代表它在驱动;数据往里走（Kafka
消费者）不代表它被驱动。「通知型」端口是被驱动端口的教科书范例（易被 Notifier 词义带偏——
见 learning-records/0002）。
*（第 1 课抽象判据 5/5；「通知型端口」第 2 课回顾 MetricsSink 题复验通过，2026-07-20。）*

### 配置式依赖 Configurable Dependency
用哪个适配器，在应用**外部**（组装根）决定并插入；六边形内部只见端口。判据：换生产连接为
测试替身（再换回来）**不必改应用源码**（Cockburn Budapest 2023 Benefits）。
*（第 3 课全对，2026-07-20。）*

### 组装根 Composition Root（configurator）
靠近应用入口、**唯一**创建具体适配器并完成接线的位置（`main` / 测试 setup）。C++ 口诀：
**组装根拥有，应用借用**（应用持 `ForX&`，不 `new` 适配器）。DI 容器若使用，也只许出现在此处。
*（第 3 课全对；与 learning-records/0003 软点一并结案——技术类名可出现在适配器与组装根。）*

### 测试线束 / 测试替身 Test harness / Test double
驱动侧用**测试线束**（FIT、Cucumber、gtest 调驱动端口）替主参与者；被驱动侧用**测试替身**
（in-memory、spy）替次参与者。左右自然配对，对调即废。
*（第 4 课试一试 Q1 判对；FIT 易被误当成「真驱动」——见 learning-records/0005，待第 5 课复验。）*

### S2（孤立六边形）
开发顺序矩阵中：驱动侧=测试、六边形=真实、被驱动侧=替身。**此阶段达成六边形架构主目标**
——业务可在无 UI、无真库下经驱动端口回归。S5（全真）是生产部署，不是「目标完成」的标记。
*（第 4 课 lesson4_isolation.cc 形态已练；试一试有人误选 S5，见 learning-records/0005。）*
