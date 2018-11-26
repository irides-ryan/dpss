#pragma once

#include "stdafx.h"
#include "widget/QRWidget.h"
#include "widget/SingleListView.h"

class ShareDialog : public QDialog {
Q_OBJECT

public:
  explicit ShareDialog(QWidget *parent = nullptr);
  ~ShareDialog() override;

private slots:
  void onItemSelected();
  void onSaveQRCodeImage();

private:
  SingleListView *m_listView;
  QRWidget *m_widgetQR;
  QLineEdit *m_labelUrl;
  QPushButton *m_btnSave;
  QGridLayout *m_layout;

  void setCurrentUrl(QString url);
};
