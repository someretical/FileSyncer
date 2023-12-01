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

#include <QDir>

#include <botan_all.h>
#include <array>

#include "KMessageWidget.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "NetworkManager.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    s_console = ui->console;
    ui->console->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    setWindowTitle(QApplication::applicationName());
    ui->messageWidget->hide();

    const auto &nmInstance = NetworkManager::instance();
    ui->lineEditClientID->setText(nmInstance.getClientID());
    ui->lineEditClientSecret->setText(nmInstance.getClientSecret());
    ui->lineEditScopes->setText(nmInstance.getScope());
    ui->spinBoxLocalPort->setValue(nmInstance.getLocalPort());

    connect(ui->pushButtonUpdateConfigurations, &QPushButton::clicked, this,
            &MainWindow::slotUpdateConfigurationsButtonClicked);
    connect(ui->pushButtonAuthorize, &QPushButton::clicked, this, &MainWindow::slotAuthorizeButtonClicked);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &MainWindow::slotCancelButtonClicked);
    connect(ui->pushButtonForget, &QPushButton::clicked, this, &MainWindow::slotForgetButtonClicked);

    connect(&nmInstance, &NetworkManager::sigOAuth2FlowFinished, this, &MainWindow::slotOAuth2FlowFinished);
    
    qInfo() << "Program executable location:" << QDir::currentPath();
    const QString working_dir_str = QDir::current().filePath("working");
    QDir::current().mkdir(working_dir_str);
    working_dir = QDir(working_dir_str);
    qInfo() << "Using working directory:" << working_dir_str;
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::slotUpdateConfigurationsButtonClicked() {
    auto &nmInstance = NetworkManager::instance();

    nmInstance.setClientId(ui->lineEditClientID->text());
    nmInstance.setClientSecret(ui->lineEditClientSecret->text());
    nmInstance.setScopes(ui->lineEditScopes->text());
    nmInstance.setLocalPort(ui->spinBoxLocalPort->value());
    nmInstance.setAccessToken(ui->lineEditAccessToken->text());
    nmInstance.setRefreshToken(ui->lineEditRefreshToken->text());

    ui->messageWidget->setText("Updated configurations");
    ui->messageWidget->setMessageType(KMessageWidget::MessageType::Positive);
    ui->messageWidget->animatedShow();
    qInfo() << "Updated configurations";
}

void MainWindow::slotAuthorizeButtonClicked() {
//    std::array<uint8_t, 32> salt{};
//    Botan::system_rng().randomize(salt);
//
//    const std::string password = "tell no one";
//    std::array<uint8_t, 32> key{};
//
//    auto pwd_fam = Botan::PasswordHashFamily::create_or_throw("Argon2i");
//    auto pwdhash = pwd_fam->from_params(8192, 3, 1);
//    pwdhash->hash(key, password, salt);
//
//    ui->console->appendPlainText(QByteArray(reinterpret_cast<const char *>(key.data()), key.size()).toHex());

    NetworkManager::instance().setClientId(ui->lineEditClientID->text());
    NetworkManager::instance().setClientSecret(ui->lineEditClientSecret->text());
    NetworkManager::instance().setScopes(ui->lineEditScopes->text());
    NetworkManager::instance().setLocalPort(ui->spinBoxLocalPort->value());

    ui->pushButtonCancel->setEnabled(true);
    ui->pushButtonAuthorize->setEnabled(false);
    NetworkManager::instance().link();
}

void MainWindow::slotCancelButtonClicked() {
    ui->console->appendPlainText("Cancelling OAuth2 flow");
    NetworkManager::instance().OAuth2FlowFinished();
    NetworkManager::instance().clearData();
}

void MainWindow::slotForgetButtonClicked() {

}

void MainWindow::slotOAuth2FlowFinished() {
    const auto &nmInstance = NetworkManager::instance();
    auto status = nmInstance.getOAuth2State();

    if (status == NetworkManager::OAuth2State::Granted) {
        ui->pushButtonAuthorize->setEnabled(false);
        ui->pushButtonCancel->setEnabled(false);
        ui->pushButtonForget->setEnabled(true);

        ui->lineEditAccessToken->setText(nmInstance.getAccessToken());
        ui->lineEditRefreshToken->setText(nmInstance.getRefreshToken());
    } else {
        ui->pushButtonAuthorize->setEnabled(true);
        ui->pushButtonCancel->setEnabled(false);
        ui->pushButtonForget->setEnabled(false);

        ui->lineEditAccessToken->setText("");
        ui->lineEditRefreshToken->setText("");
    }
}
