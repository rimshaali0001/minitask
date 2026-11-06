#include "mainwindow.h"
#include "../signal_control.h"
#include <sys/types.h>

#include <QHeaderView>
#include <QTableWidgetItem>
#include <QString>
#include <QMessageBox>

// Constructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("MiniTask - System Monitor");
    resize(1100, 700);

    setupUI();
    applyTheme();

    connect(btnPause, &QPushButton::clicked,
            this, &MainWindow::pauseProcess);

    connect(btnResume, &QPushButton::clicked,
            this, &MainWindow::resumeProcess);

    connect(btnTerminate, &QPushButton::clicked,
            this, &MainWindow::terminateProcess);

    connect(btnKill, &QPushButton::clicked,
            this, &MainWindow::killProcess);

    // Connect Refresh button
    connect(btnRefresh, &QPushButton::clicked,
            this, &MainWindow::refreshProcesses);

    // Connect Search box
    connect(searchBox, &QLineEdit::textChanged,
            this, &MainWindow::filterProcesses);

    // Create automatic refresh timer
    refreshTimer = new QTimer(this);

    connect(refreshTimer, &QTimer::timeout,
            this, &MainWindow::refreshProcesses);

    // Load processes immediately
    refreshProcesses();

    // Refresh every second
    refreshTimer->start(1000);
}

MainWindow::~MainWindow() {}


void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    mainSplitter = new QSplitter(Qt::Horizontal);

    // LEFT PANEL
    leftPanel = new QWidget();

    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 10, 0);

    QLabel *lblProcesses = new QLabel("Processes");
    lblProcesses->setStyleSheet(
        "font-size: 16px; font-weight: bold; margin-bottom: 5px;"
    );

    leftLayout->addWidget(lblProcesses);

    // Search and Refresh
    QHBoxLayout *searchLayout = new QHBoxLayout();

    searchBox = new QLineEdit();
    searchBox->setPlaceholderText("Search app name or PID...");

    btnRefresh = new QPushButton("Refresh");

    searchLayout->addWidget(searchBox);
    searchLayout->addWidget(btnRefresh);

    leftLayout->addLayout(searchLayout);

    // Process Table
    processTable = new QTableWidget(0, 5);

    processTable->setHorizontalHeaderLabels({
        "PID", "App Name", "State", "CPU %", "Memory %"
    });

    processTable->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch
    );

    processTable->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    processTable->setSelectionMode(
        QAbstractItemView::SingleSelection
    );

    processTable->setSortingEnabled(true);
    processTable->verticalHeader()->setVisible(false);

    leftLayout->addWidget(processTable);

    // Action Buttons
    QHBoxLayout *controlLayout = new QHBoxLayout();

    btnPause = new QPushButton("Pause");
    btnResume = new QPushButton("Resume");
    btnTerminate = new QPushButton("Terminate");
    btnKill = new QPushButton("Kill");

    controlLayout->addWidget(btnPause);
    controlLayout->addWidget(btnResume);
    controlLayout->addStretch();
    controlLayout->addWidget(btnTerminate);
    controlLayout->addWidget(btnKill);

    leftLayout->addLayout(controlLayout);

    // RIGHT PANEL
    rightPanel = new QWidget();

    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(10, 0, 0, 0);

    // CPU Section
    QFrame *cpuFrame = new QFrame();
    cpuFrame->setObjectName("metricFrame");

    QVBoxLayout *cpuLayout = new QVBoxLayout(cpuFrame);

    QLabel *lblCpu = new QLabel("CPU Usage");

    lblCpu->setStyleSheet(
        "font-size: 14px; font-weight: bold;"
    );

    cpuBar = new QProgressBar();
    cpuBar->setRange(0, 100);
    cpuBar->setFixedHeight(80);

    cpuLayout->addWidget(lblCpu);
    cpuLayout->addWidget(cpuBar);

    rightLayout->addWidget(cpuFrame);

    // Memory Section
    QFrame *memFrame = new QFrame();
    memFrame->setObjectName("metricFrame");

    QVBoxLayout *memLayout = new QVBoxLayout(memFrame);

    QLabel *lblMem = new QLabel("Memory Usage");

    lblMem->setStyleSheet(
        "font-size: 14px; font-weight: bold;"
    );

    memBar = new QProgressBar();
    memBar->setRange(0, 100);
    memBar->setFixedHeight(80);

    memLayout->addWidget(lblMem);
    memLayout->addWidget(memBar);

    rightLayout->addWidget(memFrame);

    rightLayout->addStretch();

    // Combine Panels
    mainSplitter->addWidget(leftPanel);
    mainSplitter->addWidget(rightPanel);

    mainSplitter->setStretchFactor(0, 6);
    mainSplitter->setStretchFactor(1, 4);

    mainLayout->addWidget(mainSplitter);

    setCentralWidget(centralWidget);
}


