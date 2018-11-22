#pragma once

#include <QObject>
#include <QDir>
#include "config/Configuration.h"

class GConfig : public QObject
{
Q_OBJECT

private:
  const char* CONFIG_FILE = "gui-config.json";

  config::Configuration m_config;
  const QString m_configPath;

  void Load(QString const &path);
  void Save(QString const &path);
public:
  GConfig();
  ~GConfig() override;

  void Load();
  void Save();
};
