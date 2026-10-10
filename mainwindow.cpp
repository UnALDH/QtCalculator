#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_firstOperand(0.0)
    , m_pendingOperator("")
    , m_waitingForOperand(false)
    , m_hasCalculated(false)
    , m_isError(false)
{
    ui->setupUi(this);
    initUI();
    connectSignals();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI()
{
    updateDisplay("0");
    ui->labelExpression->clear();

    // 遍历所有按钮，设置焦点策略为 NoFocus，确保键盘事件始终由 MainWindow 捕获
    const auto buttons = findChildren<QPushButton*>();
    for (QPushButton *btn : buttons) {
        btn->setFocusPolicy(Qt::NoFocus);
    }

    // 设置现代化流畅界面样式表 (Fluent Design / Windows 计算器风格)
    QString qss = R"(
        QMainWindow {
            background-color: #f3f3f3;
        }

        /* 顶部表达式历史记录显示 */
        QLabel#labelExpression {
            color: #5c5c5c;
            font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
            font-size: 14px;
            font-weight: normal;
            padding-right: 12px;
            background: transparent;
        }

        /* 主数据显示区 */
        QLabel#labelDisplay {
            color: #1b1b1b;
            font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
            font-size: 38px;
            font-weight: bold;
            padding-right: 10px;
            background: transparent;
        }

        /* 通用按键基础样式 */
        QPushButton {
            font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
            border-radius: 6px;
            border: 1px solid #e2e2e2;
            font-size: 16px;
            min-height: 44px;
            outline: none;
        }

        /* 数字按键与小数点：清爽明亮纯白质感 */
        QPushButton#btn0, QPushButton#btn1, QPushButton#btn2,
        QPushButton#btn3, QPushButton#btn4, QPushButton#btn5,
        QPushButton#btn6, QPushButton#btn7, QPushButton#btn8,
        QPushButton#btn9, QPushButton#btnDot {
            background-color: #ffffff;
            color: #1a1a1a;
            font-size: 18px;
            font-weight: 600;
        }
        QPushButton#btn0:hover, QPushButton#btn1:hover, QPushButton#btn2:hover,
        QPushButton#btn3:hover, QPushButton#btn4:hover, QPushButton#btn5:hover,
        QPushButton#btn6:hover, QPushButton#btn7:hover, QPushButton#btn8:hover,
        QPushButton#btn9:hover, QPushButton#btnDot:hover {
            background-color: #f7f7f7;
            border-color: #d0d0d0;
        }
        QPushButton#btn0:pressed, QPushButton#btn1:pressed, QPushButton#btn2:pressed,
        QPushButton#btn3:pressed, QPushButton#btn4:pressed, QPushButton#btn5:pressed,
        QPushButton#btn6:pressed, QPushButton#btn7:pressed, QPushButton#btn8:pressed,
        QPushButton#btn9:pressed, QPushButton#btnDot:pressed {
            background-color: #ededed;
        }

        /* 运算符与功能键：柔和浅灰 */
        QPushButton#btnAdd, QPushButton#btnSub, QPushButton#btnMul, QPushButton#btnDiv,
        QPushButton#btnPercent, QPushButton#btnCE, QPushButton#btnC, QPushButton#btnBackspace,
        QPushButton#btnReciprocal, QPushButton#btnSquare, QPushButton#btnSqrt, QPushButton#btnSign {
            background-color: #f8f8f8;
            color: #242424;
            font-size: 16px;
        }
        QPushButton#btnAdd:hover, QPushButton#btnSub:hover, QPushButton#btnMul:hover, QPushButton#btnDiv:hover,
        QPushButton#btnPercent:hover, QPushButton#btnCE:hover, QPushButton#btnC:hover, QPushButton#btnBackspace:hover,
        QPushButton#btnReciprocal:hover, QPushButton#btnSquare:hover, QPushButton#btnSqrt:hover, QPushButton#btnSign:hover {
            background-color: #eeeeee;
            border-color: #d6d6d6;
        }
        QPushButton#btnAdd:pressed, QPushButton#btnSub:pressed, QPushButton#btnMul:pressed, QPushButton#btnDiv:pressed,
        QPushButton#btnPercent:pressed, QPushButton#btnCE:pressed, QPushButton#btnC:pressed, QPushButton#btnBackspace:pressed,
        QPushButton#btnReciprocal:pressed, QPushButton#btnSquare:pressed, QPushButton#btnSqrt:pressed, QPushButton#btnSign:pressed {
            background-color: #e2e2e2;
        }

        /* 等号按键：醒目主题强调色 (Windows 蓝) */
        QPushButton#btnEqual {
            background-color: #0078d4;
            color: #ffffff;
            border: 1px solid #006cbd;
            font-size: 22px;
            font-weight: bold;
        }
        QPushButton#btnEqual:hover {
            background-color: #1084d8;
            border-color: #0e7ac7;
        }
        QPushButton#btnEqual:pressed {
            background-color: #0067b8;
        }
    )";
    this->setStyleSheet(qss);
}

