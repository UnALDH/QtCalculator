#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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
    // 初始化主显示区为 "0"
    updateDisplay("0");
    ui->labelExpression->clear();
}

void MainWindow::connectSignals()
{
    // 关联数字按键 (0-9)
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

    // 关联小数点按键
    connect(ui->btnDot, &QPushButton::clicked, this, &MainWindow::onDotClicked);
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

void MainWindow::inputDigit(const QString &digit)
{
    // 错误状态下优先重置
    if (m_isError) {
        m_isError = false;
        updateDisplay(digit);
        return;
    }

    // 若刚才刚计算完结果，直接输入数字视作开启全新一轮运算
    if (m_hasCalculated) {
        m_hasCalculated = false;
        ui->labelExpression->clear();
        updateDisplay(digit);
        return;
    }

    // 若正等待输入新操作数
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
    if (m_isError) return;

    // 若处于刚计算完状态，重置为 "0."
    if (m_hasCalculated) {
        m_hasCalculated = false;
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

void MainWindow::updateDisplay(const QString &text)
{
    ui->labelDisplay->setText(text);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
}
