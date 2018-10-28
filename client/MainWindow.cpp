#include "stdafx.h"
#include "utils.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ConfigDialog.h"
#include "PACUrlDialog.h"
#include "ProxyDialog.h"
#include "LogMainWindow.h"
#include "Toolbar.h"
#include "GuiConfig.h"
#include "DDEProxyModeManager.h"
#include "widget/ProfileView.h"
#include "widget/ProfileItem.h"
#include "QRCodeCapturer.h"
#include "SSValidator.h"
#include "ShareDialog.h"
#include <DDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
    DMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    installEventFilter(this);
    GuiConfig::instance()->readFromDisk();

    systemTrayIcon = new QSystemTrayIcon();
    systemTrayIcon->setContextMenu(ui->menuTray);
    systemTrayIcon->setIcon(QIcon(Utils::getIconQrcPath("ss16.png")));
    systemTrayIcon->show();

    proxyManager = new ProxyManager(this);
    const auto &guiConfig = GuiConfig::instance();
    systemProxyModeManager = new DDEProxyModeManager(this);
    if (guiConfig->get("enabled").toBool()) {
        on_actionEnable_System_Proxy_triggered(true);
        if (guiConfig->get("global").toBool()) {
            switchToGlobal();
        } else {
            switchToPacMode();
        }
    }
    in = 0;
    out = 0;
    ins.clear();
    outs.clear();

    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTrayIcon);
    timer->start(100);

    // 流量监控
    connect(proxyManager, &ProxyManager::bytesReceivedChanged, [=](quint64 n) {
        qDebug() << "bytesReceivedChanged" << n;
        term_usage_in = n;
        GuiConfig::instance()->setCurrentTermUsage(term_usage_in
                                                   + term_usage_out);
    });
    connect(proxyManager, &ProxyManager::bytesSentChanged, [=](quint64 n) {
        qDebug() << "bytesSentChanged" << n;
        term_usage_out = n;
        GuiConfig::instance()->setCurrentTermUsage(term_usage_in
                                                   + term_usage_out);
    });
    connect(proxyManager, &ProxyManager::newBytesReceived, [=](quint64 n) {
        qDebug() << "newBytesReceived" << n;
        in += n;
        GuiConfig::instance()->addTotalUsage(n);
    });
    connect(proxyManager, &ProxyManager::newBytesSent, [=](quint64 n) {
        qDebug() << "newBytesSent" << n;
        out += n;
        GuiConfig::instance()->addTotalUsage(n);
    });

    updateTrayIcon();
    on_actionStart_on_Boot_triggered(guiConfig->get("autostart").toBool(true));
    updateMenu();
}

MainWindow::~MainWindow() {
    proxyManager->stop();
    systemProxyModeManager->switchToNone();
    delete ui;
}

void MainWindow::switchToPacMode() {
    auto guiConfig = GuiConfig::instance();
    QString online_pac_uri = "http://file.lolimay.cn/autoproxy.pac";
    QString pacURI = "";
    if (guiConfig->get("useOnlinePac").toBool(true)) {
        pacURI = guiConfig->get("pacUrl").toString();
        if (pacURI.isEmpty()) {
            Utils::warning("online pac uri is empty. we will use default uri.");
            pacURI = online_pac_uri;
            guiConfig->set("pacUrl", pacURI);
        }
    } else {
        QString pac_file = QDir(Utils::configPath()).filePath("autoproxy.pac");
        QFile file(pac_file);
        if (!file.exists()) {
            Utils::warning("local pac is not exist. we will use on pac file. "
                           "you can change it");
            pacURI = online_pac_uri;
            guiConfig->set("pacUrl", pacURI);
            guiConfig->set("useOnlinePac", true);
        } else {
            pacURI = "file://" + pac_file;
        }
    }
    systemProxyModeManager->switchToAuto(pacURI);
}

