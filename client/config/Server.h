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
  explicit Server(const QSX::Server &server);
  explicit Server(const QJsonObject &json);
  ~Server() override = default;

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

  void fromJson(const QJsonObject &json) override;

  QJsonObject toJson() override;

  QString toUri();

  bool fromUri(const QString &uri);

  template<typename T = QSX::Server>
  static QJsonArray ToJson(const QList<T> &list) {
    QJsonArray array;
    for (auto &s : list) {
      array.append(Server(s).toJson());
    }
    return array;
  }

  template<typename T = QSX::Server>
  static QList<T> FromJson(const QJsonArray &array) {
    QList<T> list;
    for (auto j : array) {
      auto json = j.toObject();
      list.append(Server(json));
    }
    return list;
  }
};

}
