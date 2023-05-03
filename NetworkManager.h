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

#include <QNetworkAccessManager>
#include <QTcpServer>
#include <QNetworkReply>
#include <QJsonDocument>

#ifndef FILESYNCER_NETWORKMANAGER_H
#define FILESYNCER_NETWORKMANAGER_H

QMap<QNetworkReply::NetworkError, QString> const NETWORKERROR_DESCRIPTION_MAP = {
        {QNetworkReply::ConnectionRefusedError,            "the remote server refused the connection (the server is not accepting requests)"},
        {QNetworkReply::RemoteHostClosedError,             "the remote server closed the connection prematurely, before the entire reply was received and processed"},
        {QNetworkReply::HostNotFoundError,                 "the remote host name was not found (invalid hostname)"},
        {QNetworkReply::TimeoutError,                      "the connection to the remote server timed out"},
        {QNetworkReply::OperationCanceledError,            "the operation was canceled via calls to abort() or close() before it was finished."},
        {QNetworkReply::SslHandshakeFailedError,           "the SSL/TLS handshake failed and the encrypted channel could not be established. The sslErrors() signal should have been emitted."},
        {QNetworkReply::TemporaryNetworkFailureError,      "the connection was broken due to disconnection from the network, however the system has initiated roaming to another access point. The request should be resubmitted and will be processed as soon as the connection is re-established."},
        {QNetworkReply::NetworkSessionFailedError,         "the connection was broken due to disconnection from the network or failure to start the network."},
        {QNetworkReply::BackgroundRequestNotAllowedError,  "the background request is not currently allowed due to platform policy."},
        {QNetworkReply::TooManyRedirectsError,             "while following redirects, the maximum limit was reached. The limit is by default set to 50 or as set by QNetworkRequest::setMaxRedirectsAllowed(). (This value was introduced in 5.6.)"},
        {QNetworkReply::InsecureRedirectError,             "while following redirects, the network access API detected a redirect from a encrypted protocol (https) to an unencrypted one (http). (This value was introduced in 5.6.)"},
        {QNetworkReply::ProxyConnectionRefusedError,       "the connection to the proxy server was refused (the proxy server is not accepting requests)"},
        {QNetworkReply::ProxyConnectionClosedError,        "the proxy server closed the connection prematurely, before the entire reply was received and processed"},
        {QNetworkReply::ProxyNotFoundError,                "the proxy host name was not found (invalid proxy hostname)"},
        {QNetworkReply::ProxyTimeoutError,                 "the connection to the proxy timed out or the proxy did not reply in time to the request sent"},
        {QNetworkReply::ProxyAuthenticationRequiredError,  "the proxy requires authentication in order to honour the request but did not accept any credentials offered (if any)"},
        {QNetworkReply::ContentAccessDenied,               "the access to the remote content was denied (similar to HTTP error 403)"},
        {QNetworkReply::ContentOperationNotPermittedError, "the operation requested on the remote content is not permitted"},
        {QNetworkReply::ContentNotFoundError,              "the remote content was not found at the server (similar to HTTP error 404)"},
        {QNetworkReply::AuthenticationRequiredError,       "the remote server requires authentication to serve the content but the credentials provided were not accepted (if any)"},
        {QNetworkReply::ContentReSendError,                "the request needed to be sent again, but this failed for example because the upload data could not be read a second time."},
        {QNetworkReply::ContentConflictError,              "the request could not be completed due to a conflict with the current state of the resource."},
        {QNetworkReply::ContentGoneError,                  "the requested resource is no longer available at the server."},
        {QNetworkReply::InternalServerError,               "the server encountered an unexpected condition which prevented it from fulfilling the request."},
        {QNetworkReply::OperationNotImplementedError,      "the server does not support the functionality required to fulfill the request."},
        {QNetworkReply::ServiceUnavailableError,           "the server is unable to handle the request at this time."},
        {QNetworkReply::ProtocolUnknownError,              "the Network Access API cannot honor the request because the protocol is not known"},
        {QNetworkReply::ProtocolInvalidOperationError,     "the requested operation is invalid for this protocol"},
        {QNetworkReply::UnknownNetworkError,               "an unknown network-related error was detected"},
        {QNetworkReply::UnknownProxyError,                 "an unknown proxy-related error was detected"},
        {QNetworkReply::UnknownContentError,               "an unknown error related to the remote content was detected"},
        {QNetworkReply::ProtocolFailure,                   "a breakdown in protocol was detected (parsing error, invalid or unexpected responses, etc.)"},
        {QNetworkReply::UnknownServerError,                "an unknown error related to the server response was detected"}
};

