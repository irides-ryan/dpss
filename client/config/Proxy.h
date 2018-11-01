#pragma once

#include <QtCore/QJsonObject>
#include "JConfig.h"

namespace config {

  class Proxy : JConfig {

  public:
    enum {
      SOCKS5, HTTP
    };

    bool use{};
    int type{};
    QString server;
    int port{};
    int timeout{};

    Proxy();
    explicit Proxy(QJsonObject &json);

    void fromJson(QJsonObject &json) override;
    QJsonObject toJson() override;
  };

}
