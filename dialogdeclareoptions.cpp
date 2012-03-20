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

#include "dialogdeclareoptions.h"
#include "ui_dialogdeclareoptions.h"
#include <QSettings>

DialogDeclareOptions::DialogDeclareOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDeclareOptions)
{
    ui->setupUi(this);

    // only allow numbers in the edit boxes
    setupValidators();

    // setup the edit boxes to automatically update the generated options string
    connect(ui->alert_count,        SIGNAL(textChanged(QString)), this, SLOT(lineEditChanged(QString)));
    connect(ui->alert_repeat_gap,   SIGNAL(textChanged(QString)), this, SLOT(lineEditChanged(QString)));
    connect(ui->alert_size,         SIGNAL(textChanged(QString)), this, SLOT(lineEditChanged(QString)));
    connect(ui->auto_delete_timeout,SIGNAL(textChanged(QString)), this, SLOT(lineEditChanged(QString)));
    connect(ui->file_count,         SIGNAL(textChanged(QString)), this, SLOT(lineEditChanged(QString)));
    connect(ui->file_size,          SIGNAL(textChanged(QString)), this, SLOT(lineEditChanged(QString)));
    connect(ui->flow_resume_count,  SIGNAL(textChanged(QString)), this, SLOT(lineEditChanged(QString)));
    connect(ui->flow_resume_size,   SIGNAL(textChanged(QString)), this, SLOT(lineEditChanged(QString)));
    connect(ui->flow_stop_count,    SIGNAL(textChanged(QString)), this, SLOT(lineEditChanged(QString)));
    connect(ui->flow_stop_size,     SIGNAL(textChanged(QString)), this, SLOT(lineEditChanged(QString)));
    connect(ui->max_count,          SIGNAL(textChanged(QString)), this, SLOT(lineEditChanged(QString)));
    connect(ui->max_size,           SIGNAL(textChanged(QString)), this, SLOT(lineEditChanged(QString)));
    connect(ui->priorities,         SIGNAL(textChanged(QString)), this, SLOT(lineEditChanged(QString)));

    // setup the checkbox to update the options string
    connect(ui->last_value_queue,           SIGNAL(clicked(bool)), this, SLOT(checkboxChanged(bool)));
    connect(ui->last_value_queue_no_browse, SIGNAL(clicked(bool)), this, SLOT(checkboxChanged(bool)));
    connect(ui->optimistic_consume,         SIGNAL(clicked(bool)), this, SLOT(checkboxChanged(bool)));
    connect(ui->persist_last_node,          SIGNAL(clicked(bool)), this, SLOT(checkboxChanged(bool)));

    // setup the radio buttons to update the options string
    connect(ui->policy_type_none,         SIGNAL(clicked()), this, SLOT(radioOptionChanged()));
    connect(ui->policy_type_reject,       SIGNAL(clicked()), this, SLOT(radioOptionChanged()));
    connect(ui->policy_type_flow_to_disk, SIGNAL(clicked()), this, SLOT(radioOptionChanged()));
    connect(ui->policy_type_ring,         SIGNAL(clicked()), this, SLOT(radioOptionChanged()));
    connect(ui->policy_type_ring_strict,  SIGNAL(clicked()), this, SLOT(radioOptionChanged()));
    connect(ui->queue_event_generation_0, SIGNAL(clicked()), this, SLOT(radioOptionChanged()));
    connect(ui->queue_event_generation_1, SIGNAL(clicked()), this, SLOT(radioOptionChanged()));
    connect(ui->queue_event_generation_2, SIGNAL(clicked()), this, SLOT(radioOptionChanged()));

    // Restore must be done after the connects are setup. This is so the slots are fired when the
    // form values are restored. Thereby updating the generated options string
    restoreSettings();
}

DialogDeclareOptions::~DialogDeclareOptions()
{
    delete ui;
}

void DialogDeclareOptions::setupValidators()
{
    setupValidator(ui->alert_count);
    setupValidator(ui->alert_repeat_gap);
    setupValidator(ui->alert_size);
    setupValidator(ui->auto_delete_timeout);
    setupValidator(ui->file_count);
    setupValidator(ui->file_size);
    setupValidator(ui->flow_resume_count);
    setupValidator(ui->flow_resume_size);
    setupValidator(ui->flow_stop_count);
    setupValidator(ui->flow_stop_size);
    setupValidator(ui->max_count);
    setupValidator(ui->max_size);

    // the priorities field has a fixed range of 1 - 10
    QValidator *validator = new QIntValidator(1, 10, this);
    ui->priorities->setValidator(validator);

    // change the queue name box style if the name is empty
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(validateQueueName()));
}

