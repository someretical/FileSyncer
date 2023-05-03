//
// Created by someretical on 28/04/2023.
//

#ifndef FILESYNCER_MAINWINDOW_H
#define FILESYNCER_MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    inline static QPlainTextEdit *s_console = nullptr;

public slots:

    void slotUpdateConfigurationsButtonClicked();

    void slotAuthorizeButtonClicked();

    void slotCancelButtonClicked();

    void slotForgetButtonClicked();

    void slotOAuth2FlowFinished();

private:
    Ui::MainWindow *ui;
    QDir working_dir;
};


#endif //FILESYNCER_MAINWINDOW_H
