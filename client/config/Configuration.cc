#include "Configuration.h"

namespace config {

Configuration::Configuration() : QSS::Configuration() {}

Configuration::Configuration(QJsonObject &json) {
  fromJson(json);
}

void Configuration::fromJson(QJsonObject &json) {
  if (!json.isEmpty()) {
    auto _servers = json["configs"].toArray();
    m_servers = Server::fromJson<QSS::Server>(_servers);
    auto _proxy = json["proxy"].toObject();
    ((Proxy &)m_proxy).fromJson(_proxy);
    m_shareOverLan = json["shareOverLan"].toBool();
  } else {
    *this = Configuration();
  }
}

QJsonObject Configuration::toJson() {
  QJsonObject json;
  // TODO
  return json;
}

Configuration::~Configuration() {
  QSS::Configuration::unregisterChooser();
}

}

#include "types/configuration.h"
#include "types/server.h"

using QSS::IChooser;
using QSS::Server;

class SingleChooser : public IChooser {
  Server getServer() override {
    return Server();
  }
};
