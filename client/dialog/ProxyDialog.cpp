#include <GConfig.h>
#include "ProxyDialog.h"
#include "ui_ProxyDialog.h"

#include "GuiConfig.h"

ProxyDialog::ProxyDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ProxyDialog) {
    ui->setupUi(this);

    ui->lineEditProxyPort->setValidator(new QIntValidator(0, 65535, this));
    ui->lineEditTimeout->setValidator(new QIntValidator(0, 99, this));

    m_proxy = GConfig::instance()->config().getProxy();

    ui->checkBoxUseProxy->setChecked(m_proxy.use);
    emit ui->checkBoxUseProxy->stateChanged(m_proxy.use);

    if (m_proxy.type == 0) {
        ui->comboBoxProxyType->setCurrentIndex(0);
    } else {
        ui->comboBoxProxyType->setCurrentIndex(1);
    }
    ui->lineEditProxyAddr->setText(m_proxy.server);
    ui->lineEditProxyPort->setText(QString::number(m_proxy.port));
    ui->lineEditTimeout->setText(QString::number(m_proxy.timeout));
}

ProxyDialog::~ProxyDialog() {
    delete ui;
}

bool ProxyDialog::isConfigChanged() {
    return m_isConfigChanged;
}

void ProxyDialog::on_checkBoxUseProxy_stateChanged(int state)
{
    if (state == 0) {
        ui->comboBoxProxyType->setEnabled(false);
        ui->lineEditProxyAddr->setEnabled(false);
        ui->lineEditProxyPort->setEnabled(false);
        ui->lineEditTimeout->setEnabled(false);
    } else {
        ui->comboBoxProxyType->setEnabled(true);
        ui->lineEditProxyAddr->setEnabled(true);
        ui->lineEditProxyPort->setEnabled(true);
        ui->lineEditTimeout->setEnabled(true);
    }
}

void ProxyDialog::on_buttonBox_accepted()
{
    // TODO verify the "proxyServer"
    QSX::Proxy _proxy = GConfig::instance()->config().getProxy();

    m_proxy.use = ui->checkBoxUseProxy->isChecked();
    m_proxy.type = ui->comboBoxProxyType->currentIndex();
    m_proxy.server = ui->lineEditProxyAddr->text();
    m_proxy.port = static_cast<uint16_t>(ui->lineEditProxyPort->text().toInt());
    m_proxy.timeout = static_cast<uint16_t>(ui->lineEditTimeout->text().toInt());

    m_isConfigChanged = _proxy == m_proxy;
    if (m_isConfigChanged) {
        GConfig::instance()->config().setProxy(m_proxy);
    }
}
