#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void refreshGameList();

private slots:
    void refreshGameInfo();
    void launchGame();
    void uninstallGame();
    void gameClosed(int);
};

#endif // MAINWINDOW_H
