#include "stdafx.h"
#include "utils.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ConfigDialog.h"
#include "PACUrlDialog.h"
#include "ProxyDialog.h"
#include "LogMainWindow.h"
#include "DDEProxyModeManager.h"
#include "QRCaptor.h"
#include "ShareDialog.h"
#include "GConfig.h"
#include <DDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
    DMainWindow(parent),
    ui(new Ui::MainWindow),
    m_sysTray(new QSystemTrayIcon),
    m_controller(new SController(this)),
    m_sysProxyModeMgr(new DDEProxyModeManager(this)),
    m_timer(new QTimer(this)) {

    ui->setupUi(this);
    installEventFilter(this);

    const auto &gConfig = GConfig::instance();

    m_sysTray->setContextMenu(ui->menuTray);
    m_sysTray->setIcon(QIcon(Utils::getIconQrcPath("ss16.png")));
    m_sysTray->show();

    m_sysProxyModeMgr = new DDEProxyModeManager(this);

    if (gConfig->enabled()) {
      emit ui->actionEnable_System_Proxy->triggered(true);
    }
    auto autoStart = gConfig->autoStart();
    emit ui->actionStart_on_Boot->triggered(autoStart);

    connect(m_timer, &QTimer::timeout, this, &MainWindow::updateTrayIcon);
    m_timer->start(100);

//    // 流量监控
//    connect(proxyManager, &ProxyManager::bytesReceivedChanged, [=](quint64 n) {
//        qDebug() << "bytesReceivedChanged" << n;
//        term_usage_in = n;
//    });
//    connect(proxyManager, &ProxyManager::bytesSentChanged, [=](quint64 n) {
//        qDebug() << "bytesSentChanged" << n;
//        term_usage_out = n;
//    });
//    connect(proxyManager, &ProxyManager::newBytesReceived, [=](quint64 n) {
//        qDebug() << "newBytesReceived" << n;
//        in += n;
//    });
//    connect(proxyManager, &ProxyManager::newBytesSent, [=](quint64 n) {
//        qDebug() << "newBytesSent" << n;
//        out += n;
//    });

    updateMenu();
}

MainWindow::~MainWindow() {
    delete m_controller;
    delete m_sysProxyModeMgr;
    delete m_sysTray;
    delete m_timer;
    delete ui;
}

void MainWindow::updateMenu() {
    const auto gConfig = GConfig::instance();
    QList<QAction *> actionList;

    auto enabled = gConfig->enabled();
    ui->actionEnable_System_Proxy->setChecked(enabled);
    ui->menuMode->setEnabled(true);

    auto global = gConfig->global();
    ui->actionGlobal->setChecked(global);
    ui->actionPAC->setChecked(!global);

    auto useOnlinePac = gConfig->useOnlinePac();
    ui->actionOnline_PAC->setChecked(useOnlinePac);
    ui->actionLocal_PAC->setChecked(!useOnlinePac);

    // protect local pac file
    auto secureLocalPac = gConfig->secureLocalPac();
    ui->actionSecure_Local_PAC->setChecked(secureLocalPac);

    auto shareOverLan = gConfig->shareOverLan();
    ui->actionAllow_Clients_from_LAN->setChecked(shareOverLan);

    auto isVerboseLogging = gConfig->isVerboseLogging();
    ui->actionVerbose_Logging->setChecked(isVerboseLogging);

    auto autoCheckUpdate = gConfig->autoCheckUpdate();
    ui->actionCheck_for_Updates_at_Startup->setChecked(autoCheckUpdate);

    auto checkPreRelease = gConfig->checkPreRelease();
    ui->actionCheck_Pre_release_Version->setChecked(checkPreRelease);

    auto autoStart = gConfig->autoStart();
    ui->actionStart_on_Boot->setChecked(autoStart);

    ui->menuServers->clear();
    actionList << ui->actionLoad_Balance
               << ui->actionHigh_Availability
               << ui->actionChoose_by_statistics;
    ui->menuServers->addActions(actionList);
    actionList.clear();

    ui->menuServers->addSeparator();

    auto remarks = gConfig->remarks();
    for (int i = 0; i < remarks.size(); i++) {
        auto action = ui->menuServers->addAction(remarks[i], [=]() {
            gConfig->index(i);
            on_actionEnable_System_Proxy_triggered(true);
        });
        action->setCheckable(true);
        if (gConfig->enabled() && gConfig->index() == i) {
            action->setChecked(true);
        }
    }
    actionList.clear();

    ui->menuServers->addSeparator();

    actionList << ui->actionEdit_Servers
               << ui->actionStatistics_Config
               << ui->actionImport_from_gui_config_json
               << ui->actionExport_as_gui_config_json;
    ui->menuServers->addActions(actionList);
    actionList.clear();

    ui->menuServers->addSeparator();

    actionList << ui->actionShare_Server_Config
               << ui->actionScan_QRCode_from_Screen
               << ui->actionImport_URL_from_Clipboard;
    ui->menuServers->addActions(actionList);
    actionList.clear();

    ui->menuServers->addSeparator();

    ui->menuHelp->menuAction()->setVisible(true);
    ui->menuPAC->menuAction()->setVisible(false);
}

