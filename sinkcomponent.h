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

#ifndef SINKCOMPONENT_H
#define SINKCOMPONENT_H

#include <QWidget>
#include <QLabel>
#include <QTimer>

namespace Ui {
    class SinkComponent;
}

class SinkComponent : public QWidget
{
    Q_OBJECT

public:
    explicit SinkComponent(QWidget *parent = 0);
    ~SinkComponent();

    void setTitle(const QString & title, const QString & countPrompt);

signals:
    void stateChanged(bool);
    void tick(int);

public slots:
    void counted(int);
    void stopTimer();

private slots:
    void setEveryLabel(int i);
    void setDurationLabel(int i);
    void buttonToggled(bool);
    void strobe();
    void resetTimer();

private:
    Ui::SinkComponent *ui;
    QTimer alarm;
    ulong count;

    QLabel *label_count;
    QLabel *label_prompt;

};

#endif // SINKCOMPONENT_H
