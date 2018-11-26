#include "ShareDialog.h"
#include "utils.h"
#include "widget/SingleListItem.h"
#include <GConfig.h>
#include <DDesktopServices>
#include <QGridLayout>
#include <QHBoxLayout>

ShareDialog::ShareDialog(QWidget *parent) :
  QDialog(parent) {
  // 无法获取Item的点击事件，自己造了一个
  m_listView = new SingleListView(this);
  m_widgetQR = new QRWidget(this);
  m_labelUrl = new QLineEdit(this);
  m_labelUrl->setReadOnly(true);
  m_btnSave = new QPushButton(tr("Save QRCode Image"), this);
  m_listView->setMinimumSize(200, 350);
  m_widgetQR->setMinimumSize(350, 350);

  QList<DSimpleListItem *> items;
  auto servers = GConfig::instance()->config().getServers();
  for (auto &s : servers) {
    QString label = s.remarks + " (" + s.server + ":"
                    + QString::number(s.server_port) + ")";
    auto item = new SingleListItem(label);
    item->setProperty("data", config::Server(s).toUri());
    QObject::connect(item, &SingleListItem::selected,
                     this, &ShareDialog::onItemSelected);
    items << item;
  }
  if (!servers.isEmpty()) {
    setCurrentUrl(config::Server(servers.first()).toUri());
  }
  m_listView->refreshItems(items);

  m_layout = new QGridLayout(this);
  m_layout->addWidget(m_widgetQR, 1, 0);
  m_layout->addWidget(m_listView, 1, 1);
  m_layout->addWidget(m_labelUrl, 2, 0, 1, 2);
  m_layout->addWidget(m_btnSave, 3, 1, Qt::AlignTop | Qt::AlignRight);
  this->setLayout(m_layout);

  QObject::connect(m_btnSave, &QPushButton::clicked,
                   this, &ShareDialog::onSaveQRCodeImage);
}

ShareDialog::~ShareDialog() {
  delete m_widgetQR;
  delete m_labelUrl;
  delete m_btnSave;
  delete m_listView;
  delete m_layout;
}

void ShareDialog::onItemSelected() {
  auto item = dynamic_cast<SingleListItem *>(sender());
  QString uri = item->property("data").toString();
  setCurrentUrl(uri);
}

void ShareDialog::onSaveQRCodeImage() {
  auto filename = QFileDialog::getSaveFileName(nullptr, tr("Save QRCode Image"),
                                               QStandardPaths::standardLocations(
                                                 QStandardPaths::PicturesLocation).first(),
                                               tr("Images (*.png *.xpm *.jpg)"));
  if (filename.isEmpty()) {
    return;
  }
  bool flag = m_widgetQR->getQRImage().save(filename, "png");
  if (flag) {
    Utils::info(tr("Save QRCode Image Success"));
    DDesktopServices::showFileItem(filename);
  } else {
    Utils::critical(tr("Save QRCode Image Error"));
  }
}

void ShareDialog::setCurrentUrl(QString url) {
  m_widgetQR->setQRData(url.toLocal8Bit());
  m_widgetQR->update();
  m_labelUrl->setText(url);
}
