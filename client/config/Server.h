#pragma once

#include <QtCore/QCryptographicHash>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QRegExp>
#include "JConfig.h"

namespace config {

  class Server : public JConfig {

  public:
    QString server;
    QString passwd;
    QString method;
    QString remarks;
    uint16_t server_port{};
    uint16_t timeout{};

    Server();
    explicit Server(QJsonObject &json);

    uint64_t hash();
    void fromJson(QJsonObject &json) override;
    QJsonObject toJson() override;
    QString toUri();
    bool fromUri(const QString &uri);

    static QJsonArray toJson(QList<Server> &list);
    static QList<Server> fromJson(QJsonArray &array);
  };

}
