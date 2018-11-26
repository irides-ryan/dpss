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

#pragma once

#include <QMainWindow>

class QRCaptor : public QMainWindow {
Q_OBJECT

public:
  explicit QRCaptor(QWidget *parent = nullptr);
  ~QRCaptor() override;

  static QString scanEntireScreen();

signals:
  void qrCodeFound(const QString &result);
  void closed();

protected slots:
  void moveEvent(QMoveEvent *e) override;
  void resizeEvent(QResizeEvent *e) override;
  void closeEvent(QCloseEvent *e) override;

private:
  void decodeCurrentRegion();
  static QString decodeImage(const QImage &image);
};
