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

#ifndef PROXYDIALOG_H
#define PROXYDIALOG_H

#include <QDialog>
#include <config/Proxy.h>
#include "stdafx.h"

namespace Ui {
class ProxyDialog;
}

class ProxyDialog : public QDialog {
    Q_OBJECT

public:
    explicit ProxyDialog(QWidget *parent = nullptr);
    ~ProxyDialog() override;

    bool isConfigChanged();

private slots:
    void on_checkBoxUseProxy_stateChanged(int state);
    void on_buttonBox_accepted();

private:
    Ui::ProxyDialog *ui;
    QJsonObject m_confProxy;
    QSX::Proxy m_proxy;
    bool m_isConfigChanged = false;
};

#endif // PROXYDIALOG_H
