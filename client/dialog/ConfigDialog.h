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

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include "stdafx.h"

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr);
    ~ConfigDialog();
    bool isConfigChanged();


private:
    Ui::ConfigDialog *ui;
    QJsonArray m_configs;
    int m_currentPos;
    bool m_isConfigChanged;

    void save();
    void stashConfig(int index);
    void showConfig(int index);

private slots:
    void on_pushButtonAdd_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonDuplicate_clicked();
    void on_pushButtonMoveUp_clicked();
    void on_pushButtonMoveDown_clicked();
    void on_pushButtonOK_clicked();
    void on_lineEditRemarks_textEdited(const QString &arg1);
    void on_listWidget_currentRowChanged(int currentRow);
};

#endif // CONFIGDIALOG_H