// Add validators to the edit boxes.
// The maxLength property is used to store the maximum number of digits allowed
void DialogDeclareOptions::setupValidator(QLineEdit *edit)
{
    // 1 - number of digits in maxLength
    QString digits = QString("[1-9]\\d{0,%1}").arg(edit->maxLength());
    QRegExp rx(digits);
    // these will be automatically destroyed when the dialog is destroyed
    QValidator *validator = new QRegExpValidator(rx, this);
    edit->setValidator(validator);
}

// SLOT: triggered when the queue name is changed
void DialogDeclareOptions::validateQueueName()
{
    // if the queue name is empty, make the queue box red
    if (ui->lineEdit->text().isEmpty()) {
        ui->lineEdit->setStyleSheet("border: 1px solid red;color:red;font-weight:bold;");
    } else {
        ui->lineEdit->setStyleSheet("border: 1px solid black;color:black;font-weight:normal;");
    }
}

// SLOT: the OK button was pressed
void DialogDeclareOptions::accept()
{
    // there must be a queue name
    QString name = ui->lineEdit->text();
    if (name.isEmpty()) {
        ui->lineEdit->setFocus();
        return;
    }

    saveSettings();
    // tell the main window that this dialog was accepted
    emit dialogDeclareOptionsAccept();

    // hide but don't destroy this dialog so the main
    // window can retrieve the contents
    hide();;
}

// SLOT: triggered when any of the edit boxes' contents changes
// regenerate the queue options text and display it
void DialogDeclareOptions::lineEditChanged(const QString& value)
{
    QLineEdit *edit = (QLineEdit *)sender();
    QGroupBox *parentGroupBox = (QGroupBox *)edit->parent();
    QString defaultValue = parentGroupBox->statusTip();
    QString option = edit->objectName();
    if ((value == defaultValue) || (value.isEmpty())) {
        ui->textEditOptions->clearOption(option);
    } else {
        ui->textEditOptions->setOption(option, value);
    }
    ui->textEditOptions->regenText();

}

// SLOT: triggered when any of the radio buttons change
// regenerate the queue options text and display it
void DialogDeclareOptions::radioOptionChanged()
{
    QRadioButton *radio = (QRadioButton *)sender();
    QGroupBox *parentGroupBox = (QGroupBox *)radio->parent();
    QString name = parentGroupBox->statusTip();
    QString value = radio->statusTip();
    if (value.isEmpty()) {
        ui->textEditOptions->clearOption(name);
    } else {
        ui->textEditOptions->setOption(name, value);
    }
    ui->textEditOptions->regenText();
}

// SLOT: triggered when any of the check boxes change
// regenerate the queue options text and display it
void DialogDeclareOptions::checkboxChanged(bool checked)
{
    QCheckBox *check = (QCheckBox *)sender();
    QString option = check->objectName();
    if (checked) {
        ui->textEditOptions->setOption(option, "1");
    } else {
        ui->textEditOptions->clearOption(option);
    }
    ui->textEditOptions->regenText();

}

// return the queue name
QString DialogDeclareOptions::name()
{
    return ui->lineEdit->text();
}

// return the generated queue options
QString DialogDeclareOptions::args()
{
    return ui->textEditOptions->constructString();
}