void MainWindow::switchToGlobal() {
    auto guiConfig = GuiConfig::instance();
    QString local_address = guiConfig->get("local_address").toString();
    if (local_address.isEmpty()) {
        local_address = "127.0.0.1";
        guiConfig->set("local_address", local_address);
    }
    int local_port = guiConfig->get("local_port").toInt();
    if (local_port == 0) {
        local_port = 1080;
        guiConfig->set("local_port", local_port);
    }
    systemProxyModeManager->switchToManual(local_address, local_port);
}

bool MainWindow::startss() {
    proxyManager->stop();

    auto guiConfig = GuiConfig::instance();
    auto configs = guiConfig->getConfigs();
    auto index = guiConfig->get("index").toInt();
    if (0 == configs.size()) {
        return false;
    } else if (index > configs.size() - 1) {
        index = configs.size() - 1;
    }

    auto config = guiConfig->getConfigs()[index].toObject();
    auto useMixedProxy = guiConfig->get("useMixedProxy").toBool();
    auto localPort = guiConfig->get("localPort").toInt(1080);
    auto proxy = guiConfig->get("proxy").toObject();
    proxyManager->setConfig(config);
    proxyManager->setMixedProxy(useMixedProxy, localPort);
    proxyManager->setProxy(proxy);
    bool succeed = proxyManager->start();

    // if config parameters were corrected, save them.
    configs[index] = config;
    guiConfig->setConfigs(configs);

    return succeed;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *)
{
    qDebug()<<"right click";
}

void MainWindow::updateTrayIcon() {
    ins.append(in);
    outs.append(out);
    QString icon = "ssw";
    if (in > 0) {
        icon.append("_in");
    }
    if (out > 0) {
        icon.append("_out");
    }
    in = 0;
    out = 0;
    if (!GuiConfig::instance()->get("enabled").toBool()) {
        icon.append("_none");
    } else if (GuiConfig::instance()->get("global").toBool()) {
        icon.append("_manual");
    } else {
        icon.append("_auto");
    }
    icon.append("128.svg");
    systemTrayIcon->setIcon(QIcon(Utils::getIconQrcPath(icon)));
}

void MainWindow::on_actionEdit_Servers_triggered() {
    ConfigDialog dialog(this);
    dialog.exec();
    if (dialog.isConfigChanged()) {
        startss();
    }
    updateMenu();
}

void MainWindow::on_actionEdit_Online_PAC_URL_triggered() {
    PACUrlDialog dialog(this);
    dialog.show();
}

void MainWindow::on_actionForward_Proxy_triggered() {
    auto config = GuiConfig::instance();
    ProxyDialog dialog(this);
    dialog.exec();
    if (dialog.result() == QDialog::Accepted) {
        if (dialog.isConfigChanged()) {
            qDebug() << "Forward Proxy Changed, reloading...";
            startss();
        }
    }
}

void MainWindow::on_actionShow_Logs_triggered() {
    LogMainWindow w(this);
    w.show();
}

