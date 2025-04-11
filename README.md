# Arduino UNO LED 與小車項目

## 項目概述

本項目為基於 **Arduino UNO** 的電子實習課程，專注於 LED 控制與小車應用。使用 **CLion** 搭配 **PlatformIO** 進行程式設計，實現 LED 燈光效果及簡單小車的移動控制（如前進、後退、避障）。旨在培養電路搭建、感測器應用及嵌入式開發能力。

### 功能
- **LED 控制**：點亮
- **小車控制**：馬達驅動、方向控制、超音波避障
- 最終目標：製作自動避障小車，搭配 LED 狀態指示

## 環境設置

### 硬體需求
- Arduino UNO CH340 開發板

### 軟體需求
- **CLion**（JetBrains 最新版）
- **PlatformIO** 插件（CLion 內安裝）
- Arduino UNO 驅動程式（CH340）

### 安装步骤
1. 安裝 CLion，於插件市場安裝 PlatformIO。
2. 創建 PlatformIO 項目，選擇 Arduino UNO 板。
3. 配置 `platformio.ini`（`framework = arduino` 及程式庫）。
4. 連接 Arduino UNO，確認通訊埠。
5. 測試：運行 `Blink` 範例，驗證 LED 閃爍。