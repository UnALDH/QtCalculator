#include "mainwindow.h"
#include "ui_mainwindow.h"
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
    // 若当前处于错误状态，按数字自动清除错误并开启新输入
    if (m_isError) {
        clearAll();
    }

    // 若刚完成一轮等号计算，直接输入数字视作开启全新运算
    if (m_hasCalculated) {
        m_hasCalculated = false;
        m_pendingOperator.clear();
        ui->labelExpression->clear();
        updateDisplay(digit);
        return;
    }

    // 若正等待输入新操作数，替换显示并重置等待标记
    if (m_waitingForOperand) {
        updateDisplay(digit);
        m_waitingForOperand = false;
        return;
    }

    QString currentText = ui->labelDisplay->text();

    // 避免首位无意义多零 (如 0005)
    if (currentText == "0") {
        if (digit == "0") {
            return;
        }
        currentText = digit;
    } else {
        // 限制操作数长度，防止超出界面
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

void MainWindow::inputOperator(const QString &op)
{
    if (m_isError) return;

    // 边界情况 1：连续操作符切换
    // 若用户刚按完运算符（处于等待第二操作数状态），又改按了另一个运算符，则仅替换待运算操作符
    if (m_waitingForOperand && !m_hasCalculated) {
        m_pendingOperator = op;
        updateExpression(QString("%1 %2").arg(formatNumber(m_firstOperand)).arg(op));
        return;
    }

    // 边界情况 2：连续运算 (例如 2 + 3 + 4)
    // 若之前已有待运算的操作符，且用户已输入了第二操作数，则先执行前面的运算
    if (!m_pendingOperator.isEmpty() && !m_waitingForOperand) {
        double secondOperand = ui->labelDisplay->text().toDouble();
        double intermediateResult = 0.0;
        if (!executePendingOperation(secondOperand, intermediateResult)) {
            return; // 发生除零等错误，已在 executePendingOperation 中处理
        }
        m_firstOperand = intermediateResult;
        updateDisplay(formatNumber(m_firstOperand));
    } else {
        // 正常首次按运算符或等号后继续运算
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
        // 边界情况 3：除以零检测
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

    // 若没有待处理的操作符，不做计算
    if (m_pendingOperator.isEmpty()) {
        return;
    }

    double secondOperand = ui->labelDisplay->text().toDouble();
    double result = 0.0;

    if (!executePendingOperation(secondOperand, result)) {
        return; // 除以零错误已处理
    }

    // 更新表达式栏为完整算式：例如 "12 + 34 ="
    updateExpression(QString("%1 %2 %3 =").arg(formatNumber(m_firstOperand))
                                          .arg(m_pendingOperator)
                                          .arg(formatNumber(secondOperand)));

    // 更新主显示区为计算结果
    updateDisplay(formatNumber(result));

    m_firstOperand = result;
    m_pendingOperator.clear();
    m_waitingForOperand = true;
    m_hasCalculated = true;
}

void MainWindow::executeBackspace()
{
    // 错误状态、等待新输入、刚计算完结果时不允许退格
    if (m_isError || m_waitingForOperand || m_hasCalculated) return;

    QString currentText = ui->labelDisplay->text();
    currentText.chop(1);

    // 退格至空字符或单一负号时，归零
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
    // 使用 'g' 格式，有效位数 12 位，自动去除末尾多余零
    return QString::number(value, 'g', 12);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
}
