# 六边形架构 Resources

## Knowledge

- [Article: "Hexagonal Architecture" — Alistair Cockburn（2005 原文）](https://alistair.cockburn.us/hexagonal-architecture)
  模式发明者的原始定义：intent、port/adapter 定义、primary/secondary actor 不对称性、典型端口数量（2–4 个）。
  Use for: 一切权威定义的出处，引用优先查它，不凭记忆。
- [Book: _Hexagonal Architecture Explained_ — Cockburn & Garrido de Paz（2024）](https://www.amazon.com/Hexagonal-Architecture-Explained-Alistair-Cockburn/dp/173751978X)
  发明者本人写的权威成书（202 页），含示例代码与 FAQ、测试策略（test-to-test-double 矩阵）。
  Use for: 端口粒度、配置式依赖、测试策略的深入课；推荐用户购买作为主教材。
- [Slides: Cockburn Budapest 2023 演讲 PDF](https://alistaircockburn.com/Hexagonal%20Budapest%2023-05-18.pdf)
  发明者近年对模式的自述与澄清（含常见误读）。Use for: 免费的、比 2005 原文更新的一手材料。
- [Article: "Ready for changes with Hexagonal Architecture" — Netflix Tech Blog](https://netflixtechblog.com/ready-for-changes-with-hexagonal-architecture-b315ec967749)
  Netflix 真实案例：靠六边形结构在数天内换掉数据源。Use for: 迁移弧的动机与实战证据。
- [Guide: Hexagonal architecture pattern — AWS Prescriptive Guidance](https://docs.aws.amazon.com/prescriptive-guidance/latest/cloud-design-patterns/hexagonal-architecture.html)
  大厂工程化视角的模式描述。Use for: 评审课的第二视角、术语交叉验证。
- [Book: _Working Effectively with Legacy Code_ — Michael Feathers](https://www.oreilly.com/library/view/working-effectively-with/0131177052/)
  接缝（seam）、特征测试（characterization test）的出处。Use for: 迁移弧（改造现有项目）的方法论地基。
- [Article: "Composition Root" — Mark Seemann (2011)](https://blog.ploeh.dk/2011/07/28/CompositionRoot/)
  组装根定义：靠近应用入口、唯一组装对象图的位置；容器只许出现在此处。
  Use for: 第 3 课与 Cockburn「configurator」交叉印证；评审「DI 容器漏进领域」时的判据。
- 本地：`~/learning/learning-ddd/reference/DDD_Reference_2015-03.pdf`（Evans DDD Reference, CC BY 4.0）
  Use for: 六边形内部（领域/应用）术语与 DDD 课衔接时的定义出处。

## Wisdom (Communities)

- （待补：用户尚未表达社区偏好。候选：r/softwarearchitecture、DDD-CQRS-ES Discord——
  Cockburn 本人偶有出没的场合以其网站/演讲为准。迁移弧开课后再议。）

## Gaps

- 缺一个高质量的「C++ 六边形架构」参考实现——主流示例全是 Java/Kotlin/C#。
  处理方式与 DDD 课相同：模式翻译成 C++ 惯用法，代码自己写、真编译（`code/`）。
