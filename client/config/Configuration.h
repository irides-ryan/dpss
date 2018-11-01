#pragma once

#include "JConfig.h"
#include "Server.h"
#include "Proxy.h"

namespace config {

  class Configuration : JConfig {

  public:
    QList<Server> servers;
    Proxy proxy;

    Configuration();
    explicit Configuration(QJsonObject &json);

    void fromJson(QJsonObject &json) override;
    QJsonObject toJson() override;
  };

}
