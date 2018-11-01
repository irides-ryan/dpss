#include "Configuration.h"

namespace config {

Configuration::Configuration()
  : servers(QList<Server>()),
    proxy(Proxy())
{}

Configuration::Configuration(QJsonObject &json) {
  fromJson(json);
}

void Configuration::fromJson(QJsonObject &json) {
  if (!json.isEmpty()) {
    auto _servers = json["configs"].toArray();
    servers = Server::fromJson(_servers);
    auto _proxy = json["proxy"].toObject();
    proxy.fromJson(_proxy);
  } else {
    *this = Configuration();
  }
}

QJsonObject Configuration::toJson() {
  QJsonObject json;
  // TODO
  return json;
}

}
