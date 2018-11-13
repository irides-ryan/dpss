#pragma once

#include <QtCore/QCryptographicHash>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QRegExp>
#include <types/server.h>
#include "JConfig.h"

namespace config {

class Server : JConfig, public QSX::Server {

public:
  Server();

  explicit Server(QJsonObject &json);

  Server &operator=(Server const &s) {
    server = s.server;
    passwd = s.passwd;
    method = s.method;
    remarks = s.remarks;
    server_port = s.server_port;
    timeout = s.timeout;
    return *this;
  }

  uint64_t hash();

  void fromJson(QJsonObject &json) override;

  QJsonObject toJson() override;

  QString toUri();

  bool fromUri(const QString &uri);

  static QJsonArray toJson(QList<Server> &list);

  template<typename T = QSX::Server>
  static QList<T> fromJson(QJsonArray &array) {
    QList<T> list;
    for (auto j : array) {
      auto json = j.toObject();
      list.append(Server(json));
    }
    return list;
  }
};

}
