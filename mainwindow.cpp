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

    connect(ui->gameList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(launchGame()));
    connect(ui->gameList, SIGNAL(itemSelectionChanged()), this, SLOT(refreshGameInfo()));
    connect(ui->launch, SIGNAL(clicked()), this, SLOT(launchGame()));
    connect(ui->uninstall, SIGNAL(clicked()), this, SLOT(uninstallGame()));

    connect(ui->aboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    connect(ui->aboutGOGLauncher, SIGNAL(triggered()), this, SLOT(aboutGOGLauncher()));

    this->refreshGameList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::aboutGOGLauncher()
{
    QMessageBox::about(this, QString("About GOGLauncher"), QString("GOGLauncher is an open source software under the GNU-GPLv3.\nThis is an independant project.\nFeel free to contribute at https://gitlab.com/Aztorius/GOGLauncher"));
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

    QFile gameinfo(QDir::homePath() + QString("/GOG Games/") + gameName + QString("/gameinfo"));

    if (!gameinfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->version->setText(QString());
        return;
    }

    gameinfo.readLine();

    QString version = gameinfo.readLine();
    version.append(gameinfo.readLine());

    ui->version->setText(version);

    gameinfo.close();
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
