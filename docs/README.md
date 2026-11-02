# 文档索引

本目录存放 OpenPowerLab 的技术文档。根目录的 `README.md` 是项目总览，本页负责把读者按角色分流到具体文档。

## 阅读路径

| 你是 | 建议顺序 |
| --- | --- |
| **第一次了解项目** | [根目录 README](../README.md) → [硬件设计文档](hardware.md) → [安全使用指南](safety.md) |
| **想复刻制作** | [快速开始](../README.md#快速开始) → [电源板说明](../hardware/power-board/README.md) → [BOM](../hardware/power-board/BOM.csv) → [安全使用指南](safety.md) → [测试计划](testing.md) |
| **只想用拓展板做监测** | [拓展板说明](../hardware/monitoring-board/README.md) → [固件说明](../firmware/README.md) → [安全使用指南](safety.md) |
| **参与贡献** | [贡献指南](../CONTRIBUTING.md) → [变更日志](../CHANGELOG.md) |

## 文档清单

| 文档 | 内容 | 状态 |
| --- | --- | --- |
| [hardware.md](hardware.md) | 系统框图、输入保护、稳压通道、USB-C PD、电流/电压检测电路、连接器引脚定义、PCB 设计说明 | 已随设计更新 |
| [safety.md](safety.md) | 通用安全规则、上电检查清单、首次上电步骤、输入/输出安全、静电防护、故障排查、紧急处理 | 完整 |
| [testing.md](testing.md) | 测试环境与仪器要求、电源板 7 项测试、拓展板 4 项测试、记录模板、合格判定标准 | 计划，未实测 |
| [../hardware/power-board/README.md](../hardware/power-board/README.md) | 可调电源板：规格、输入/输出接口、电路原理、核心元器件、制板说明 | 已验证部分标注 |
| [../hardware/monitoring-board/README.md](../hardware/monitoring-board/README.md) | 电源拓展板：规格、接口分区、检测原理、引脚分配、校准方法 | 设计阶段 |
| [../firmware/README.md](../firmware/README.md) | 固件架构、外设支持、引脚分配、串口协议、校准算法、开发计划 | 骨架版本 |
| [../CONTRIBUTING.md](../CONTRIBUTING.md) | Issue/PR 流程、提交信息规范、代码风格、许可证约定 | 完整 |
| [../CHANGELOG.md](../CHANGELOG.md) | 按版本记录的变更、已知问题、待办 | 持续更新 |

## 尚未提供的文档

以下文档在 [开发路线图](../README.md#开发路线图) 中列为待完成项，欢迎贡献：

- 原理图 PDF（当前仅提供 EasyEDA Pro 源工程）
- 连接器引脚定义图（图文版，现有文字表格见 [hardware.md](hardware.md#连接器引脚定义)）
- 装配指南（焊接顺序、关键器件方向、上电前复查项）
- 实测测试报告（按 [testing.md](testing.md) 的项目填写）

## 文档约定

- 使用 Markdown，中英文版本分别维护（`README.md` 与 `README.en.md`）
- 提示信息使用 GitHub 风格 callout：`> [!NOTE]`、`> [!IMPORTANT]`、`> [!WARNING]`
- 板卡统一命名为「可调电源板」与「电源拓展板」，不再使用「监测板」「监测扩展板」
- 涉及未实测的参数，须显式标注「标称」或「未验证」
- 修订文档时同步更新 [CHANGELOG.md](../CHANGELOG.md)
