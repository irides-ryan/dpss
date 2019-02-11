#pragma once

#include <QtCore/QJsonObject>
#include <types/proxy.h>
#include "JConfig.h"

namespace config {

class Proxy : JConfig, public QSX::Proxy {

public:
  Proxy();
  explicit Proxy(const QSX::Proxy &p);
  explicit Proxy(const QJsonObject &json);
  ~Proxy() override = default;

  Proxy &operator=(Proxy const &proxy) {
    use = proxy.use;
    type = proxy.type;
    server = proxy.server;
    port = proxy.port;
    timeout = proxy.timeout;
    return *this;
  }

  void fromJson(const QJsonObject &json) override;

  QJsonObject toJson() override;

  template<typename T = QSX::Proxy>
  static QJsonObject ToJson(T &t) {
    return Proxy(t).toJson();
  }

  template<typename T = QSX::Proxy>
  static T FromJson(const QJsonObject &json) {
    Proxy p(json);
    return p;
  }
};

}
