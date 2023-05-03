/*
 * This file is part of FileSyncer
 * Copyright (C) 2023 someretical
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
