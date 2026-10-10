#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    // 数字与小数点槽函数
    void onDigitClicked();
    void onDotClicked();

    // 运算符及计算槽函数
    void onOperatorClicked();
    void onEqualClicked();

    // 编辑与清除槽函数
    void onBackspaceClicked();
    void onClearAllClicked();
    void onClearEntryClicked();

private:
    Ui::MainWindow *ui;

    // 计算器核心状态变量
    double m_firstOperand;      // 第一个操作数
    QString m_pendingOperator;  // 当前待运算的双目运算符
    bool m_waitingForOperand;   // 等待输入下一个操作数标记
    bool m_hasCalculated;       // 是否完成单次计算标记
    bool m_isError;             // 错误状态标记

    // 核心业务处理方法
    void inputDigit(const QString &digit);
    void inputDot();
    void inputOperator(const QString &op);
    void calculateEqual();
    void executeBackspace();
    void clearAll();
    void clearEntry();

    // 辅助工具方法
    void updateDisplay(const QString &text);
    void updateExpression(const QString &text);
    QString formatNumber(double value) const;
    void initUI();
    void connectSignals();
};

#endif // MAINWINDOW_H
