/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "qmf-thread.h"
#include "sink.h"
#include "dialogopen.h"
#include "dialogobjects.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QmfThread* qmf;
    Sink *sink;
    QLabel *label_connection_prompt;
    QLabel *label_connection_status;

    DialogOpen*      openDialog;
    DialogObjects*   queuesDialog;

    void setupStatusBar();

signals:
    void stopTimer();
    void initSink();

private slots:
    void dispatchResponse(QObject *target, const qmf::ConsoleEvent& event);
    void queryQueues();
    void setCurrentObject(const qmf::Data &queue);
    void createQueue();

};

#endif // MAINWINDOW_H
