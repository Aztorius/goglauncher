#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "version.h"

#include <QDir>
#include <QProcess>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("GOGLauncher " + VERSION);

    connect(ui->gameList, SIGNAL(itemSelectionChanged()), this, SLOT(refreshGameInfo()));
    connect(ui->gameList, SIGNAL(doubleClicked()), this, SLOT(launchGame()));
    connect(ui->launch, SIGNAL(clicked()), this, SLOT(launchGame()));
    connect(ui->uninstall, SIGNAL(clicked()), this, SLOT(uninstallGame()));

    this->refreshGameList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshGameList()
{
    QDir dir(QDir::homePath() + "/GOG Games/");

    QFileInfoList subdirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    ui->gameList->clear();

    foreach (QFileInfo subdir, subdirs) {
        ui->gameList->addItem(subdir.fileName());
        QIcon icon;
        icon.addFile(QDir::homePath() + "/GOG Games/" + subdir.fileName() + "/support/icon.png");
        QListWidgetItem *item = ui->gameList->item(ui->gameList->count() - 1);
        item->setIcon(icon);
    }
}

void MainWindow::refreshGameInfo()
{
    QString gameName = ui->gameList->currentItem()->text();

    QPixmap pix;
    pix.load(QDir::homePath() + QString("/GOG Games/") + gameName + QString("/support/icon.png"));

    ui->label_gameIcon->setPixmap(pix);
}

void MainWindow::launchGame()
{
    setWindowState(Qt::WindowMinimized);

    QString gameName = ui->gameList->currentItem()->text();

    QProcess *process = new QProcess(this);
    connect(process, SIGNAL(finished(int)), this, SLOT(gameClosed(int)));
    process->setWorkingDirectory(QString(QDir::homePath() + "/GOG Games/") + gameName);
    process->start(QString("bash"), QStringList() << QDir::homePath() + QString("/GOG Games/") + gameName + QString("/start.sh"));
}

void MainWindow::uninstallGame()
{
    QString gameName = ui->gameList->currentItem()->text();

    QProcess *process = new QProcess(this);
    process->setWorkingDirectory(QString(QDir::homePath() + "/GOG Games/") + gameName);
    process->start(QString("bash"), QStringList() << QDir::homePath() + QString("/GOG Games/") + gameName + QString("/uninstall-") + gameName + QString(".sh"));
}

void MainWindow::gameClosed(int exitCode)
{
    setWindowState(Qt::WindowNoState);

    if (exitCode) {
        QMessageBox::information(this, QString("Game closed"), QString("The game exited with an error code ") + QString::number(exitCode));
    }
}
