# 变更日志

所有重要的项目变更将记录在此文件中。

格式基于 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.1.0/)，
且项目遵循 [语义化版本](https://semver.org/lang/zh-CN/) 规范。

## [Unreleased] - 2026

### 许可证 (License)

- **项目许可证统一为 GNU General Public License v3.0**（SPDX: `GPL-3.0-only`）
  - 根目录 `LICENSE` 更换为 GPL-3.0 **官方全文**（此前为 CERN-OHL-S v2 的摘要版，导致 GitHub 无法识别许可证类型）
  - 移除 `firmware/LICENSE`（原 MIT），固件改由根目录 GPL-3.0 统一覆盖，避免同一仓库内多重许可冲突
  - 此前「硬件 CERN-OHL-S-2.0 / 固件 MIT / 文档 CC-BY-4.0」的三许可方案废止

### 文档 (Documentation)

- README 重构：新增许可证、项目类型、固件语言、EDA 工具、PR 欢迎等徽章；补充安全警示；修正仓库结构树与图片清单（移除未提交的 `MDK-ARM/`，补 `README.en.md`、`docs/README.md`、`.github/`）
- 新增英文版 `README.en.md`，并在中英文版本顶部加入语言切换
- 新增 `docs/README.md` 文档索引
- 统一「监测板 / 监测扩展板」命名为「电源拓展板」，与 CHANGELOG 中的重命名记录保持一致
- 修正固件文档与 `board.h`：OLED 接口实为 **SPI**（PA5/PA7 + PB0/PB1/PB2），删除已废弃的 I2C（PB6/PB7）引脚定义与文档描述
- 更新版权年份为 2024-2026
- 新增 `.github/` Issue 与 Pull Request 模板

### 新增 (Added)

- 可调电源板 PCB 设计（EasyEDA Pro 工程 + Gerber）
- 电源拓展板 PCB 设计（BOM 已就绪，源文件待上传）
- 两路可调 DC 输出（XL4015 + WH148 电位器）
- +3.3V / +5V / -5V 三路固定输出
- 三路可选输入（DC 插座 / USB-C PD / 接线端子）
- 输入保护：保险丝 + TVS + 反接 MOS
- USB-C PD 诱骗（CH224K，支持 9/12/15/20V）
- 四通道电压电流监测（INA4181 + APM32F030）
- 0.96" OLED 显示接口（SPI）
- 固件骨架代码（board、ADC、INA4181、SSD1306 SPI、UART 驱动）
- 双页面 OLED 显示与按键交互
- BOM 清单（电源板 52 种，拓展板 16 种）
- 项目文档（README、硬件设计、安全指南、测试计划）

### 变更 (Changed)

- 板卡名称由「监测扩展板」更名为「电源拓展板」
- OLED 接口由 I2C 改为 SPI（4 线 SPI，速度更快）
- XL6019 负压电路验证通过，−5V 输出工作正常
- 两块 PCB 尺寸确认：100 mm × 100 mm
- 拓展板接口分区定义：+5V / +3.3V / ±5V / ADJ / BUCK
- 电源板完成焊接验证，各路输出工作正常
- 添加电源板焊接实物照片

### 已知问题 (Known Issues)

- 拓展板源文件和 Gerber 待上传
- Keil MDK 工程尚未提交，当前仅有源码
- 固件为骨架版本，校准和告警功能待完善
- 引脚复用冲突：拓展板 PA5 / PA6 / PA7 同时用作 ADC 电流采样输入与 SPI1（OLED），硬件上不能并行使用，最终版需重新分配
- 所有电气参数未经过实际测试验证
- 保护功能未经过完整验证

### 待完成 (TODO)

- [ ] 电源板原型完整测试
- [ ] 拓展板设计文件开源
- [ ] 拓展板原型验证
- [ ] 固件校准系统和告警功能
- [ ] 原理图 PDF 和连接器引脚图
- [ ] 装配指南

---

## 版本说明

版本号格式：`MAJOR.MINOR.PATCH`

- **MAJOR**：重大架构变更，不兼容的硬件/固件接口修改
- **MINOR**：新增功能，向后兼容
- **PATCH**：Bug 修复、文档更新、小优化

### 硬件版本标识

- PCB 丝印上标注版本号（如 v0.1）
- 大版本变更时重新打板
- 小修改可通过飞线/补焊实现
