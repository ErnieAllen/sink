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

#include "sinkcomponent.h"
#include "ui_sinkcomponent.h"

SinkComponent::SinkComponent(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SinkComponent),
    alarm(this),
    count(0)
{
    ui->setupUi(this);

    ui->comboBox->addItem("Seconds", QVariant(1));
    ui->comboBox->addItem("Minutes", QVariant(60));
    ui->buttonStart->setIcon(QIcon::fromTheme("media-playback-start"));

    setEveryLabel(ui->spinAmount->value());
    setDurationLabel(ui->spinSeconds->value());

    label_count = new QLabel();
    label_prompt = new QLabel();
    ui->statusBar->addWidget(label_prompt);
    ui->statusBar->addWidget(label_count);

    connect(ui->spinAmount, SIGNAL(valueChanged(int)), ui->dialAmount, SLOT(setValue(int)));
    connect(ui->spinSeconds, SIGNAL(valueChanged(int)), ui->dialSeconds, SLOT(setValue(int)));
    connect(ui->dialAmount, SIGNAL(valueChanged(int)), ui->spinAmount, SLOT(setValue(int)));
    connect(ui->dialSeconds, SIGNAL(valueChanged(int)), ui->spinSeconds, SLOT(setValue(int)));

    connect(ui->spinAmount, SIGNAL(valueChanged(int)), this, SLOT(setEveryLabel(int)));
    connect(ui->spinSeconds, SIGNAL(valueChanged(int)), this, SLOT(setDurationLabel(int)));

    connect(ui->buttonStart, SIGNAL(toggled(bool)), this, SLOT(buttonToggled(bool)));
    connect(&alarm, SIGNAL(timeout()), this, SLOT(strobe()));

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(resetTimer()));
}

SinkComponent::~SinkComponent()
{
    delete label_prompt;
    delete label_count;
    delete ui;
}

void SinkComponent::setTitle(const QString &title, const QString & countPrompt)
{
    ui->groupBox->setTitle(title);
    ui->labelVerb->setText(title);
    label_prompt->setText(countPrompt);
}


// SLOT triggered when the amout changes
// If the amount is 1, change the text to singular
void SinkComponent::setEveryLabel(int i)
{
    if (i == 1) {
        ui->labelEvery->setText(tr("message every"));
    } else {
        ui->labelEvery->setText(tr("messages every"));
    }
}

// SLOT triggered when the duration changes
// If the amount is 1, change the text to singular
void SinkComponent::setDurationLabel(int i)
{
    if (i == 1) {
        ui->comboBox->clear();
        ui->comboBox->addItem("Second", QVariant(1));
        ui->comboBox->addItem("Minute", QVariant(60));
    } else {
        ui->comboBox->clear();
        ui->comboBox->addItem("Seconds", QVariant(1));
        ui->comboBox->addItem("Minutes", QVariant(60));
    }
    if (ui->buttonStart->isChecked())
        resetTimer();
}

void SinkComponent::resetTimer()
{
    this->alarm.stop();
    int row = ui->comboBox->currentIndex();
    if (row < 0)
        row = 0;
    int secs = ui->comboBox->itemData(row).toInt();
    secs *= ui->spinSeconds->value();
    // fire timer handler immediately
    strobe();
    // also set timer to fire the handler every second
    alarm.start(secs * 1000);
}

void SinkComponent::stopTimer()
{
    ui->buttonStart->setChecked(false);
    alarm.stop();
    label_count->setText("");
}


void SinkComponent::buttonToggled(bool b)
{
    if (ui->buttonStart->isChecked()) {
        ui->buttonStart->setIcon(QIcon::fromTheme("media-playback-stop"));
        ui->buttonStart->setText(tr("Stop"));
        resetTimer();
    } else {
        ui->buttonStart->setIcon(QIcon::fromTheme("media-playback-start"));
        ui->buttonStart->setText(tr("Start"));
        this->alarm.stop();
    }
    emit stateChanged(b);
}

// SLOT triggered when the internal timer goes off
// Send a signal to send/drain the request number of messages
void SinkComponent::strobe()
{
    // the button should be checked when get a timer event, but just in case
    if (ui->buttonStart->isChecked()) {
        // tell the app to send/drain this number of messages
        emit tick(ui->spinAmount->value());
    }
}

void SinkComponent::counted(int count)
{
    label_count->setText(QString::number(count));
}
