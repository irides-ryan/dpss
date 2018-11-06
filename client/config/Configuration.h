#pragma once

#include "JConfig.h"
#include "Server.h"
#include "Proxy.h"
#include "types/configuration.h"

namespace config {

class Configuration : JConfig, public QSS::Configuration {

  public:
    Configuration();
    explicit Configuration(QJsonObject &json);
    ~Configuration() override;

    void fromJson(QJsonObject &json) override;
    QJsonObject toJson() override;
  };

}