QMap<QNetworkReply::NetworkError, QString> const NETWORKERROR_NAME_MAP = {
        {QNetworkReply::ConnectionRefusedError,            "ConnectionRefusedError"},
        {QNetworkReply::RemoteHostClosedError,             "RemoteHostClosedError"},
        {QNetworkReply::HostNotFoundError,                 "HostNotFoundError"},
        {QNetworkReply::TimeoutError,                      "TimeoutError"},
        {QNetworkReply::OperationCanceledError,            "OperationCanceledError"},
        {QNetworkReply::SslHandshakeFailedError,           "SslHandshakeFailedError"},
        {QNetworkReply::TemporaryNetworkFailureError,      "TemporaryNetworkFailureError"},
        {QNetworkReply::NetworkSessionFailedError,         "NetworkSessionFailedError"},
        {QNetworkReply::BackgroundRequestNotAllowedError,  "BackgroundRequestNotAllowedError"},
        {QNetworkReply::TooManyRedirectsError,             "TooManyRedirectsError"},
        {QNetworkReply::InsecureRedirectError,             "InsecureRedirectError"},
        {QNetworkReply::ProxyConnectionRefusedError,       "ProxyConnectionRefusedError"},
        {QNetworkReply::ProxyConnectionClosedError,        "ProxyConnectionClosedError"},
        {QNetworkReply::ProxyNotFoundError,                "ProxyNotFoundError"},
        {QNetworkReply::ProxyTimeoutError,                 "ProxyTimeoutError"},
        {QNetworkReply::ProxyAuthenticationRequiredError,  "ProxyAuthenticationRequiredError"},
        {QNetworkReply::ContentAccessDenied,               "ContentAccessDenied"},
        {QNetworkReply::ContentOperationNotPermittedError, "ContentOperationNotPermittedError"},
        {QNetworkReply::ContentNotFoundError,              "ContentNotFoundError"},
        {QNetworkReply::AuthenticationRequiredError,       "AuthenticationRequiredError"},
        {QNetworkReply::ContentReSendError,                "ContentReSendError"},
        {QNetworkReply::ContentConflictError,              "ContentConflictError"},
        {QNetworkReply::ContentGoneError,                  "ContentGoneError"},
        {QNetworkReply::InternalServerError,               "InternalServerError"},
        {QNetworkReply::OperationNotImplementedError,      "OperationNotImplementedError"},
        {QNetworkReply::ServiceUnavailableError,           "ServiceUnavailableError"},
        {QNetworkReply::ProtocolUnknownError,              "ProtocolUnknownError"},
        {QNetworkReply::ProtocolInvalidOperationError,     "ProtocolInvalidOperationError"},
        {QNetworkReply::UnknownNetworkError,               "UnknownNetworkError"},
        {QNetworkReply::UnknownProxyError,                 "UnknownProxyError"},
        {QNetworkReply::UnknownContentError,               "UnknownContentError"},
        {QNetworkReply::ProtocolFailure,                   "ProtocolFailure"},
        {QNetworkReply::UnknownServerError,                "UnknownServerError"}
};

class NetworkManager : public QObject {
Q_OBJECT

public:
    enum class OAuth2State {
        NotAuthenticated,
        Granted,
        RefreshingToken,
    };

    enum class OAuth2FlowError {
        None,
        FailedToStartServer,
        ReceivedEmptyCode,
        TokenRequestError,
        TokenParseError,
    };

signals:

    void sigOAuth2FlowInterrupted(OAuth2FlowError error);

    void sigOAuth2FlowGranted();

    // Finished is always called regardless of error or granted
    void sigOAuth2FlowFinished();

public:
    NetworkManager(NetworkManager const &) = delete;

    void operator=(NetworkManager const &) = delete;

    void clearData();

    /// Start of OAuth2Flow
    void link();

    void OAuth2FlowRequestAuth();

    void handleTCPConnection();

    void handleTCPSocket(QTcpSocket *socket);

    void OAuth2FlowHandleResponse(const QString &code);

    void OAuth2ParseTokens(QNetworkReply::NetworkError status, QNetworkReply *reply);

    void OAuth2FlowGranted();

    void OAuth2FlowFinished();

    // This is a catch-all for any errors that may occur during the OAuth2 flow
    void OAuth2FlowInterrupted(OAuth2FlowError error);
    /// End of OAuth2Flow

private:
    NetworkManager();

    ~NetworkManager() override;

    QNetworkAccessManager *m_networkManager;
    QTcpServer *m_replyServer;

    QString m_clientID = QStringLiteral("401770500943-o1j9579dp6bva9fmeh1prks3vmucc4cj.apps.googleusercontent.com");
    QString m_clientSecret = QStringLiteral("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");
    QUrl m_authorizationEndpoint = QUrl("https://accounts.google.com/o/oauth2/v2/auth");
    QUrl m_tokenEndpoint = QUrl("https://oauth2.googleapis.com/token");
    QString m_scopes = QStringLiteral("https://www.googleapis.com/auth/userinfo.email");
    qint16 m_localPort = 8888;

    QByteArray m_OAuth2StateToken;
    QString m_accessToken;
    QString m_refreshToken;
    QString m_id;
    OAuth2State m_OAuth2State = OAuth2State::NotAuthenticated;

public:
    static NetworkManager &instance();

    [[nodiscard]] QNetworkAccessManager *m() const;

    [[nodiscard]] const QString &getClientID() const;

    [[nodiscard]] const QString &getClientSecret() const;

    [[nodiscard]] const QString &getScope() const;

    [[nodiscard]] qint16 getLocalPort() const;

    [[nodiscard]] const QByteArray &getOAuth2StateToken() const;

    [[nodiscard]] const QString &getAccessToken() const;

    [[nodiscard]] const QString &getRefreshToken() const;

    [[nodiscard]] const QString &getID() const;

    [[nodiscard]] OAuth2State getOAuth2State() const;

    void setClientId(const QString &mClientId);

    void setClientSecret(const QString &mClientSecret);

    void setScopes(const QString &mScope);

    void setLocalPort(qint16 mLocalPort);

    void setOAuth2State(const QByteArray &mOAuth2State);

    void setAccessToken(const QString &mAccessToken);

    void setRefreshToken(const QString &mRefreshToken);

    void setID(const QString &mId);
};


#endif //FILESYNCER_NETWORKMANAGER_H