// THEME
void MainWindow::applyTheme()
{
    QString darkTheme =
        "QWidget { background-color: #1e1e2e; color: #cdd6f4; font-family: 'Segoe UI', Ubuntu, sans-serif; }"

        "QLineEdit { background-color: #313244; border: 1px solid #45475a; border-radius: 6px; padding: 5px; color: #cdd6f4; }"

        "QTableWidget { background-color: #232334; border-radius: 8px; border: 1px solid #313244; gridline-color: #313244; }"

        "QHeaderView::section { background-color: #181825; padding: 4px; border: none; font-weight: bold; }"

        "QTableWidget::item:selected { background-color: #45475a; }"

        "QPushButton { background-color: #313244; color: #cdd6f4; border-radius: 6px; padding: 8px 15px; font-weight: bold; }"

        "QPushButton:hover { background-color: #45475a; }"

        "QPushButton#btnRefresh { background-color: #cba6f7; color: #11111b; }"

        "QPushButton#btnRefresh:hover { background-color: #b4befe; }"

        "QFrame#metricFrame { background-color: #232334; border-radius: 8px; padding: 10px; }"

        "QProgressBar { background-color: #313244; border-radius: 6px; text-align: center; font-weight: bold; border: none; }"

        "QProgressBar::chunk { background-color: #cba6f7; border-radius: 6px; }";

    btnRefresh->setObjectName("btnRefresh");

    setStyleSheet(darkTheme);
}

bool MainWindow::getSelectedPid(pid_t &pid)
{
    int row = processTable->currentRow();

    if (row < 0)
    {
        QMessageBox::warning(
            this,
            "No Process Selected",
            "Please select a process from the table first."
        );

        return false;
    }

    QTableWidgetItem *item = processTable->item(row, 0);

    if (!item)
        return false;

    bool ok;
    long long value = item->text().toLongLong(&ok);

    if (!ok || value <= 0)
        return false;

    pid = static_cast<pid_t>(value);

    return true;
}

// READ AND UPDATE PROCESSES
void MainWindow::refreshProcesses()
{
    // Remember the currently selected PID
    QString selectedPid;

    int currentRow = processTable->currentRow();

    if (currentRow >= 0 &&
        processTable->item(currentRow, 0))
    {
        selectedPid =
            processTable->item(currentRow, 0)->text();
    }

    // Refresh backend data
    allProcesses = processReader.readAll();

    // Update CPU
    double cpuUsage = cpuMonitor.getUsagePercent();

    cpuBar->setValue(
        qBound(0, static_cast<int>(cpuUsage), 100)
    );

    // Update memory
    MemInfo memory = readMemory();

    memBar->setValue(
        qBound(0, static_cast<int>(memory.usedPercent), 100)
    );

    // Rebuild the table
    filterProcesses(searchBox->text());

    // Restore selection using the PID
    if (!selectedPid.isEmpty())
    {
        for (int row = 0;
             row < processTable->rowCount();
             row++)
        {
            QTableWidgetItem *item =
                processTable->item(row, 0);

            if (item && item->text() == selectedPid)
            {
                processTable->selectRow(row);
                break;
            }
        }
    }
}

