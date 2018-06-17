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

    connect(ui->gameList, SIGNAL(clicked(QModelIndex)), this, SLOT(refreshGameInfo(QModelIndex)));
    connect(ui->gameList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(launchGame(QModelIndex)));

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

void MainWindow::refreshGameInfo(QModelIndex index)
{
    QString gameName = index.data().toString();

    QPixmap pix;
    pix.load(QDir::homePath() + QString("/GOG Games/") + gameName + QString("/support/icon.png"));

    ui->label_gameIcon->setPixmap(pix);
}

void MainWindow::launchGame(QModelIndex index)
{
    QString gameName = index.data().toString();

    QProcess *process = new QProcess(this);
    connect(process, SIGNAL(finished(int)), this, SLOT(gameClosed(int)));
    process->setWorkingDirectory(QString(QDir::homePath() + "/GOG Games/") + gameName);
    process->start(QString("bash"), QStringList() << QDir::homePath() + QString("/GOG Games/") + gameName + QString("/start.sh"));
}

void MainWindow::gameClosed(int exitCode)
{
    if (exitCode) {
        QMessageBox::information(this, QString("Game closed"), QString("The game exited with an error code ") + QString::number(exitCode));
    }
}
