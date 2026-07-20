# Working Notes

## 工作区
- 路径：`~/learning/learing-hexagonal-architecture`（目录名有拼写笔误 learing，用户未要求改，勿动）
- 已 `git init`（分支 main）。入口页：`index.html`——新增课程后记得更新进度条与课程行。
- 本课程是 **DDD 课续篇**（`~/learning/learning-ddd`），用户 2026-07-19 确认。
  组件库从 DDD 课复制（course.css / quiz.js / categorize.js），API 见各文件头注释。

## 用户画像（继承自 DDD 课 + 本课确认）
- C++ 工程师，DDD 战略+战术已收官（连续满分，端口/适配器直觉已在仓储、事件发布、防腐层上验证）
- 本课动机（2026-07-19 多选）：**架构师能力补课 + 改造现有项目**。注意：没选「新项目落地」——
  与 DDD 课 mission 不同，这门课的落点是**遗留代码迁移**，绿地示例只是教学脚手架。
- **迁移弧开课前必问**：现有项目是什么领域/规模/痛点。没有真实项目上下文的迁移课是纸上谈兵。

## 教学偏好（沿用 DDD 课，已验证有效）
- 中文授课，代码与注释一律英文；现代 C++（C++17/20），不要 Java 味
- 节奏快：少铺垫、多判断题。每课一个概念。
- 课首「你在地图上的位置」+ 回顾检查 2 题（间隔重复 + 证据回收）
- 课末反馈只问「答错哪几道 + 选了哪个」，不让用户数总分；点明本课有几个计分区
- C++ 代码必须真编译（`code/`，clang -std=c++20 -Wall -Wextra），贴真实编译器报错
- 好手法：「一条绿色断言证明一件坏事」；让类型系统把概念变成看得见的东西
- 词汇表只收测验证明掌握的术语

## 路线草图（随学习记录滚动修正）
1. ✅ 六边形模型：从分层到六边形（0001）— 回顾 2/2、练一练 9/10、试一试 5/5（2026-07-19）。
   核心「谁先开口=驱动侧」立住。唯一软点:练一练第 8 题 `CustomerNotifier` 误归驱动端口
   （正解被驱动端口）——是「Notifier/通知」词义语感干扰,非判据坏（试一试 Q3「通知=被驱动」
   已判对,同卷 SmtpNotifier=被驱动适配器也对,自相矛盾即证据）。详见 learning-records/0002。
   词汇表 +3:端口、适配器、驱动/被驱动侧（驱动/被驱动的「通知型端口」格待第 2 课复验）。
2. ✅ 端口的粒度与命名（0002）— 回顾 2/2、练一练 5/8、试一试 5/5（2026-07-20）。
   主线（purposeful conversation / 2–4 / 意图命名）立住。软点：练一练第 3/6/7 题
   把「名字含技术词」与「按技术命名（坏）」绑死——`SqlOrderRepository`、`InMemoryOrderStore`
   误判为坏命名，而 `PostgresGateway+RedisGateway` 当端口时反判成适配器。
   实为「技术词贴端口才坏、贴适配器合格」的另一半未立住。详见 learning-records/0003。
   课首回顾（第 3 课）确定放：技术类名出现在哪一侧才合格。
3. 配置式依赖与组装根：手工 DI、main() 组装、C++ 里的所有权与生命周期
   **课首回顾检查确定放**:「`PostgresOrderStore` 这个类名——端口还是适配器？」验 0003。
4. 测试策略：驱动侧进、被驱动侧换替身；test-to-test-double 矩阵（书第 6 章方向）
5. 对比评审课：Hexagonal vs Clean vs Onion vs 传统分层（mission「评审」落点）
6. 迁移弧（多课）：接缝识别 → 特征测试 → 抽第一个端口 → 绞杀式推进
   （mission「改造现有项目」落点；**开课前先问真实项目**）

## DDD 课可复用的锚点
- 用户已熟的示例词汇：`Order` / `OrderRepository` / `SqlOrderRepository` / `EventPublisher` / ACL
- 已内化的判据：「策略/编排=应用层，机制/技术=基础设施层」「依赖只朝里指」
- 六边形课的新增量在**驱动侧（左边）**：UI/测试/cron 与 DB 地位对称——分层图里没有这半边
- 间隔重复池：DDD 课 0006/0007 的软点已结案，但「两种事务边界」「送进 Kafka=适配器」
  仍可作为本课回顾检查素材（跨课程间隔重复，强化存储强度）

## 组件库（assets/）
- 从 DDD 课复制：course.css / quiz.js（renderQuiz）/ categorize.js（renderCategorize）
- 约定不变：课程专属文案一律做成参数，不硬编码进组件
- 验证方式：`python3 -m http.server 8732`（**必须非沙箱启动**，沙箱内起的服务外部进程连不上）。
  **2026-07-19 实测：claude-in-chrome 扩展连 localhost 一律 ERR_CONNECTION_REFUSED**（连非沙箱
  服务也不行，127.0.0.1 同样；疑与用户代理设置有关）——**改用 Playwright 插件验证**（browser_navigate
  + evaluate + screenshot，全部正常）。JS/CSS 缓存要加 `?cachebust=N`。
  Playwright 截图/快照会写进工作区（`.playwright-mcp/`、根目录），验证完记得删，已入 .gitignore。
