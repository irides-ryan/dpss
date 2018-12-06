#include "Configuration.h"

namespace config {

Configuration::Configuration() : QSX::Configuration() {}

Configuration::Configuration(const QJsonObject &json) {
  fromJson(json);
}

void Configuration::fromJson(const QJsonObject &json) {
  if (!json.isEmpty()) {
    m_servers = Server::fromJson(json["configs"].toArray());
    m_proxy = Proxy::fromJson(json["proxy"].toObject());

    m_shareOverLan = json["shareOverLan"].toBool(false);
    m_localPort = static_cast<uint16_t>(json["localPort"].toInt(1080));

    m_index = json["index"].toInt(0);
    m_enabled = json["enabled"].toBool(false);
    m_global = json["global"].toBool(false);
    m_autoStart = json["autoStart"].toBool(false);
    m_autoCheckUpdate = json["autoCheckUpdate"].toBool(false);
    m_checkPreRelease = json["checkPreRelease"].toBool(false);
    m_isVerboseLogging = json["isVerboseLogging"].toBool(false);

    m_pacUrl = json["pacUrl"].toString("");
    m_useOnlinePac = json["useOnlinePac"].toBool(false);
    m_secureLocalPac = json["secureLocalPac"].toBool(false);
  } else {
    *this = Configuration();
  }
}

QJsonObject Configuration::toJson() {
  QJsonObject json;
  json.insert("configs", Server::toJson(m_servers));
  json.insert("proxy", Proxy::toJson(m_proxy));

  json.insert("localPort", m_localPort);
  json.insert("shareOverLan", m_shareOverLan);

  json.insert("enabled", m_enabled);
  json.insert("index", m_index);
  json.insert("global", m_global);
  json.insert("autoStart", m_autoStart);
  json.insert("autoCheckUpdate", m_autoCheckUpdate);
  json.insert("checkPreRelease", m_checkPreRelease);
  json.insert("isVerboseLogging", m_isVerboseLogging);

  json.insert("pacUrl", m_pacUrl);
  json.insert("useOnlinePac", m_useOnlinePac);
  json.insert("secureLocalPac", m_secureLocalPac);
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

  config::Configuration m_config;

  explicit SingleChooser(config::Configuration &configuration)
    : m_config(configuration) {}

  Server getServer(QSS::Address &destination, int index) override {
    auto servers = m_config.getServers();
    if (servers.empty()) {
      return Server();
    }
    int i = index;
    if (!(0 <= i && i < servers.length())) {
      i = 0;
    }
    return servers[i];
  }
};
