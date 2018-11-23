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

  void Load(QString const &path);
  void Save(QString const &path);

public:
  GConfig();
  ~GConfig() override;

  static GConfig *instance() {
    static GConfig m_instance;
    return &m_instance;
  }

  void Load();
  void Save();

  config::Configuration &config() {
    return m_config;
  }

  void config(config::Configuration &config) {
    m_config = config;
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