// persist the queue options
void DialogDeclareOptions::saveSettings() {
    QSettings settings;

    settings.beginGroup("DeclareQueue");

    settings.setValue("alert_count", ui->alert_count->text());
    settings.setValue("alert_repeat_gap", ui->alert_repeat_gap->text());
    settings.setValue("alert_size", ui->alert_size->text());
    settings.setValue("auto_delete_timeout", ui->auto_delete_timeout->text());
    settings.setValue("file_count", ui->file_count->text());
    settings.setValue("file_size", ui->file_size->text());
    settings.setValue("flow_resume_count", ui->flow_resume_count->text());
    settings.setValue("flow_resume_size", ui->flow_resume_size->text());
    settings.setValue("flow_stop_count", ui->flow_stop_count->text());
    settings.setValue("flow_stop_size", ui->flow_stop_size->text());
    settings.setValue("max_count", ui->max_count->text());
    settings.setValue("max_size", ui->max_size->text());
    settings.setValue("priorities", ui->priorities->text());

    settings.setValue("last_value_queue", ui->last_value_queue->isChecked());
    settings.setValue("last_value_queue_no_browse", ui->last_value_queue_no_browse->isChecked());
    settings.setValue("optimistic_consume", ui->optimistic_consume->isChecked());
    settings.setValue("persist_last_node", ui->persist_last_node->isChecked());

    settings.setValue("policy_type_none", ui->policy_type_none->isChecked());
    settings.setValue("policy_type_reject", ui->policy_type_reject->isChecked());
    settings.setValue("policy_type_flow_to_disk", ui->policy_type_flow_to_disk->isChecked());
    settings.setValue("policy_type_ring", ui->policy_type_ring->isChecked());
    settings.setValue("policy_type_ring_strict", ui->policy_type_ring_strict->isChecked());
    settings.setValue("queue_event_generation_0", ui->queue_event_generation_0->isChecked());
    settings.setValue("queue_event_generation_1", ui->queue_event_generation_1->isChecked());
    settings.setValue("queue_event_generation_2", ui->queue_event_generation_2->isChecked());

    settings.endGroup();

}

// read the queue options from the store and initialize the form controls
void DialogDeclareOptions::restoreSettings() {
    QSettings settings;

    settings.beginGroup("DeclareQueue");

    ui->alert_count->setText(settings.value("alert_count", ui->alert_count->text()).toString());
    ui->alert_repeat_gap->setText(settings.value("alert_repeat_gap", ui->alert_repeat_gap->text()).toString());
    ui->alert_size->setText(settings.value("alert_size", ui->alert_size->text()).toString());
    ui->auto_delete_timeout->setText(settings.value("auto_delete_timeout", ui->auto_delete_timeout->text()).toString());
    ui->file_count->setText(settings.value("file_count", ui->file_count->text()).toString());
    ui->file_size->setText(settings.value("file_size", ui->file_size->text()).toString());
    ui->flow_resume_count->setText(settings.value("flow_resume_count", ui->flow_resume_count->text()).toString());
    ui->flow_resume_size->setText(settings.value("flow_resume_size", ui->flow_resume_size->text()).toString());
    ui->flow_stop_count->setText(settings.value("flow_stop_count", ui->flow_stop_count->text()).toString());
    ui->flow_stop_size->setText(settings.value("flow_stop_size", ui->flow_stop_size->text()).toString());
    ui->max_count->setText(settings.value("max_count", ui->max_count->text()).toString());
    ui->max_size->setText(settings.value("max_size", ui->max_size->text()).toString());
    ui->priorities->setText(settings.value("priorities", ui->priorities->text()).toString());


    ui->last_value_queue->setChecked(settings.value("last_value_queue",
                                                    ui->last_value_queue->isChecked()).toBool());
    ui->last_value_queue_no_browse->setChecked(settings.value("last_value_queue_no_browse",
                                                              ui->last_value_queue_no_browse->isChecked()).toBool());
    ui->optimistic_consume->setChecked(settings.value("optimistic_consume", ui->optimistic_consume->isChecked()).toBool());
    ui->persist_last_node->setChecked(settings.value("persist_last_node",
                                                     ui->persist_last_node->isChecked()).toBool());

    ui->policy_type_none->setChecked(settings.value("policy_type_none",
                                                    ui->policy_type_none->isChecked()).toBool());
    ui->policy_type_reject->setChecked(settings.value("policy_type_reject",
                                                      ui->policy_type_reject->isChecked()).toBool());
    ui->policy_type_flow_to_disk->setChecked(settings.value("policy_type_flow_to_disk",
                                                            ui->policy_type_flow_to_disk->isChecked()).toBool());
    ui->policy_type_ring->setChecked(settings.value("policy_type_ring",
                                                    ui->policy_type_ring->isChecked()).toBool());
    ui->policy_type_ring_strict->setChecked(settings.value("policy_type_ring_strict",
                                                           ui->policy_type_ring_strict->isChecked()).toBool());
    ui->queue_event_generation_0->setChecked(settings.value("queue_event_generation_0",
                                                            ui->queue_event_generation_0->isChecked()).toBool());
    ui->queue_event_generation_1->setChecked(settings.value("queue_event_generation_1",
                                                            ui->queue_event_generation_1->isChecked()).toBool());
    ui->queue_event_generation_2->setChecked(settings.value("queue_event_generation_2",
                                                            ui->queue_event_generation_2->isChecked()).toBool());

    settings.endGroup();

}