void MainWindow::updateMenu() {
    auto guiConfig = GuiConfig::instance();
    //    qDebug() << guiConfig->getConfigs();
    if (guiConfig->get("enabled").toBool()) {
        ui->actionEnable_System_Proxy->setChecked(true);
        ui->menuMode->setEnabled(true);
    } else {
        ui->actionEnable_System_Proxy->setChecked(false);
        ui->menuMode->setEnabled(false);
    }
    if (guiConfig->get("global").toBool()) {
        ui->actionGlobal->setChecked(true);
        ui->actionPAC->setChecked(false);
    } else {
        ui->actionGlobal->setChecked(false);
        ui->actionPAC->setChecked(true);
    }
    if (guiConfig->get("useOnlinePac").toBool()) {
        ui->actionOnline_PAC->setChecked(true);
        ui->actionLocal_PAC->setChecked(false);
    } else {
        ui->actionOnline_PAC->setChecked(false);
        ui->actionLocal_PAC->setChecked(true);
    }
    if (guiConfig->get("secureLocalPac").toBool()) {
        ui->actionSecure_Local_PAC->setChecked(true);
    } else {
        ui->actionSecure_Local_PAC->setChecked(false);
    }
    if (guiConfig->get("shareOverLan").toBool()) {
        ui->actionAllow_Clients_from_LAN->setChecked(true);
    } else {
        ui->actionAllow_Clients_from_LAN->setChecked(false);
    }
    if (guiConfig->get("isVerboseLogging").toBool()) {
        ui->actionVerbose_Logging->setChecked(true);
    } else {
        ui->actionVerbose_Logging->setChecked(false);
    }
    if (guiConfig->get("autoCheckUpdate").toBool()) {
        ui->actionCheck_for_Updates_at_Startup->setChecked(true);
    } else {
        ui->actionCheck_for_Updates_at_Startup->setChecked(false);
    }
    if (guiConfig->get("checkPreRelease").toBool()) {
        ui->actionCheck_Pre_release_Version->setChecked(true);
    } else {
        ui->actionCheck_Pre_release_Version->setChecked(false);
    }
    if(guiConfig->get("autostart").toBool()){
        ui->actionStart_on_Boot->setChecked(true);
    }else{
        ui->actionStart_on_Boot->setChecked(false);
    }
    auto configs = guiConfig->getConfigs();
    ui->menuServers->clear();
    QList<QAction *> action_list;
    action_list << ui->actionLoad_Balance
                << ui->actionHigh_Availability
                << ui->actionChoose_by_statistics;
    ui->menuServers->addActions(action_list);
    ui->menuServers->addSeparator();
    action_list.clear();
    for (int i = 0; i < configs.size(); i++) {
        auto it = configs.at(i);
        QString name = it.toObject().value("remarks").toString();
        auto action = ui->menuServers->addAction(name, [=]() {
            GuiConfig::instance()->set("index", i);
            on_actionEnable_System_Proxy_triggered(true);
        });
        action->setCheckable(true);
        if (guiConfig->get("enabled").toBool()
            && guiConfig->get("index").toInt() == i) {
            action->setChecked(true);
        }
    }
    ui->menuServers->addSeparator();
    action_list << ui->actionEdit_Servers
                << ui->actionStatistics_Config
                << ui->actionImport_from_gui_config_json
                << ui->actionExport_as_gui_config_json;
    ui->menuServers->addActions(action_list);
    ui->menuServers->addSeparator();
    ui->menuServers->addSeparator();
    action_list << ui->actionShare_Server_Config
                << ui->actionScan_QRCode_from_Screen
                << ui->actionImport_URL_from_Clipboard;
    ui->menuServers->addActions(action_list);
    ui->menuServers->addSeparator();

    ui->menuHelp->menuAction()->setVisible(false);
    ui->menuPAC->menuAction()->setVisible(false);
}

void MainWindow::on_actionImport_from_gui_config_json_triggered() {
    QString filename = QFileDialog::getOpenFileName(
          this, "choose gui-config.json file",
          QDir::homePath(),
          "gui-config.json"
    );
    if (filename.isEmpty()) {
        return;
    }
    GuiConfig::instance()->readFromDisk(filename);
    updateMenu();
}

void MainWindow::on_actionEnable_System_Proxy_triggered(bool flag) {
    auto guiConfig = GuiConfig::instance();
    if (!flag) {
        proxyManager->stop();
        systemProxyModeManager->switchToNone();
    } else {
        startss();

        if(guiConfig->get("global").toBool()) {
            switchToGlobal();
        } else {
            switchToPacMode();
        }
    }
    guiConfig->set("enabled", flag);
    updateMenu();
}

void MainWindow::on_actionPAC_triggered(bool checked) {
    qDebug() << "pac" << checked;
    auto guiConfig = GuiConfig::instance();
    if (guiConfig->get("global").toBool() == checked) {
        guiConfig->set("global", !checked);
        switchToPacMode();
    }
    updateMenu();
}

