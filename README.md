# QtCalculator - 带键盘事件的现代化计算器

## 1. 项目简介
本项目为东莞理工学院《Qt应用程序开发》课程实验一——**带键盘事件的计算器**。
基于 **Qt 5.12 / Qt 6** 与 C++ 开发，使用 Qt Designer 完成 UI 布局，结合 QSS 实现 Windows 11 Fluent Design 风格的高颜值现代化交互界面。
具备完整的四则运算、单目运算、防重容错保护、异常除零处理以及鼠标按键与物理键盘 100% 逻辑复用能力。

---

## 2. 功能特性

1. **基础四则运算与多操作数连续计算**
   - 支持加（`+`）、减（`-`）、乘（`×`）、除（`÷`）标准运算。
   - 支持连续链式运算（如输入 `2 + 3 × 4 =`，自动在输入新运算符时结算前一步结果）。
   - 智能操作符切换（输入操作符后在输入第二操作数前修改操作符，自动更新操作符而不误算）。
2. **丰富单目运算**
   - 支持平方（`x²`）、开方（`√x`）、倒数（`1/x`）、正负号反转（`+/-`）、百分比（`%`）。
3. **高健壮性异常与边界防护**
   - **除数为 0 保护**：输入 `5 ÷ 0 =` 友好提示“除数不能为零”，阻止崩溃并锁定错误状态。
   - **负数开方保护**：输入 `-9 √x` 提示“无效输入”。
   - **小数点防重输入**：同一操作数内禁止输入多个小数点，等待新数时按 `.` 自动补全为 `0.`。
   - **退格保护**：退格至空或负号时自动归零为 `0`；运算结束后或等待新数阶段禁止破坏性退格。
   - **运算后无缝衔接**：按 `=` 得出结果后，直接按数字自动开启新运算；直接按运算符则以当前结果作为第一操作数继续连算。
4. **键盘事件无缝融合与 100% 逻辑复用**
   - 重写 `keyPressEvent`，将键盘按键映射至对应 `QPushButton` 并通过 `animateClick()` 触发。
   - 带来两项关键优势：
     1. 物理键盘按下时，界面对应按钮产生真实的物理下压/回弹动画反馈。
     2. 物理键盘事件直接触发按钮的 `clicked` 信号与槽函数，鼠标与键盘执行 100% 相同的业务逻辑代码，杜绝冗余重复代码。
5. **现代 Fluent Design 风格样式表 (QSS)**
   - 柔和浅灰背景、高对比度白色数字按键、微软经典蓝 (`#0078d4`) 突出等号、悬停高亮及按下动效反馈。

---

## 3. 键盘键位映射表

| 键盘按键 | 对应计算器功能 | 说明 |
| :--- | :--- | :--- |
| `0` ~ `9` (主键盘/数字小键盘) | `0` ~ `9` | 输入对应数字，界面按钮同步动画反馈 |
| `.` 或 `,` | `.` (小数点) | 自动防重复 |
| `+` (或 `Shift` + `=`) | `＋` (加法) | 双目加法运算符 |
| `-` | `－` (减法) | 双目减法运算符 |
| `*` (或 `Shift` + `8`) | `×` (乘法) | 双目乘法运算符 |
| `/` | `÷` (除法) | 双目除法运算符 |
| `Enter` / `Return` / `=` | `=` (等号) | 执行求值计算 |
| `Backspace` (退格键) | `⌫` (退格) | 删除当前输入末尾字符 |
| `Esc` | `C` (全部清除) | 复位所有状态与操作数 |
| `Delete` | `CE` (清除输入) | 清除当前显示操作数归零 |
| `%` (或 `Shift` + `5`) | `%` (百分比) | 当前数值百分比计算 |

---

## 4. 关键设计与核心代码

### 4.1 如何避免同一操作数重复输入小数点
```cpp
void MainWindow::inputDot()
{
    if (m_isError) {
        clearAll();
    }

    // 若刚完成等号计算，按小数点重置为 "0."
    if (m_hasCalculated) {
        m_hasCalculated = false;
        m_pendingOperator.clear();
        ui->labelExpression->clear();
        updateDisplay("0.");
        m_waitingForOperand = false;
        return;
    }

    // 若等待新操作数时按小数点，自动补全为 "0."
    if (m_waitingForOperand) {
        updateDisplay("0.");
        m_waitingForOperand = false;
        return;
    }

    QString currentText = ui->labelDisplay->text();

    // 防重复输入小数点：检查当前操作数是否已包含小数点
    if (!currentText.contains('.')) {
        currentText += ".";
        updateDisplay(currentText);
    }
}
```

### 4.2 键盘与鼠标逻辑完全复用
```cpp
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 通过 animateClick() 将键盘按键事件直接派发至对应的 QPushButton
    switch (event->key()) {
    case Qt::Key_0: ui->btn0->animateClick(); break;
    case Qt::Key_1: ui->btn1->animateClick(); break;
    // ... (省略其余数字)
    case Qt::Key_Plus: ui->btnAdd->animateClick(); break;
    case Qt::Key_Minus: ui->btnSub->animateClick(); break;
    case Qt::Key_Asterisk: ui->btnMul->animateClick(); break;
    case Qt::Key_Slash: ui->btnDiv->animateClick(); break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Equal: ui->btnEqual->animateClick(); break;
    case Qt::Key_Backspace: ui->btnBackspace->animateClick(); break;
    case Qt::Key_Escape: ui->btnC->animateClick(); break;
    case Qt::Key_Delete: ui->btnCE->animateClick(); break;
    default:
        // 捕获 Shift 组合键输入的文本符号
        if (event->text() == "+") ui->btnAdd->animateClick();
        else if (event->text() == "*") ui->btnMul->animateClick();
        else QMainWindow::keyPressEvent(event);
        break;
    }
}
```

---

## 5. 编译与运行方式

### 使用 Qt Creator（推荐）
1. 打开 Qt Creator。
2. 点击 `文件` -> `打开文件或项目...`，选择本项目目录下的 `QtCalculator.pro`。
3. 选择对应的构建套件（如 Desktop Qt 5.12.11 MinGW 64-bit 或 Qt 6.x）。
4. 点击左下角绿色三角形（或按快捷键 `Ctrl + R`）即可一键编译并运行。

### 使用命令行构建 (MinGW)
```bash
cd D:\Any.Store\Qt.Store\QtCalculator\QtCalculator
qmake QtCalculator.pro
mingw32-make
./release/QtCalculator.exe
```
