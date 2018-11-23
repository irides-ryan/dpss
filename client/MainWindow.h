/**
 * Copyright (C) 2017 ~ 2018 PikachuHy
 *
 * Author:     PikachuHy <pikachuhy@163.com>
 * Maintainer: PikachuHy <pikachuhy@163.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "stdafx.h"
#include "ProxyManager.h"
#include "interface/SystemProxyModeManager.h"
#include "dbusinterface/DBusStartManager.h"
#include "GuiConfig.h"
#include "Settings.h"
#include "Toolbar.h"
#include "widget/ProfileView.h"
#include "scontroller.h"

DWIDGET_USE_NAMESPACE
using StartManagerInter = com::deepin::StartManager;
namespace Ui {
class MainWindow;
}

class MainWindow : public DMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    bool eventFilter(QObject *, QEvent *) override ;

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *m_sysTray;

    SController *m_controller;
    SystemProxyModeManager *m_sysProxyModeMgr;
    StartManagerInter startManagerInter;
    QTimer *m_timer;

    quint64 in = 0;
    quint64 out = 0;

    void updateMenu();
    void switchToPacMode();
    void switchToGlobal();
    bool start();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void on_actionEdit_Servers_triggered();
    void on_actionEdit_Online_PAC_URL_triggered();
    void on_actionForward_Proxy_triggered();
    void on_actionShow_Logs_triggered();
    void on_actionImport_from_gui_config_json_triggered();
    void on_actionEnable_System_Proxy_triggered(bool flag);
    void on_actionPAC_triggered(bool checked);
    void on_actionGlobal_triggered(bool checked);
    void updateTrayIcon();
    void on_actionStart_on_Boot_triggered(bool checked);
    void on_actionQuit_triggered();
    void on_actionDisconnect_triggered();
    void on_actionScan_QRCode_from_Screen_triggered();
    void on_actionImport_URL_from_Clipboard_triggered();
    void on_actionShare_Server_Config_triggered();
    void on_actionExport_as_gui_config_json_triggered();
};

#endif // MAINWINDOW_H
