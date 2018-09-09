#include "ProxyDialog.h"
#include "ui_ProxyDialog.h"

#include "GuiConfig.h"

ProxyDialog::ProxyDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ProxyDialog) {
    ui->setupUi(this);

    ui->lineEditProxyPort->setValidator(new QIntValidator(0, 65535, this));
    ui->lineEditTimeout->setValidator(new QIntValidator(0, 99, this));

    bool usePorxy = false;
    int  proxyType = 0;
    QString proxyServer = "127.0.0.1";
    int  proxyPort = 8080;
    int  proxyTimeout = 3;

    auto configProxy = GuiConfig::instance()->get("proxy");
    if (configProxy.isUndefined()) {
        // then define it
        m_confProxy.insert("useProxy", usePorxy);
        m_confProxy.insert("proxyType", proxyType);
        m_confProxy.insert("proxyServer", proxyServer);
        m_confProxy.insert("proxyPort", proxyPort);
        m_confProxy.insert("proxyTimeout", proxyTimeout);
        GuiConfig::instance()->set("proxy", m_confProxy);
    } else {
        m_confProxy = configProxy.toObject();
        usePorxy = m_confProxy["useProxy"].toBool(false);
        proxyType = m_confProxy["proxyType"].toInt(0);
        proxyServer = m_confProxy["proxyServer"].toString();
        proxyPort = m_confProxy["proxyPort"].toInt(8080);
        proxyTimeout = m_confProxy["proxyTimeout"].toInt(3);
    }

    ui->checkBoxUseProxy->setChecked(usePorxy);
    emit ui->checkBoxUseProxy->stateChanged(usePorxy);

    if (proxyType == 0) {
        ui->comboBoxProxyType->setCurrentIndex(0);
    } else {
        ui->comboBoxProxyType->setCurrentIndex(1);
    }
    ui->lineEditProxyAddr->setText(proxyServer);
    ui->lineEditProxyPort->setText(QString::number(proxyPort));
    ui->lineEditTimeout->setText(QString::number(proxyTimeout));
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

    m_confProxy["useProxy"] = ui->checkBoxUseProxy->isChecked();
    m_confProxy["proxyType"] = ui->comboBoxProxyType->currentIndex();
    m_confProxy["proxyServer"] = ui->lineEditProxyAddr->text();
    m_confProxy["proxyPort"] = ui->lineEditProxyPort->text().toInt();
    m_confProxy["proxyTimeout"] = ui->lineEditTimeout->text().toInt();

    if (GuiConfig::instance()->set("proxy", m_confProxy)) {
        m_isConfigChanged = true;
        GuiConfig::instance()->saveToDisk();
    }
}
