#include "Configuration.h"

namespace config {

Configuration::Configuration() : QSX::Configuration() {}

Configuration::Configuration(QJsonObject &json) {
  fromJson(json);
}

void Configuration::fromJson(QJsonObject &json) {
  if (!json.isEmpty()) {
    auto _servers = json["configs"].toArray();
    m_servers = Server::fromJson(_servers);
    auto _proxy = json["proxy"].toObject();
    ((Proxy &)m_proxy).fromJson(_proxy);

    m_shareOverLan = json["shareOverLan"].toBool();
    m_localPort = static_cast<uint16_t>(json["localPort"].toInt());

    m_index = json["index"].toInt();
    m_enable = json["enable"].toBool();
    m_global = json["global"].toBool();
    m_autoStart = json["autoStart"].toBool();
    m_autoCheckUpdate = json["autoCheckUpdate"].toBool();
  } else {
    *this = Configuration();
  }
}

QJsonObject Configuration::toJson() {
  QJsonObject json;
  json.insert("configs", ((Server*)&m_servers)->toJson());
  json.insert("proxy", ((Proxy*)&m_proxy)->toJson());

  json.insert("localPort", m_localPort);
  json.insert("shareOverLan", m_shareOverLan);

  json.insert("enable", m_enable);
  json.insert("index", m_index);
  json.insert("global", m_global);
  json.insert("autoStart", m_autoStart);
  json.insert("autoCheckUpdate", m_autoCheckUpdate);
  return json;
}

Configuration::~Configuration() {
  QSX::Configuration::unregisterChooser();
}

}

#include "types/configuration.h"
#include "types/server.h"

using QSX::IChooser;
using QSX::Server;

class SingleChooser : public IChooser {
  Server getServer(QSS::Address &destination) override {
    return Server();
  }
};
