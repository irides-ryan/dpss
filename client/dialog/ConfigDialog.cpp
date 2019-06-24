#include <GConfig.h>
#include "ConfigDialog.h"
#include "ui_ConfigDialog.h"
#include "utils.h"

using QCS = Qt::CheckState;
using QMB = QMessageBox;

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog),
    m_currentPos(-1),
    m_isConfigChanged(false)
{
    ui->setupUi(this);

    m_servers = GConfig::instance()->config().getServers();

    auto methodBA = QSS::Cipher::supportedMethods();
    QStringList methodList;
    for (auto &method : methodBA) {
        methodList << QString::fromStdString(method);
    }
    qSort(methodList.begin(), methodList.end());
    ui->comboBoxEncryption->addItems(methodList);

    ui->listWidget->clear();
    for (auto &s : m_servers) {
        ui->listWidget->addItem(s.remarks);
    }
    if (0 == ui->listWidget->count()) {
        ui->pushButtonDelete->setEnabled(false);
    } else {
        ui->listWidget->setCurrentRow(0);
    }

    int proxyPort = GConfig::instance()->localPort();
    ui->spinBoxProxyPort->setValue(proxyPort);

    // TODO keep it, re-use it until mixed-proxy re-implement.
//    bool useMixedProxy = guiConfig->get("useMixedProxy").toBool(false);
//    QCS state = useMixedProxy ? QCS::Checked : QCS::Unchecked;
//    ui->checkBoxMixedPort->setCheckState(state);

    Dtk::Widget::moveToCenter(this);
}

ConfigDialog::~ConfigDialog() {
    ui->listWidget->clear();
    delete ui;
}

bool ConfigDialog::isConfigChanged() {
    return m_isConfigChanged;
}

void ConfigDialog::stashConfig(int index) {
    auto config = m_servers[index];
    // TODO validate them
    config.server = ui->lineEditServerAddr->text();
    config.server_port = static_cast<uint16_t>(ui->spinBoxServerPort->value());
    config.passwd = ui->lineEditPassword->text();
    config.remarks = ui->lineEditRemarks->text();
    config.local_port = static_cast<uint16_t>(ui->spinBoxProxyPort->value());
    config.method = ui->comboBoxEncryption->currentText();
    config.timeout = static_cast<uint16_t>(ui->spinBoxTimeout->value());

    m_servers.replace(index, config);
}

void ConfigDialog::showConfig(int index) {
    auto config = m_servers[index];
    ui->lineEditPassword->setText(config.passwd);
    ui->lineEditRemarks->setText(config.remarks);
    ui->lineEditServerAddr->setText(config.server);
    ui->spinBoxServerPort->setValue(config.server_port);
    ui->spinBoxTimeout->setValue(config.timeout);
    ui->comboBoxEncryption->setCurrentText(config.method);
}

void ConfigDialog::save() {
    stashConfig(m_currentPos);
//    bool useMixedProxy = ui->checkBoxMixedPort->checkState() == QCS::Checked;
    auto localPort = static_cast<uint16_t>(ui->spinBoxProxyPort->value());

    // get origin configuration
    auto _servers = GConfig::instance()->config().getServers();
//    auto _useMixedProxy = GuiConfig::instance()->get("useMixedProxy").toBool();
    auto _localPort = GConfig::instance()->localPort();

    // check for changes
    bool serversChanged = (m_servers != _servers);
//    bool proxyChanged = useMixedProxy == _useMixedProxy;
    bool portChanged = localPort != _localPort;

    m_isConfigChanged = serversChanged || portChanged;
    if (m_isConfigChanged) {
        auto ret = QMB::information(this,
                                    tr("Save"),
                                    tr("Configuration changed. Save it?"),
                                    QMB::Yes | QMB::No);
        if (QMB::Yes == ret) {
            GConfig::instance()->config().setServers(m_servers);
            GConfig::instance()->localPort(localPort);
//            GuiConfig::instance()->set("useMixedProxy", useMixedProxy);
            GConfig::instance()->save();
        }
    }
}

