#pragma once

#include <QObject>
#include <QDir>
#include "config/Configuration.h"

class GConfig : public QObject {
Q_OBJECT

private:
  const char *CONFIG_FILE = "gui-config.json";

  config::Configuration m_config;
  const QString m_configPath;

public:
  GConfig();
  ~GConfig() override;

  static GConfig *instance() {
    static GConfig m_instance;
    return &m_instance;
  }

  void load(QString const &path);
  void save(QString const &path);
  void load();
  void save();

  config::Configuration &config() {
    return m_config;
  }

  void config(config::Configuration &config) {
    m_config = config;
  }

  config::Server currentServer() {
    auto index = m_config.m_index;
    auto &servers = m_config.m_servers;
    if (servers.empty()) {
      return config::Server();
    }
    if (index >= servers.size()) {
      index = 0;
    }
    auto &server = servers.at(index);
    return config::Server(server);
  }

  bool addServer(QString const &url) {
    config::Server s;
    bool isGood = s.fromUri(url);
    if (isGood) {
      m_config.m_servers.append(s);
    }
    return isGood;
  }

  QList<QString> remarks() {
    QList<QString> remarks;
    for (auto &s : m_config.m_servers) {
      remarks.append(s.remarks);
    }
    return remarks;
  }

// Getter and Setter
#define GET(type, name) type name() { return m_config.m_##name; }
#define SET(type, name) void name(type v) { m_config.m_##name = v; }
#define GET_SET(type, name) GET(type, name) SET(type, name)

  GET_SET(bool, shareOverLan)
  GET_SET(uint16_t, localPort)
  GET_SET(int, index)
  GET_SET(bool, enabled)
  GET_SET(bool, autoStart)
  GET_SET(bool, autoCheckUpdate)
  GET_SET(bool, global)
  GET_SET(bool, isVerboseLogging)
  GET_SET(bool, checkPreRelease)
  GET_SET(QString, pacUrl);
  GET_SET(bool, useOnlinePac);
  GET_SET(bool, secureLocalPac);
};