void MainWindow::on_actionGlobal_triggered(bool checked) {
    qDebug() << "global" << checked;
    auto guiConfig = GuiConfig::instance();
    if (guiConfig->get("global").toBool() != checked) {
        guiConfig->set("global", checked);
        switchToGlobal();
    }
    updateMenu();
}

void MainWindow::on_actionStart_on_Boot_triggered(bool checked) {
    // 如果使用flatpak，这里可能有问题
    QString url = "/usr/share/applications/shadowsocks-client.desktop";
    if (!checked) {
        QDBusPendingReply<bool> reply = startManagerInter.RemoveAutostart(url);
        reply.waitForFinished();
        if (!reply.isError()) {
            bool ret = reply.argumentAt(0).toBool();
            qDebug() << "remove from startup:" << ret;
        } else {
            qCritical() << reply.error().name() << reply.error().message();
            Utils::critical("change auto boot error");
        }
    } else {
        QDBusPendingReply<bool> reply = startManagerInter.AddAutostart(url);
        reply.waitForFinished();
        if (!reply.isError()) {
            bool ret = reply.argumentAt(0).toBool();
            qDebug() << "add to startup:" << ret;
        } else {
            qCritical() << reply.error().name() << reply.error().message();
            Utils::critical("change auto boot error");
        }
    }
    GuiConfig::instance()->set("autostart", checked);

}

void MainWindow::on_actionQuit_triggered() {
    qApp->exit();
}

bool MainWindow::eventFilter(QObject *, QEvent *event) {
    //    qDebug()<<event->type();
    if (event->type() == QEvent::WindowStateChange) {
        //        adjustStatusBarWidth();
    } else if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_F) {
            if (keyEvent->modifiers() == Qt::ControlModifier) {
                toolbar->focusInput();
            }
        }
    } else if (event->type() == QEvent::Close) {
    }

    return false;
}

void MainWindow::on_actionDisconnect_triggered() {
    on_actionEnable_System_Proxy_triggered(false);
}

void MainWindow::on_actionScan_QRCode_from_Screen_triggered()
{
    QString uri = QRCodeCapturer::scanEntireScreen();
    if (uri.isNull()) {
        QMessageBox::critical(
                    nullptr,
                    tr("QR Code Not Found"),
                    tr("Can't find any QR code image that contains "
                       "valid URI on your screen(s)."));
    } else {
        qDebug() << "QR scan: get uri:" << uri;
        if(uri.startsWith("ss://")){
            if(SSValidator::validate(uri)){
                GuiConfig::instance()->addConfig(uri);
                updateMenu();
                on_actionEdit_Servers_triggered();
            }else{
                Utils::info("URI is invalid");
            }
        }
    }
}

void MainWindow::on_actionImport_URL_from_Clipboard_triggered()
{
    QString uri = QApplication::clipboard()->text();
    if(SSValidator::validate(uri)){
        GuiConfig::instance()->addConfig(uri);
        updateMenu();
        on_actionEdit_Servers_triggered();
    }else{
        Utils::info("URI is invalid");
    }
}

void MainWindow::on_actionShare_Server_Config_triggered()
{
    // ? 这里肯定是有内存泄露的，但是我只要一析构，程序就崩溃了 ?
    // TODO Need More Info: the crush does not recur.
    // 生成分享的二维码
    ShareDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_actionExport_as_gui_config_json_triggered()
{
    using QSP = QStandardPaths;
    QString filename = QFileDialog::getExistingDirectory(
          nullptr,
          tr("Save gui-config.json"),
          QSP::standardLocations(QSP::DocumentsLocation).first()
    );
    if(filename.isEmpty()){
        return;
    }
    filename = filename + "/gui-config.json";
    GuiConfig::instance()->saveToDisk(filename);
    DDesktopServices::showFileItem(filename);
}
