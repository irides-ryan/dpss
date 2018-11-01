#include "Proxy.h"

namespace config {

Proxy::Proxy()
  : use(false),
    type(SOCKS5),
    server(""),
    port(0),
    timeout(3)
{}

Proxy::Proxy(QJsonObject &json) {
  fromJson(json);
}

void Proxy::fromJson(QJsonObject &json) {
  if (!json.isEmpty()) {
    use = json["useProxy"].toBool();
    type = json["proxyType"].toInt();
    server = json["proxyServer"].toString();
    port = json["proxyPort"].toInt();
    timeout = json["proxyTimeout"].toInt();
  } else {
    *this = Proxy();
  }
}

QJsonObject Proxy::toJson() {
  QJsonObject json;
  json.insert("useProxy", use);
  json.insert("proxyType", type);
  json.insert("proxyServer", server);
  json.insert("proxyPort", port);
  json.insert("proxyTimeout", timeout);
  return json;
}

}
