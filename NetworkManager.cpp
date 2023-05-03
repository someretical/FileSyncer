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

#include <QApplication>
#include <QDesktopServices>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <random>
#include <botan_all.h>

#include "NetworkManager.h"

NetworkManager::NetworkManager() {
    m_networkManager = new QNetworkAccessManager();

    m_OAuth2StateToken = QByteArray(32, '\0');
    m_replyServer = new QTcpServer();
}

NetworkManager::~NetworkManager() {
    clearData();

    delete m_networkManager;
    m_replyServer->close();
    delete m_replyServer;
}

// Getters

NetworkManager &NetworkManager::instance() {
    static NetworkManager i;
    return i;
}

QNetworkAccessManager *NetworkManager::m() const {
    return m_networkManager;
}

const QString &NetworkManager::getClientID() const {
    return m_clientID;
}

const QString &NetworkManager::getClientSecret() const {
    return m_clientSecret;
}

const QString &NetworkManager::getScope() const {
    return m_scopes;
}

qint16 NetworkManager::getLocalPort() const {
    return m_localPort;
}

const QByteArray &NetworkManager::getOAuth2StateToken() const {
    return m_OAuth2StateToken;
}

const QString &NetworkManager::getAccessToken() const {
    return m_accessToken;
}

const QString &NetworkManager::getRefreshToken() const {
    return m_refreshToken;
}

const QString &NetworkManager::getID() const {
    return m_id;
}

NetworkManager::OAuth2State NetworkManager::getOAuth2State() const {
    return m_OAuth2State;
}

// Setters

void NetworkManager::setClientId(const QString &mClientId) {
    m_clientID = mClientId;
}

void NetworkManager::setClientSecret(const QString &mClientSecret) {
    m_clientSecret = mClientSecret;
}

void NetworkManager::setScopes(const QString &mScope) {
    m_scopes = mScope;
}

void NetworkManager::setLocalPort(qint16 mLocalPort) {
    m_localPort = mLocalPort;
}

void NetworkManager::setOAuth2State(const QByteArray &mOAuth2State) {
    m_OAuth2StateToken = mOAuth2State;
}

void NetworkManager::setAccessToken(const QString &mAccessToken) {
    m_accessToken = mAccessToken;
}

void NetworkManager::setRefreshToken(const QString &mRefreshToken) {
    m_refreshToken = mRefreshToken;
}

void NetworkManager::setID(const QString &mId) {
    m_id = mId;
}

// Other functions

void NetworkManager::clearData() {
    Botan::secure_scrub_memory(m_accessToken.data(), m_accessToken.size());
    Botan::secure_scrub_memory(m_refreshToken.data(), m_refreshToken.size());
    Botan::secure_scrub_memory(m_id.data(), m_id.size());
    Botan::secure_scrub_memory(m_OAuth2StateToken.data(), m_OAuth2StateToken.size());
}

void NetworkManager::link() {
    qInfo() << "Starting OAuth2 flow";
    // TODO
    // Check if previous credentials are present
    // If not, start OAuth2 flow
    // If yes, check if they are still valid
    // If not, start OAuth2 flow
    // Account for refresh token as well

    OAuth2FlowRequestAuth();
}

