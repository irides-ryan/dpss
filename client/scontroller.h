#pragma once

#include <util/listener.h>

class SController : public QObject {
Q_OBJECT
private:
  QSX::Listener *m_listener = nullptr;

public:
  explicit SController(QObject *parent = nullptr);
  ~SController() override;

  bool start();
  void stop();

  bool isRunning() {
    return true;
  }

signals:
  void accept(QSX::TcpHandler *handler);

};
