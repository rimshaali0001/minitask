#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <sys/types.h>

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QFrame>
#include <QTimer>

#include <vector>

#include "../process_reader.h"
#include "../system_info.h"
#include "../signal_control.h"
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QSplitter *mainSplitter;
    QWidget *leftPanel;
    QWidget *rightPanel;

    QLineEdit *searchBox;
    QTableWidget *processTable;

    QPushButton *btnRefresh;
    QPushButton *btnPause;
    QPushButton *btnResume;
    QPushButton *btnTerminate;
    QPushButton *btnKill;

    QProgressBar *cpuBar;
    QProgressBar *memBar;

    // Backend objects
    ProcessReader processReader;
    CpuMonitor cpuMonitor;

    // Stores the latest process list
    std::vector<ProcessInfo> allProcesses;

    // Automatic refresh timer
    QTimer *refreshTimer;

    void setupUI();
    void applyTheme();

    void populateProcessTable(
        const std::vector<ProcessInfo> &processes
    );

private slots:
    void refreshProcesses();
    void filterProcesses(const QString &text);

    void terminateProcess();
    void killProcess();

    void pauseProcess();
    void resumeProcess();

    bool getSelectedPid(pid_t &pid);
};

#endif // MAINWINDOW_H
