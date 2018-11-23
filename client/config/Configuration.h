#pragma once

#include "JConfig.h"
#include "Server.h"
#include "Proxy.h"
#include "types/configuration.h"

// declare in global
class GConfig;

namespace config {

class Configuration : JConfig, public QSX::Configuration {

private:
  int m_index = 0;
  bool m_enable = false;
  bool m_global = false;
  bool m_autoStart = false;
  bool m_autoCheckUpdate = false;

  // using global namespace
  friend class ::GConfig;

public:
  Configuration();
  explicit Configuration(const QJsonObject &json);
  ~Configuration() override;

  void fromJson(const QJsonObject &json) override;
  QJsonObject toJson() override;
};

}