void ConfigDialog::on_listWidget_currentRowChanged(int currentRow) {
    if (currentRow < 0) {
        // list is cleared.
        return;
    }
    if (-1 != m_currentPos) {
        // don't stash the config if m_currentPos is set to -1.
        stashConfig(m_currentPos);
    }
    showConfig(currentRow);
    m_currentPos = currentRow;
}

void ConfigDialog::on_pushButtonAdd_clicked() {
    QSX::Server newOne;
    if (newOne.remarks.isEmpty()) {
        newOne.remarks = "unnamed";
    }

    int addPos = m_servers.size();
    m_servers.append(newOne);

    // push back this config.
    ui->listWidget->insertItem(addPos, newOne.remarks);
    ui->listWidget->setCurrentRow(addPos);
    if (!ui->pushButtonDelete->isEnabled()) {
      ui->pushButtonDelete->setEnabled(true);
    }
}

void ConfigDialog::on_pushButtonDelete_clicked() {
    int ret = QMB::warning(this,
                           tr("Delete"),
                           tr("Are you sure to delete this?"),
                           QMB::Yes | QMB::No);
    if (ret == QMB::No) {
        return;
    }

    int currentPos = m_currentPos;
    delete ui->listWidget->takeItem(currentPos);
    // don't stash the config
    m_currentPos = -1;
    m_servers.removeAt(currentPos);

    if (0 == ui->listWidget->count()) {
        ui->lineEditPassword->clear();
        ui->lineEditRemarks->clear();
        ui->lineEditServerAddr->clear();
        ui->spinBoxServerPort->setValue(8388);
        // could not delete any more.
        ui->pushButtonDelete->setEnabled(false);
    } else {
      currentPos = currentPos < m_servers.size() ? currentPos : currentPos - 1;
      // this action won't trigger on_currentRowChanged()
      ui->listWidget->setCurrentRow(currentPos);
      m_currentPos = currentPos;
    }
}

void ConfigDialog::on_pushButtonDuplicate_clicked() {
    auto newOne = m_servers[m_currentPos];
    int addPos = m_servers.size();
    m_servers.append(newOne);

    // push back this config.
    ui->listWidget->insertItem(addPos, newOne.remarks);
    ui->listWidget->setCurrentRow(addPos);
}

void ConfigDialog::on_pushButtonMoveUp_clicked() {
    if (m_currentPos == 0) {
        return;
    }
    int currentPos = m_currentPos;

    m_currentPos = -1;
    auto tmp = ui->listWidget->takeItem(currentPos - 1);
    // swap the two configs
    m_servers.swap(currentPos, currentPos - 1);

    m_currentPos = -1;
    ui->listWidget->insertItem(currentPos, tmp);

    ui->listWidget->setCurrentRow(currentPos - 1);
    m_currentPos = currentPos - 1;
}

void ConfigDialog::on_pushButtonMoveDown_clicked() {
//    if (m_currentPos == m_configs.size() - 1) {
    if (m_currentPos == m_servers.size() - 1) {
        return;
    }
    int currentPos = m_currentPos;

    m_currentPos = -1;
    auto tmp = ui->listWidget->takeItem(currentPos + 1);
    // swap the two configs
    m_servers.swap(currentPos, currentPos + 1);

    m_currentPos = -1;
    ui->listWidget->insertItem(currentPos, tmp);

    ui->listWidget->setCurrentRow(currentPos + 1);
    m_currentPos = currentPos + 1;
}

void ConfigDialog::on_lineEditRemarks_textEdited(const QString &edited) {
    auto currentItem = ui->listWidget->item(m_currentPos);
    if (edited.isEmpty()) {
        currentItem->setData(0, "unnamed");
    } else if (currentItem->text() != edited) {
        currentItem->setData(0, edited);
    }
}

void ConfigDialog::on_pushButtonOK_clicked() {
    save();
    close();
}
