#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrlQuery>
#include "App.h"
#include "OAuth.h"

const QString DROPBOX_APP_KEY = "wmae5esncijnqie";
const QString DROPBOX_APP_SECRET = "3fp3vak8ijd1l1e";

OAuth::OAuth(App& app) : app_(app), ui(new Ui::OAuth) {
    ui->setupUi(this);

    connect(&manager_, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(tokenReply(QNetworkReply*)));
}

OAuth::~OAuth() {
    delete ui;
}

void OAuth::setService(UploadService service) {
    service_ = service;
    QString text("1. Откройте в браузере <a href=\"{LINK}\">ссылку</a>.");

    switch (service) {
        case UploadService::DROPBOX:
            text.replace("{LINK}", "https://www.dropbox.com/1/oauth2/authorize?response_type=code&client_id=" + DROPBOX_APP_KEY);
            break;
    }

    ui->link->setText(text.replace("{LINK}", text));
}

void OAuth::accept() {
    ui->submitButtons->setEnabled(false);

    QNetworkRequest request(QUrl("https://api.dropboxapi.com/1/oauth2/token"));
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    QUrlQuery query;
    query.addQueryItem("code", ui->code->text());
    query.addQueryItem("grant_type", "authorization_code");
    query.addQueryItem("client_id", DROPBOX_APP_KEY);
    query.addQueryItem("client_secret", DROPBOX_APP_SECRET);

    manager_.post(request, query.toString().toLatin1());
}

void OAuth::tokenReply(QNetworkReply* reply) {
    ui->submitButtons->setEnabled(true);

    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
    reply->close();

    QJsonObject jsonObject = jsonResponse.object();

    if (QNetworkReply::NoError != reply->error()) {
        app_.trayIcon().showError("Ошибка", "Неверный код");
    } else {
        hide();

        QString token = jsonObject["access_token"].toString();
        app_.settings().setDropboxToken(token);
    }
}
