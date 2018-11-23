#include "Server.h"

namespace config {

Server::Server() : QSX::Server() {}

Server::Server(const QSX::Server &s) {
  server = s.server;
  passwd = s.passwd;
  method = s.method;
  remarks = s.remarks;
  server_port = s.server_port;
  timeout = s.timeout;
}

Server::Server(const QJsonObject &json) {
  fromJson(json);
}

uint64_t Server::hash() {
  QString s;
  s.append(server)
    .append(QString::number(server_port))
    .append(passwd)
    .append(method)
    .append(remarks)
    .append(QString::number(timeout));
  return QCryptographicHash::hash(s.toLatin1(),
                                  QCryptographicHash::Sha256).toULong();
}

void Server::fromJson(const QJsonObject &json) {
  if (!json.isEmpty()) {
    server = json["server"].toString();
    server_port = static_cast<uint16_t>(json["server_port"].toInt());
    passwd = json["password"].toString();
    method = json["method"].toString();
    remarks = json["remarks"].toString();
    timeout = static_cast<uint16_t>(json["timeout"].toInt());
  } else {
    *this = Server();
  }
}

QJsonObject Server::toJson() {
  QJsonObject json;
  json.insert("server", server);
  json.insert("server_port", server_port);
  json.insert("password", passwd);
  json.insert("method", method);
  json.insert("remarks", remarks);
  json.insert("timeout", timeout);
  return json;
}

QString Server::toUri() {
  QString s = QString("%1:%2@%3:%4")
    .arg(method)
    .arg(passwd)
    .arg(server)
    .arg(server_port);
  return QString("ss://%1#%2")
    .arg(QString(s.toLocal8Bit().toBase64()))
    .arg(remarks);
}

bool Server::fromUri(const QString &uri) {
  QRegExp r_ss("ss://(.+)#(.+)");
  r_ss.indexIn(uri);
  if (r_ss.isEmpty()) {
    return false;
  }
  auto decoded = QString(QByteArray::fromBase64(r_ss.cap(1).toLatin1()));
  remarks = r_ss.cap(2);
  QRegExp r_decoded("(.+):(.+)@(.+):(.+)");
  r_decoded.indexIn(decoded);
  if (r_decoded.isEmpty()) {
    return false;
  }

  method = r_decoded.cap(1);
  passwd = r_decoded.cap(2);
  server = r_decoded.cap(3);
  server_port = static_cast<uint16_t>(r_decoded.cap(4).toInt());
  return true;
}

}
