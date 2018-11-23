#pragma once

#include <QObject>

namespace config {

  class JConfig {

  public:
    virtual QJsonObject toJson() = 0;
    virtual void fromJson(const QJsonObject &json) = 0;

    virtual ~JConfig() = default;
  };

}
