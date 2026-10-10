#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    if (m_isError) {
        clearAll();
    }

    if (m_hasCalculated) {
        m_hasCalculated = false;
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
        if (digit == "0") return;
        currentText = digit;
    } else {
        if (currentText.length() >= 16) return;
        currentText += digit;
    }

    updateDisplay(currentText);
}

void MainWindow::inputDot()
{
    if (m_isError) return;

    if (m_hasCalculated) {
        m_hasCalculated = false;
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

    m_firstOperand = ui->labelDisplay->text().toDouble();
    m_pendingOperator = op;
    m_waitingForOperand = true;
    updateExpression(QString("%1 %2").arg(formatNumber(m_firstOperand)).arg(op));
}

void MainWindow::calculateEqual()
{
    if (m_isError || m_pendingOperator.isEmpty()) return;

    double secondOperand = ui->labelDisplay->text().toDouble();
    double result = 0.0;

    if (m_pendingOperator == "＋" || m_pendingOperator == "+") {
        result = m_firstOperand + secondOperand;
    } else if (m_pendingOperator == "－" || m_pendingOperator == "-") {
        result = m_firstOperand - secondOperand;
    } else if (m_pendingOperator == "×" || m_pendingOperator == "*") {
        result = m_firstOperand * secondOperand;
    } else if (m_pendingOperator == "÷" || m_pendingOperator == "/") {
        result = m_firstOperand / secondOperand;
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

void MainWindow::executeBackspace()
{
    if (m_isError || m_waitingForOperand || m_hasCalculated) return;

    QString currentText = ui->labelDisplay->text();
    currentText.chop(1);
    if (currentText.isEmpty() || currentText == "-") {
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
    QMainWindow::keyPressEvent(event);
}