void NetworkManager::OAuth2FlowRequestAuth() {
    const QString redirectURI = QString("http://127.0.0.1:%1/").arg(QString::number(m_localPort));

    QUrlQuery query;
    query.addQueryItem("client_id", m_clientID);
    query.addQueryItem("redirect_uri", QUrl::toPercentEncoding(redirectURI));

    query.addQueryItem("response_type", "code");

    // A space-delimited list of scopes that identify the resources that your application could access on the user's behalf. These values inform the consent screen that Google displays to the user.
    query.addQueryItem("scope", m_scopes);

    // Indicates whether your application can refresh access tokens when the user is not present at the browser. Valid parameter values are online, which is the default value, and offline.
    query.addQueryItem("access_type", "offline");

    qInfo() << "Appending prompt=consent to OAuth2 query";
    query.addQueryItem("prompt", "consent");

    // Copied from qtnetworkauth/src/oauth/qabstractoauth.cpp
    const char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    static std::mt19937 randomEngine(QDateTime::currentDateTime().toMSecsSinceEpoch());
    std::uniform_int_distribution<int> distribution(0, sizeof(characters) - 2);
    for (quint8 i = 0; i < 32; ++i)
        m_OAuth2StateToken[i] = characters[distribution(randomEngine)];

    query.addQueryItem("state", m_OAuth2StateToken);

    QUrl url(m_authorizationEndpoint);
    url.setQuery(query);

    m_replyServer = new QTcpServer();
    if (!m_replyServer->listen(QHostAddress::LocalHost, m_localPort)) {
        qInfo() << "OAuth2 TCP server failed to listen to port" << m_localPort;
        emit OAuth2FlowInterrupted(OAuth2FlowError::FailedToStartServer);
        return;
    };

    connect(m_replyServer, &QTcpServer::newConnection, this, &NetworkManager::handleTCPConnection);

    qInfo() << "Opening URL:" << url;
    QDesktopServices::openUrl(url);
}

void NetworkManager::handleTCPConnection() {
    QTcpSocket *socket = m_replyServer->nextPendingConnection();

    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    connect(socket, &QTcpSocket::readyRead, [this, socket] {
        handleTCPSocket(socket);
    });
}

void NetworkManager::handleTCPSocket(QTcpSocket *socket) {
    if (!m_replyServer->isListening()) {
        return;
    }

    // Sample socket data
    // GET /?state=dT3MEzKkMqCdY4l2VDbEerJ6LTSvyHPO&code=4%2F0AbUR2VOZ7NIp4ch7CL0A4sbuOeVwlccZEk63ID0ZzTUhxWu4v8iVCh7W_TQGmV7i4nDKCw&scope=email+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fuserinfo.email+openid&authuser=0&prompt=consent HTTP/1.1
    // Host: 127.0.0.1:8888
    // User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/112.0
    // Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8
    // Accept-Language: en-GB,en;q=0.5
    // Accept-Encoding: gzip, deflate, br
    // DNT: 1
    // Connection: keep-alive
    // Upgrade-Insecure-Requests: 1
    // Sec-Fetch-Dest: document
    // Sec-Fetch-Mode: navigate
    // Sec-Fetch-Site: cross-site
    // Sec-Fetch-User: ?1

    // We are only interested in the first line!

    QByteArray data = socket->readAll();

    auto lines = data.split('\n');
    auto tokens = lines[0].split(' ');

    if (tokens.size() != 3) {
        socket->disconnectFromHost();
        return;
    }

    const auto query = QUrlQuery(QUrl(tokens[1]).query());

    // Verify state first!
    // It does not need to be decoded as it is only alphanumeric
    const auto state = query.queryItemValue("state");

    if (state != m_OAuth2StateToken) {
        qInfo() << "State mismatch:" << m_OAuth2StateToken << "!= received" << state;
        socket->disconnectFromHost();
        return;
    }

    // We check permissions after the receiving the access and refresh tokens

    const auto code = query.queryItemValue("code");
    if (code.isEmpty()) {
        socket->disconnectFromHost();
        qInfo() << "Received empty OAuth2 code";
        OAuth2FlowInterrupted(OAuth2FlowError::ReceivedEmptyCode);
        return;
    }

    const QByteArray html =
            QByteArrayLiteral("<html><head><title>") +
            QApplication::applicationName().toUtf8() +
            QByteArrayLiteral("</title></head><body>"
                              "Callback received. Feel free to close this tab and return to the application."
                              "</body></html>");
    const QByteArray htmlSize = QByteArray::number(html.size());
    const QByteArray replyMessage =
            QByteArrayLiteral("HTTP/1.0 200 OK \r\n"
                              "Content-Type: text/html; "
                              "charset=\"utf-8\"\r\n"
                              "Content-Length: ") + htmlSize +
            QByteArrayLiteral("\r\n\r\n") +
            html;

    socket->write(replyMessage);
    socket->disconnectFromHost();
    m_replyServer->close();

    qInfo() << "Sent 200 OK to port" << m_localPort << "and closed the reply server";

    OAuth2FlowHandleResponse(code.toUtf8());
}