void MainWindow::switchToPacMode() {
    const auto guiConfig = GConfig::instance();
    QString online_pac_uri = "http://file.lolimay.cn/autoproxy.pac";
    QString pacUri = "";
    if (guiConfig->useOnlinePac()) {
        pacUri = guiConfig->pacUrl();
        if (pacUri.isEmpty()) {
            Utils::warning("online pac uri is empty. we will use default uri.");
            pacUri = online_pac_uri;
            guiConfig->pacUrl(pacUri);
        }
    } else {
        QString pac_file = QDir(Utils::configPath()).filePath("autoproxy.pac");
        QFile file(pac_file);
        if (!file.exists()) {
            Utils::warning("local pac is not exist. we will use on pac file. "
                           "you can change it");
            pacUri = online_pac_uri;
            guiConfig->pacUrl(pacUri);
            guiConfig->useOnlinePac(true);
        } else {
            pacUri = "file://" + pac_file;
        }
    }
    m_sysProxyModeMgr->switchToAuto(pacUri);
}

void MainWindow::switchToGlobal() {
    const auto gConfig = GConfig::instance();
    uint16_t localPort = gConfig->localPort();
    if (localPort == 0) {
        localPort = 1080;
        gConfig->localPort(localPort);
    }
    m_sysProxyModeMgr->switchToManual("localhost", localPort);
}

bool MainWindow::start() {
    return m_controller->start();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *) {
    qDebug() << "right click";
}

void MainWindow::updateTrayIcon() {
    QString icon = "ssw";
    if (in > 0) {
        icon.append("_in");
        in = 0;
    }
    if (out > 0) {
        icon.append("_out");
        out = 0;
    }
    if (!GConfig::instance()->enabled()) {
        icon.append("_none");
    } else if (GConfig::instance()->global()) {
        icon.append("_manual");
    } else {
        icon.append("_auto");
    }
    icon.append("128.svg");
    m_sysTray->setIcon(QIcon(Utils::getIconQrcPath(icon)));
}

void MainWindow::on_actionEdit_Servers_triggered() {
    ConfigDialog dialog(this);
    dialog.exec();
    if (dialog.isConfigChanged()) {
        qDebug() << "Server Configuration Changed, reloading...";
        start();
    }
    updateMenu();
}

void MainWindow::on_actionEdit_Online_PAC_URL_triggered() {
    PACUrlDialog dialog(this);
    dialog.show();
}

void MainWindow::on_actionForward_Proxy_triggered() {
    ProxyDialog dialog(this);
    dialog.exec();
    if (dialog.isConfigChanged()) {
        qDebug() << "Forward Proxy Changed, reloading...";
        start();
    }
}