void MainWindow::connectSignals()
{
    // 数字按键 (0-9)
    connect(ui->btn0, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn1, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn2, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn3, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn4, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn5, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn6, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn7, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn8, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn9, &QPushButton::clicked, this, &MainWindow::onDigitClicked);

    // 小数点
    connect(ui->btnDot, &QPushButton::clicked, this, &MainWindow::onDotClicked);

    // 双目运算符
    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::onOperatorClicked);
    connect(ui->btnSub, &QPushButton::clicked, this, &MainWindow::onOperatorClicked);
    connect(ui->btnMul, &QPushButton::clicked, this, &MainWindow::onOperatorClicked);
    connect(ui->btnDiv, &QPushButton::clicked, this, &MainWindow::onOperatorClicked);

    // 单目运算符
    connect(ui->btnSquare, &QPushButton::clicked, this, &MainWindow::onSquareClicked);
    connect(ui->btnSqrt, &QPushButton::clicked, this, &MainWindow::onSqrtClicked);
    connect(ui->btnReciprocal, &QPushButton::clicked, this, &MainWindow::onReciprocalClicked);
    connect(ui->btnSign, &QPushButton::clicked, this, &MainWindow::onSignClicked);
    connect(ui->btnPercent, &QPushButton::clicked, this, &MainWindow::onPercentClicked);

    // 等号
    connect(ui->btnEqual, &QPushButton::clicked, this, &MainWindow::onEqualClicked);

    // 退格与清除
    connect(ui->btnBackspace, &QPushButton::clicked, this, &MainWindow::onBackspaceClicked);
    connect(ui->btnC, &QPushButton::clicked, this, &MainWindow::onClearAllClicked);
    connect(ui->btnCE, &QPushButton::clicked, this, &MainWindow::onClearEntryClicked);
}

void MainWindow::onDigitClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        inputDigit(button->text());
    }
}

void MainWindow::onDotClicked()
{
    inputDot();
}

void MainWindow::onOperatorClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        inputOperator(button->text());
    }
}

void MainWindow::onEqualClicked()
{
    calculateEqual();
}

void MainWindow::onSquareClicked()
{
    executeSquare();
}

void MainWindow::onSqrtClicked()
{
    executeSqrt();
}

void MainWindow::onReciprocalClicked()
{
    executeReciprocal();
}

void MainWindow::onSignClicked()
{
    toggleSign();
}

void MainWindow::onPercentClicked()
{
    executePercent();
}

void MainWindow::onBackspaceClicked()
{
    executeBackspace();
}

void MainWindow::onClearAllClicked()
{
    clearAll();
}

void MainWindow::onClearEntryClicked()
{
    clearEntry();
}

void MainWindow::inputDigit(const QString &digit)
{
    if (m_isError) {
        clearAll();
    }

    if (m_hasCalculated) {
        m_hasCalculated = false;
        m_pendingOperator.clear();
        ui->labelExpression->clear();
        updateDisplay(digit);
        return;
    }

    if (m_waitingForOperand) {
        updateDisplay(digit);
        m_waitingForOperand = false;
        return;
    }

    QString currentText = ui->labelDisplay->text();

    if (currentText == "0") {
        if (digit == "0") {
            return;
        }
        currentText = digit;
    } else {
        if (currentText.length() >= 16) {
            return;
        }
        currentText += digit;
    }

    updateDisplay(currentText);
}