// SEARCH PROCESSES
void MainWindow::filterProcesses(const QString &text)
{
    std::vector<ProcessInfo> filteredProcesses =
        searchProcesses(allProcesses, text.toStdString());

    populateProcessTable(filteredProcesses);
}


// FILL PROCESS TABLE
void MainWindow::populateProcessTable(
    const std::vector<ProcessInfo> &processes
)
{
    // Temporarily disable sorting while inserting rows
    processTable->setSortingEnabled(false);

    processTable->setRowCount(0);

    MemInfo memory = readMemory();

    for (const ProcessInfo &process : processes)
    {
        int row = processTable->rowCount();

        processTable->insertRow(row);

        // Calculate process memory as percentage of total RAM
        double memoryPercent = 0.0;

        if (memory.totalKB > 0)
        {
            memoryPercent =
                (static_cast<double>(process.memoryKB) /
                 static_cast<double>(memory.totalKB)) * 100.0;
        }

        // PID
        QTableWidgetItem *pidItem =
            new QTableWidgetItem();

        pidItem->setData(Qt::DisplayRole, process.pid);

        // App Name
        QTableWidgetItem *nameItem =
            new QTableWidgetItem(
                QString::fromStdString(process.name)
            );

        // State
        QTableWidgetItem *stateItem =
            new QTableWidgetItem(
                QString::fromStdString(process.stateText)
            );

        // CPU
        QTableWidgetItem *cpuItem =
            new QTableWidgetItem();

        cpuItem->setData(
            Qt::DisplayRole,
            process.cpuPercent
        );

        // Memory
        QTableWidgetItem *memoryItem =
            new QTableWidgetItem();

        memoryItem->setData(
            Qt::DisplayRole,
            memoryPercent
        );

        processTable->setItem(row, 0, pidItem);
        processTable->setItem(row, 1, nameItem);
        processTable->setItem(row, 2, stateItem);
        processTable->setItem(row, 3, cpuItem);
        processTable->setItem(row, 4, memoryItem);
    }

    // Enable sorting again
    processTable->setSortingEnabled(true);
}


void MainWindow::pauseProcess()
{
    pid_t pid;

    if (!getSelectedPid(pid))
        return;

    SignalResult result = ::pauseProcess(pid);

    QMessageBox::information(
        this,
        "Pause Process",
        QString::fromStdString(resultToString(result))
    );

    refreshProcesses();
}


void MainWindow::resumeProcess()
{
    pid_t pid;

    if (!getSelectedPid(pid))
        return;

    SignalResult result = ::resumeProcess(pid);

    QMessageBox::information(
        this,
        "Resume Process",
        QString::fromStdString(resultToString(result))
    );

    refreshProcesses();
}


void MainWindow::terminateProcess()
{
    pid_t pid;

    if (!getSelectedPid(pid))
        return;

    QMessageBox::StandardButton reply =
        QMessageBox::question(
            this,
            "Terminate Process",
            "Are you sure you want to terminate this process?"
        );

    if (reply != QMessageBox::Yes)
        return;

    SignalResult result = ::terminateProcess(pid);

    QMessageBox::information(
        this,
        "Terminate Process",
        QString::fromStdString(resultToString(result))
    );

    refreshProcesses();
}


void MainWindow::killProcess()
{
    pid_t pid;

    if (!getSelectedPid(pid))
        return;

    QMessageBox::StandardButton reply =
        QMessageBox::warning(
            this,
            "Kill Process",
            "This will forcefully terminate the selected process. Continue?",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

    if (reply != QMessageBox::Yes)
        return;

    SignalResult result = ::killProcess(pid);

    QMessageBox::information(
        this,
        "Kill Process",
        QString::fromStdString(resultToString(result))
    );

    refreshProcesses();
}
