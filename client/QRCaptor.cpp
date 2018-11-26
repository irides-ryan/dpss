#include "QRCaptor.h"
#include <QApplication>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QDesktopWidget>
#include <QScreen>
#include <zbar.h>

QRCaptor::QRCaptor(QWidget *parent) :
  QMainWindow(parent) {
#ifdef Q_OS_WIN
  /*
   * On Windows, it requires Qt::FramelessWindowHint to be set to make
   * translucent background work, but we need a window with borders.
   * Therefore, we set the entire window semi-transparent so that
   * users are still able to see the region below while moving the
   * capturer above the QR code image.
   */
  this->setWindowOpacity(0.75);
#else
  this->setAttribute(Qt::WA_TranslucentBackground, true);
#endif
  this->setWindowTitle(tr("QR Capturer"));
  this->setMinimumSize(400, 400);
}

QRCaptor::~QRCaptor() {}

QString QRCaptor::scanEntireScreen() {
  QString uri;
  QList<QScreen *> screens = qApp->screens();
  for (auto &screen : screens) {
    QImage raw_sc = screen->grabWindow(qApp->desktop()->winId()).toImage();
    QString result = decodeImage(raw_sc);
    if (!result.isNull()) {
      uri = result;
    }
  }
  return uri;
}

void QRCaptor::moveEvent(QMoveEvent *e) {
  QMainWindow::moveEvent(e);
  decodeCurrentRegion();
}

void QRCaptor::resizeEvent(QResizeEvent *e) {
  QMainWindow::resizeEvent(e);
  decodeCurrentRegion();
}

void QRCaptor::closeEvent(QCloseEvent *e) {
  QMainWindow::closeEvent(e);
  emit closed();
}

void QRCaptor::decodeCurrentRegion() {
  QScreen *sc = qApp->screens().at(qApp->desktop()->screenNumber(this));
  QRect geometry = this->geometry();
  QImage raw_sc = sc->grabWindow(qApp->desktop()->winId(),
                                 geometry.x(),
                                 geometry.y(),
                                 geometry.width(),
                                 geometry.height()).toImage();
  QString result = decodeImage(raw_sc);
  if (!result.isNull()) {
    this->close();
    // moveEvent and resizeEvent both happen quite frequent
    // it's very likely this signal would be emitted multiple times
    // the solution is to use Qt::DirectConnection signal-slot connection
    // and disconnect such a connection in the slot function
    emit qrCodeFound(result);
  }
}


QString QRCaptor::decodeImage(const QImage &img) {
  QString uri;
  auto grayImage = img.convertToFormat(QImage::Format_Grayscale8);

  //use zbar to decode the QR code
  zbar::ImageScanner scanner;
  zbar::Image image(static_cast<unsigned int>(grayImage.bytesPerLine()),
                    static_cast<unsigned int>(grayImage.height()),
                    "Y800",
                    grayImage.bits(),
                    static_cast<unsigned long>(grayImage.byteCount()));
  scanner.scan(image);
  zbar::SymbolSet res_set = scanner.get_results();
  for (auto it = res_set.symbol_begin(); it != res_set.symbol_end(); ++it) {
    if (it->get_type() == zbar::ZBAR_QRCODE) {
      /*
       * uri will be overwritten if the result is valid
       * this means always the last uri gets used
       * therefore, please only leave one QR code for the sake of accuracy
       */
      QString result = QString::fromStdString(it->get_data());
      if (result.left(5).compare("ss://", Qt::CaseInsensitive) == 0) {
        uri = result;
      }
    }
  }

  return uri;
}