void MainWindow::inputDot()
{
    if (m_isError) {
        clearAll();
    }

    if (m_hasCalculated) {
        m_hasCalculated = false;
        m_pendingOperator.clear();
        ui->labelExpression->clear();
        updateDisplay("0.");
        m_waitingForOperand = false;
        return;
    }

    if (m_waitingForOperand) {
        updateDisplay("0.");
        m_waitingForOperand = false;
        return;
    }

    QString currentText = ui->labelDisplay->text();

    if (!currentText.contains('.')) {
        currentText += ".";
        updateDisplay(currentText);
    }
}

void MainWindow::inputOperator(const QString &op)
{
    if (m_isError) return;

    // 连续操作符切换
    if (m_waitingForOperand && !m_hasCalculated) {
        m_pendingOperator = op;
        updateExpression(QString("%1 %2").arg(formatNumber(m_firstOperand)).arg(op));
        return;
    }

    // 连续运算 (例如 2 + 3 + 4)
    if (!m_pendingOperator.isEmpty() && !m_waitingForOperand) {
        double secondOperand = ui->labelDisplay->text().toDouble();
        double intermediateResult = 0.0;
        if (!executePendingOperation(secondOperand, intermediateResult)) {
            return;
        }
        m_firstOperand = intermediateResult;
        updateDisplay(formatNumber(m_firstOperand));
    } else {
        m_firstOperand = ui->labelDisplay->text().toDouble();
    }

    m_pendingOperator = op;
    m_waitingForOperand = true;
    m_hasCalculated = false;
    updateExpression(QString("%1 %2").arg(formatNumber(m_firstOperand)).arg(op));
}

bool MainWindow::executePendingOperation(double secondOperand, double &result)
{
    if (m_pendingOperator == "＋" || m_pendingOperator == "+") {
        result = m_firstOperand + secondOperand;
    } else if (m_pendingOperator == "－" || m_pendingOperator == "-") {
        result = m_firstOperand - secondOperand;
    } else if (m_pendingOperator == "×" || m_pendingOperator == "*") {
        result = m_firstOperand * secondOperand;
    } else if (m_pendingOperator == "÷" || m_pendingOperator == "/") {
        if (qFuzzyIsNull(secondOperand) || secondOperand == 0.0) {
            triggerError("除数不能为零");
            return false;
        }
        result = m_firstOperand / secondOperand;
    } else {
        result = secondOperand;
    }
    return true;
}

void MainWindow::calculateEqual()
{
    if (m_isError) return;

    if (m_pendingOperator.isEmpty()) {
        return;
    }

    double secondOperand = ui->labelDisplay->text().toDouble();
    double result = 0.0;

    if (!executePendingOperation(secondOperand, result)) {
        return;
    }

    updateExpression(QString("%1 %2 %3 =").arg(formatNumber(m_firstOperand))
                                          .arg(m_pendingOperator)
                                          .arg(formatNumber(secondOperand)));

    updateDisplay(formatNumber(result));

    m_firstOperand = result;
    m_pendingOperator.clear();
    m_waitingForOperand = true;
    m_hasCalculated = true;
}

void MainWindow::executeSquare()
{
    if (m_isError) return;
    double val = ui->labelDisplay->text().toDouble();
    double result = val * val;
    updateExpression(QString("sqr(%1)").arg(formatNumber(val)));
    updateDisplay(formatNumber(result));
    m_waitingForOperand = true;
}

void MainWindow::executeSqrt()
{
    if (m_isError) return;
    double val = ui->labelDisplay->text().toDouble();
    if (val < 0) {
        triggerError("无效输入");
        return;
    }
    double result = std::sqrt(val);
    updateExpression(QString("√(%1)").arg(formatNumber(val)));
    updateDisplay(formatNumber(result));
    m_waitingForOperand = true;
}

void MainWindow::executeReciprocal()
{
    if (m_isError) return;
    double val = ui->labelDisplay->text().toDouble();
    if (qFuzzyIsNull(val) || val == 0.0) {
        triggerError("除数不能为零");
        return;
    }
    double result = 1.0 / val;
    updateExpression(QString("1/(%1)").arg(formatNumber(val)));
    updateDisplay(formatNumber(result));
    m_waitingForOperand = true;
}

void MainWindow::toggleSign()
{
    if (m_isError) return;
    QString currentText = ui->labelDisplay->text();
    if (currentText == "0" || currentText == "0.") return;

    if (currentText.startsWith("-")) {
        currentText.remove(0, 1);
    } else {
        currentText.prepend("-");
    }
    updateDisplay(currentText);
}

