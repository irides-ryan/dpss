#pragma once

#include "config/Configuration.h"

class GConfig
{
private:
  const char* CONFIG_FILE = "gui-config.json";

  config::Configuration configuration;

  void Load(QString const &path);
  void Save(QString const &path);
public:
  GConfig();
  ~GConfig();

  void Load();
  void Save();
};