void NetworkManager::OAuth2FlowHandleResponse(const QString &code) {
    QUrlQuery query;
    query.addQueryItem("client_id", m_clientID);
    query.addQueryItem("client_secret", m_clientSecret);
    query.addQueryItem("code", code);
    query.addQueryItem("grant_type", "authorization_code");
    query.addQueryItem("redirect_uri",
                       QUrl::toPercentEncoding(QStringLiteral("http://127.0.0.1:%1/").arg(m_localPort)));

    const auto data = query.toString().toUtf8();

    QNetworkRequest tokenRequest(m_tokenEndpoint);
    tokenRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    const auto reply = m_networkManager->post(tokenRequest, data);
    connect(reply, &QNetworkReply::finished, [this, reply] {
        OAuth2ParseTokens(QNetworkReply::NoError, reply);
    });
    connect(reply, &QNetworkReply::errorOccurred, [this, reply](QNetworkReply::NetworkError status) {
        OAuth2ParseTokens(status, reply);
    });
}

void NetworkManager::OAuth2ParseTokens(QNetworkReply::NetworkError status, QNetworkReply *reply) {
    if (status != QNetworkReply::NoError) {
        qInfo() << "Error occurred while requesting OAuth2 tokens";
        qInfo() << NETWORKERROR_NAME_MAP.value(status) << NETWORKERROR_DESCRIPTION_MAP.value(status);
        qInfo() << reply->readAll();
        OAuth2FlowInterrupted(OAuth2FlowError::TokenRequestError);
        return;
    }

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &error);

    if (document.isNull()) {
        qInfo() << "Error occurred while parsing token request JSON response:" << error.errorString();
        OAuth2FlowInterrupted(OAuth2FlowError::TokenParseError);
        return;
    }

    qDebug() << document;

//    // Check we got the same permissions as we wanted
//    const auto scope = query.queryItemValue("scope");
//    const auto tempScope = QUrl::fromPercentEncoding(scope.toUtf8());
//    const auto scopes = tempScope.split('+');
//    const auto requestedScopes = m_scopes.split(' ');
//
//    const auto difference = QSet(scopes.begin(), scopes.end()).subtract(
//            QSet(requestedScopes.begin(), requestedScopes.end())).values();
//
//    // Ha
//    if (!difference.isEmpty()) {
//        // TODO Handle error
//        qInfo() << "Scope mismatch" << difference;
////        socket->disconnectFromHost();
////        OAuth2FlowInterrupted();
////        return;
//    }


    // Verify JWT

    reply->deleteLater();
}

// On how to cancel the whole OAuth2 flow in the middle of things
// At every stage check the OAuth2 cancel flag and if it is cancelled, terminate the flow then
// reset variables as required

void NetworkManager::OAuth2FlowGranted() {
    // Fetch details about user
    m_OAuth2State = OAuth2State::Granted;

    qInfo() << "OAuth2 flow granted";
    emit sigOAuth2FlowGranted();
    OAuth2FlowFinished();
}

// This function is called regardless of whether the user grants or denies access or presses the cancel button.
void NetworkManager::OAuth2FlowFinished() {
    m_replyServer->close();

    qInfo() << "OAuth2 flow finished";
    emit sigOAuth2FlowFinished();
}

void NetworkManager::OAuth2FlowInterrupted(enum OAuth2FlowError error) {
    clearData();
    m_OAuth2State = OAuth2State::NotAuthenticated;

    qInfo() << "OAuth2 flow interrupted";
    emit sigOAuth2FlowInterrupted(error);
    OAuth2FlowFinished();
}