void MainWindow::executePercent()
{
    if (m_isError) return;
    double currentVal = ui->labelDisplay->text().toDouble();
    double percentVal = 0.0;

    if (!m_pendingOperator.isEmpty() && (m_pendingOperator == "＋" || m_pendingOperator == "－")) {
        percentVal = m_firstOperand * (currentVal / 100.0);
    } else {
        percentVal = currentVal / 100.0;
    }

    updateDisplay(formatNumber(percentVal));
}

void MainWindow::executeBackspace()
{
    if (m_isError || m_waitingForOperand || m_hasCalculated) return;

    QString currentText = ui->labelDisplay->text();
    currentText.chop(1);

    if (currentText.isEmpty() || currentText == "-" || currentText == "-0") {
        currentText = "0";
    }
    updateDisplay(currentText);
}

void MainWindow::clearAll()
{
    m_firstOperand = 0.0;
    m_pendingOperator.clear();
    m_waitingForOperand = false;
    m_hasCalculated = false;
    m_isError = false;
    ui->labelExpression->clear();
    updateDisplay("0");
}

void MainWindow::clearEntry()
{
    if (m_isError) {
        clearAll();
        return;
    }
    updateDisplay("0");
}

void MainWindow::triggerError(const QString &message)
{
    m_isError = true;
    m_pendingOperator.clear();
    m_waitingForOperand = true;
    m_hasCalculated = false;
    updateDisplay(message);
}

void MainWindow::updateDisplay(const QString &text)
{
    ui->labelDisplay->setText(text);
}

void MainWindow::updateExpression(const QString &text)
{
    ui->labelExpression->setText(text);
}

QString MainWindow::formatNumber(double value) const
{
    return QString::number(value, 'g', 12);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 键盘按键与界面按钮映射，通过 animateClick() 实现：
    // 1. 产生按钮被按下的视觉反馈动画
    // 2. 触发对应按钮的 clicked 信号，完美复用鼠标点击槽函数逻辑
    switch (event->key()) {
    case Qt::Key_0:
        ui->btn0->animateClick();
        break;
    case Qt::Key_1:
        ui->btn1->animateClick();
        break;
    case Qt::Key_2:
        ui->btn2->animateClick();
        break;
    case Qt::Key_3:
        ui->btn3->animateClick();
        break;
    case Qt::Key_4:
        ui->btn4->animateClick();
        break;
    case Qt::Key_5:
        ui->btn5->animateClick();
        break;
    case Qt::Key_6:
        ui->btn6->animateClick();
        break;
    case Qt::Key_7:
        ui->btn7->animateClick();
        break;
    case Qt::Key_8:
        ui->btn8->animateClick();
        break;
    case Qt::Key_9:
        ui->btn9->animateClick();
        break;
    case Qt::Key_Period:
    case Qt::Key_Comma:
        ui->btnDot->animateClick();
        break;
    case Qt::Key_Plus:
        ui->btnAdd->animateClick();
        break;
    case Qt::Key_Minus:
        ui->btnSub->animateClick();
        break;
    case Qt::Key_Asterisk:
        ui->btnMul->animateClick();
        break;
    case Qt::Key_Slash:
        ui->btnDiv->animateClick();
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Equal:
        ui->btnEqual->animateClick();
        break;
    case Qt::Key_Backspace:
        ui->btnBackspace->animateClick();
        break;
    case Qt::Key_Escape:
        ui->btnC->animateClick();
        break;
    case Qt::Key_Delete:
        ui->btnCE->animateClick();
        break;
    case Qt::Key_Percent:
        ui->btnPercent->animateClick();
        break;
    default:
        // 处理部分通过 Shift 或字符输入的符号（例如 Shift+5 输入 %，Shift+= 输入 +）
        if (event->text() == "+") {
            ui->btnAdd->animateClick();
        } else if (event->text() == "-") {
            ui->btnSub->animateClick();
        } else if (event->text() == "*") {
            ui->btnMul->animateClick();
        } else if (event->text() == "/") {
            ui->btnDiv->animateClick();
        } else if (event->text() == "=") {
            ui->btnEqual->animateClick();
        } else if (event->text() == "%") {
            ui->btnPercent->animateClick();
        } else if (event->text() == ".") {
            ui->btnDot->animateClick();
        } else {
            QMainWindow::keyPressEvent(event);
        }
        break;
    }
}
