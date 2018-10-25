#include "ConfigDialog.h"
#include "ui_ConfigDialog.h"
#include "GuiConfig.h"
#include "utils.h"

using QCS = Qt::CheckState;
using QMB = QMessageBox;

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog),
    m_currentPos(-1),
    m_isConfigChanged(false)
{
    auto guiConfig = GuiConfig::instance();
    ui->setupUi(this);

    m_configs = guiConfig->getConfigs();

    auto methodBA = QSS::Cipher::supportedMethods();
    QStringList methodList;
    for (auto &method : methodBA) {
        methodList << QString::fromStdString(method);
    }
    qSort(methodList.begin(), methodList.end());
    ui->comboBoxEncryption->addItems(methodList);

    ui->listWidget->clear();
    for (auto i : m_configs) {
        ui->listWidget->addItem(i.toObject()["remarks"].toString());
    }
    if (0 == ui->listWidget->count()) {
        ui->pushButtonDelete->setEnabled(false);
    } else {
        ui->listWidget->setCurrentRow(0);
    }

    int proxyPort = guiConfig->get("localPort").toInt(1080);
    ui->spinBoxProxyPort->setValue(proxyPort);

    bool useMixedProxy = guiConfig->get("useMixedProxy").toBool(false);
    QCS state = useMixedProxy ? QCS::Checked : QCS::Unchecked;
    ui->checkBoxMixedPort->setCheckState(state);

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
    auto config = m_configs[index].toObject();
    // TODO validate them
    config["password"] = ui->lineEditPassword->text();
    config["remarks"] = ui->lineEditRemarks->text();
    config["server"] = ui->lineEditServerAddr->text();
    config["server_port"] = ui->spinBoxServerPort->value();
    config["local_port"] = ui->spinBoxProxyPort->value();
    config["timeout"] = ui->spinBoxTimeout->value();
    config["method"] = ui->comboBoxEncryption->currentText();

    m_configs.replace(index, config);
}

void ConfigDialog::showConfig(int index) {
    auto config = m_configs[index].toObject();
    ui->lineEditPassword->setText(config["password"].toString());
    ui->lineEditRemarks->setText(config["remarks"].toString());
    ui->lineEditServerAddr->setText(config["server"].toString());
    ui->spinBoxServerPort->setValue(config["server_port"].toInt());
    ui->spinBoxTimeout->setValue(config["timeout"].toInt());
    ui->comboBoxEncryption->setCurrentText(config["method"].toString());
}

void ConfigDialog::save() {
    bool useMixedProxy = ui->checkBoxMixedPort->checkState() == QCS::Checked;
    int localPort = ui->spinBoxProxyPort->value();

    // get origin configuration
    auto _configs = GuiConfig::instance()->getConfigs();
    auto _useMixedProxy = GuiConfig::instance()->get("useMixedProxy").toBool();
    auto _localPort = GuiConfig::instance()->get("localPort").toInt();

    // check for changes
    bool configsChanged = (m_configs == _configs);
    bool proxyChanged = useMixedProxy == _useMixedProxy;
    bool portChanged = localPort == _localPort;

    m_isConfigChanged = configsChanged || proxyChanged || portChanged;
    if (m_isConfigChanged) {
        auto ret = QMB::information(this,
                                    tr("Save"),
                                    tr("Configuration changed. Save it?"),
                                    QMB::Yes | QMB::No);
        if (QMB::Yes == ret) {
            GuiConfig::instance()->setConfigs(m_configs);
            GuiConfig::instance()->set("useMixedProxy", useMixedProxy);
            GuiConfig::instance()->set("localPort", localPort);
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
    QJsonObject config;
    config.insert("remarks", "unnamed");
    config.insert("server", "");
    config.insert("server_port", 8388);
    config.insert("password", "");
    config.insert("local_port", 1080);
    config.insert("timeout", 6);
    config.insert("method", "");
    // GuiConfig::calId(config);
    int addPos = m_configs.size();
    m_configs.append(config);
    // push back this config.
    ui->listWidget->insertItem(addPos, config["remarks"].toString());
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
    m_configs.removeAt(currentPos);

    if (0 == ui->listWidget->count()) {
        ui->lineEditPassword->clear();
        ui->lineEditRemarks->clear();
        ui->lineEditServerAddr->clear();
        ui->spinBoxServerPort->setValue(8388);
        // could not delete any more.
        ui->pushButtonDelete->setEnabled(false);
    } else {
      currentPos = currentPos < m_configs.size() ? currentPos : currentPos - 1;
      // this action won't trigger on_currentRowChanged()
      ui->listWidget->setCurrentRow(currentPos);
      m_currentPos = currentPos;
    }
}

void ConfigDialog::on_pushButtonDuplicate_clicked() {
    QJsonObject config = m_configs[m_currentPos].toObject();
    int addPos = m_configs.size();
    m_configs.append(config);
    // push back this config.
    ui->listWidget->insertItem(addPos, config["remarks"].toString());
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
    auto _config = m_configs[currentPos];
    m_configs[currentPos] = m_configs[currentPos - 1].toObject();
    m_configs[currentPos - 1] = _config.toObject();

    m_currentPos = -1;
    ui->listWidget->insertItem(currentPos, tmp);

    ui->listWidget->setCurrentRow(currentPos - 1);
    m_currentPos = currentPos - 1;
}

void ConfigDialog::on_pushButtonMoveDown_clicked() {
    if (m_currentPos == m_configs.size() - 1) {
        return;
    }
    int currentPos = m_currentPos;

    m_currentPos = -1;
    auto tmp = ui->listWidget->takeItem(currentPos + 1);
    // swap the two configs
    auto _config = m_configs[currentPos];
    m_configs[currentPos] = m_configs[currentPos + 1].toObject();
    m_configs[currentPos + 1] = _config.toObject();

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
