#pragma once

#include <QtCore/QJsonObject>
#include <types/proxy.h>
#include "JConfig.h"

namespace config {

class Proxy : JConfig, public QSX::Proxy {

public:
  Proxy();

  explicit Proxy(QJsonObject &json);

  Proxy &operator=(Proxy const &proxy) {
    use = proxy.use;
    type = proxy.type;
    server = proxy.server;
    port = proxy.port;
    timeout = proxy.timeout;
    return *this;
  }

  void fromJson(QJsonObject &json) override;

  QJsonObject toJson() override;
};

}
