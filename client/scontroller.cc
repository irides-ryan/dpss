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
  m_listener = std::make_unique<QSX::Listener>();
  return m_listener->start(config);
}

void SController::stop() {
  if (m_listener) {
    m_listener->stop();
  }
}
