#include <QtCore/QJsonDocument>
#include "GConfig.h"
#include "utils.h"

GConfig::GConfig()
  : m_configPath(QDir(Utils::configPath()).filePath(CONFIG_FILE)) {

}

GConfig::~GConfig() {

}

void GConfig::Load(QString const &path) {
  QFile file(path);
  QFileInfo fileInfo(file);
  QDir dir = fileInfo.absoluteDir();

  if (!dir.exists()) {
    dir.mkpath(dir.absolutePath());
  }

  // file not exist
  if (!file.exists()) {
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      // TODO let someone know.
      return;
    } else {
      // write a default value
      QJsonDocument document;
      document.setObject(m_config.toJson());
      file.write(document.toJson());
      file.flush();
      file.close();
    }
  } else {
    // file exists
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    auto json = QJsonDocument::fromJson(file.readAll()).object();
    m_config.fromJson(json);

    file.close();
  }
}

void GConfig::Save(QString const &path) {
  QFile file(path);
  QFileInfo fileInfo(file);
  QDir dir = fileInfo.absoluteDir();

  if (!dir.exists()) {
    dir.mkpath(dir.absolutePath());
  }

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    // TODO let someone know.
    return;
  } else {
    // write a default value
    QJsonDocument document;
    document.setObject(m_config.toJson());
    file.write(document.toJson());
    file.flush();
    file.close();
  }
}

void GConfig::Load() {
  Load(m_configPath);
}

void GConfig::Save() {
  Save(m_configPath);
}
