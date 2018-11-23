#pragma once

#include <util/listener.h>

class SController : public QObject {
Q_OBJECT
private:
  std::unique_ptr<QSX::Listener> m_listener;

public:
  explicit SController(QObject *parent = nullptr);
  ~SController() override;

  bool start();
  void stop();

  bool isRunning() {
    return true;
  }
};