void MainWindow::on_actionShow_Logs_triggered() {
    LogMainWindow w(this);
    w.show();
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
    GConfig::instance()->load(filename);
    updateMenu();
}

void MainWindow::on_actionEnable_System_Proxy_triggered(bool flag) {
    const auto gConfig = GConfig::instance();
    if (!flag) {
        m_controller->stop();
        m_sysProxyModeMgr->switchToNone();
    } else {
        start();

        if(gConfig->global()) {
            switchToGlobal();
        } else {
            switchToPacMode();
        }
    }
    gConfig->enabled(flag);
    updateMenu();
}

void MainWindow::on_actionPAC_triggered(bool checked) {
    qDebug() << "on_pac " << checked;
    if (GConfig::instance()->global() == checked) {
        GConfig::instance()->global(!checked);
        switchToPacMode();
    }
    updateMenu();
}

void MainWindow::on_actionGlobal_triggered(bool checked) {
    qDebug() << "on_global " << checked;
    if (GConfig::instance()->global() != checked) {
        GConfig::instance()->global(checked);
        switchToGlobal();
    }
    updateMenu();
}

void MainWindow::on_actionStart_on_Boot_triggered(bool checked) {
    // 如果使用flatpak，这里可能有问题
    QString url = "/usr/share/applications/dpss.desktop";
    if (!checked) {
        QDBusPendingReply<bool> reply = startManagerInter.RemoveAutostart(url);
        reply.waitForFinished();
        if (!reply.isError()) {
            bool ret = reply.argumentAt(0).toBool();
            qDebug() << "remove from startup:" << ret;
        } else {
            qCritical() << reply.error().name() << reply.error().message();
        }
    } else {
        QDBusPendingReply<bool> reply = startManagerInter.AddAutostart(url);
        reply.waitForFinished();
        if (!reply.isError()) {
            bool ret = reply.argumentAt(0).toBool();
            qDebug() << "add to startup:" << ret;
        } else {
            qCritical() << reply.error().name() << reply.error().message();
        }
    }
    GConfig::instance()->autoStart(checked);
}

void MainWindow::on_actionQuit_triggered() {
    qApp->exit();
}

bool MainWindow::eventFilter(QObject *, QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
    } else if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = dynamic_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_F) {
            if (keyEvent->modifiers() == Qt::ControlModifier) {
            }
        }
    } else if (event->type() == QEvent::Close) {
    }
    return false;
}

void MainWindow::on_actionDisconnect_triggered() {
    emit ui->actionEnable_System_Proxy->triggered(false);
}

void MainWindow::on_actionScan_QRCode_from_Screen_triggered() {
    QString uri = QRCaptor::scanEntireScreen();
    if (uri.isNull()) {
        QMessageBox::critical(
          this,
          tr("QR Code Not Found"),
          tr("Can't find any QR code image that contains valid URI "
             "on your screen(s)."));
    } else {
        qDebug() << "QR scan: get uri: " << uri;
        if (GConfig::instance()->addServer(uri)) {
            updateMenu();
            emit ui->actionEdit_Servers->triggered();
        } else {
            Utils::info(tr("Uri from screen is invalid"));
        }
    }
}


void MainWindow::on_actionImport_URL_from_Clipboard_triggered() {
    QString uri = QApplication::clipboard()->text();
    bool isGood = GConfig::instance()->addServer(uri);
    if (isGood) {
        updateMenu();
        emit ui->actionEdit_Servers->triggered();
    } else {
        Utils::info(tr("Uri from clipboard is invalid."));
    }
}

void MainWindow::on_actionShare_Server_Config_triggered() {
    ShareDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_actionExport_as_gui_config_json_triggered() {
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
    GConfig::instance()->save(filename);
    DDesktopServices::showFileItem(filename);
}
