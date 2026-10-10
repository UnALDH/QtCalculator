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
    // 数字按键槽函数
    void onDigitClicked();

    // 小数点槽函数
    void onDotClicked();

private:
    Ui::MainWindow *ui;

    // 计算器状态变量
    bool m_waitingForOperand;   // 等待输入新操作数标记
    bool m_hasCalculated;       // 是否刚完成计算标记
    bool m_isError;             // 错误状态标记

    // 核心输入处理逻辑
    void inputDigit(const QString &digit);
    void inputDot();

    // 界面与辅助函数
    void updateDisplay(const QString &text);
    void initUI();
    void connectSignals();
};

#endif // MAINWINDOW_H
