#include "scontroller.h"
#include "GConfig.h"

SController::SController(QObject *parent) : QObject(parent) {

}

SController::~SController() {
  stop();
}

bool SController::start() {
  stop();

  auto config = GConfig::instance()->config();
  m_listener = new QSX::Listener(this);
  QObject::connect(m_listener, &QSX::Listener::accept,
                   this, &SController::accept);
  return m_listener->start(config);
}

void SController::stop() {
  if (m_listener) {
    m_listener->stop();
    m_listener->deleteLater();
    m_listener = nullptr;
  }
}
