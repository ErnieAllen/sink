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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //
    // Setup some global app vales to be used by the QSettings class
    //
    QCoreApplication::setOrganizationName("Red Hat");
    QCoreApplication::setOrganizationDomain("redhat.com");
    QCoreApplication::setApplicationName("qpid-xsink");

    // allow qmf types to be passed in signals
    qRegisterMetaType<qmf::Data>();
    qRegisterMetaType<qmf::ConsoleEvent>();

    ui->setupUi(this);

    //
    // Restore the window size and location and menu check boxes
    //
    QSettings settings;
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    restoreState(settings.value("mainWindowState").toByteArray());

    ui->componentSpout->setTitle("Spout", "Sent:");
    ui->componentDrain->setTitle("Drain", "Drained:");
    ui->action_Choose_existing_queue->setIcon(QIcon(":/images/queue-24.png"));
    ui->action_Declare_new_queue->setIcon(QIcon(":/images/queue-24-new.png"));

    //
    // Create the thread object that maintains communication with the messaging plane.
    //
    qmf = new QmfThread(this);
    sink = new Sink(this);

    setupStatusBar();
    connect(qmf, SIGNAL(connectionStatusChanged(QString)), label_connection_status, SLOT(setText(QString)));
    connect(qmf, SIGNAL(receivedResponse(QObject*,qmf::ConsoleEvent)), this, SLOT(dispatchResponse(QObject*,qmf::ConsoleEvent)));

    // menu actions to open and close the broker connection
    connect(ui->actionOpen_localhost, SIGNAL(triggered()), qmf,           SLOT(connect_localhost()));
    connect(ui->actionClose, SIGNAL(triggered()), ui->componentSpout, SLOT(stopTimer()));
    connect(ui->actionClose, SIGNAL(triggered()), ui->componentDrain, SLOT(stopTimer()));
    connect(ui->actionClose, SIGNAL(triggered()), qmf,                    SLOT(disconnect()));
    connect(ui->actionClose, SIGNAL(triggered()), sink,                   SLOT(destroyQueue()));
    connect(ui->actionClose, SIGNAL(triggered()), ui->labelQueue,         SLOT(clear()));

    //
    // Create linkages to enable and disable main-window components based on the connection status.
    //
    connect(qmf, SIGNAL(isConnected(bool)), ui->actionOpen_localhost,   SLOT(setDisabled(bool)));
    connect(qmf, SIGNAL(isConnected(bool)), ui->actionOpen_URL,         SLOT(setDisabled(bool)));
    connect(qmf, SIGNAL(isConnected(bool)), ui->actionClose,            SLOT(setEnabled(bool)));
    connect(qmf, SIGNAL(isConnected(bool)), ui->mainToolBar,            SLOT(setEnabled(bool)));
    connect(qmf, SIGNAL(disconnected(bool)), ui->centralWidget,         SLOT(setDisabled(bool)));
    connect(qmf, SIGNAL(isConnected(bool)), ui->menu_Spout,             SLOT(setEnabled(bool)));

    //
    // Create the dialog boxes
    //
    openDialog = new DialogOpen(this);
    // when the menu item is selected, show the dialog
    connect(ui->actionOpen_URL, SIGNAL(triggered()), openDialog, SLOT(show()));
    // when the dialog is accepted, open the URL
    connect(openDialog, SIGNAL(dialogOpenAccepted(QString,QString,QString)), qmf, SLOT(connect_url(QString,QString,QString)));

    /*
    declareQueueDialog = new DialogDeclareQueue(this);
    connect(ui->action_Declare_new_queue, SIGNAL(triggered()), declareQueueDialog, SLOT(show()));
    connect(declareQueueDialog, SIGNAL(dialogDeclareQueueAccept()), this, SLOT(declareQueue()));
*/
    declareOptionsDialog = new DialogDeclareOptions(this);
    connect(ui->action_Declare_new_queue, SIGNAL(triggered()), declareOptionsDialog, SLOT(show()));
    connect(declareOptionsDialog, SIGNAL(dialogDeclareOptionsAccept()), this, SLOT(declareQueue()));

    ui->centralWidget->setEnabled(false);
    ui->mainToolBar->setEnabled(false);

    queuesDialog = new DialogObjects(this, "queues");
    queuesDialog->initModels("name");

    connect(ui->action_Choose_existing_queue, SIGNAL(triggered()), this, SLOT(queryQueues()));
    connect(ui->action_Choose_existing_queue, SIGNAL(triggered()), queuesDialog, SLOT(exec()));
    connect(queuesDialog, SIGNAL(setCurrentObject(qmf::Data,QString)),
            this, SLOT(setCurrentObject(qmf::Data)));

    // connections between the spout/drain timers and the sink
    connect(ui->componentSpout, SIGNAL(tick(int)), sink, SLOT(spout(int)));
    connect(ui->componentDrain, SIGNAL(tick(int)), sink, SLOT(drain(int)));

    connect(sink, SIGNAL(spouted(int)), ui->componentSpout, SLOT(counted(int)));
    connect(sink, SIGNAL(drained(int)), ui->componentDrain, SLOT(counted(int)));

    connect(this, SIGNAL(stopTimer()), ui->componentDrain, SLOT(stopTimer()));
    connect(this, SIGNAL(stopTimer()), ui->componentSpout, SLOT(stopTimer()));
    connect(this, SIGNAL(initSink()), this, SLOT(createQueue()));

    qmf->start();
    sink->start();
}

// Display the connection status in the status bar
void MainWindow::setupStatusBar() {
    label_connection_status = new QLabel();
    label_connection_prompt = new QLabel();
    label_connection_prompt->setText("Connection status: ");
    statusBar()->addWidget(label_connection_prompt);
    statusBar()->addWidget(label_connection_status);
}

// SLOT: triggered when Queues Dialog is displayed
// Send an async query to get the list of queues
// When the response is received, send an event to the queues dialog
void MainWindow::queryQueues()
{
    qmf->queryBroker("queue", queuesDialog);
}

// SLOT: Triggered when a qmf query response is received
// Send the received event over to the appropriate dialog box
void MainWindow::dispatchResponse(QObject *target, const qmf::ConsoleEvent& event)
{
    DialogObjects *dialog = (DialogObjects *)target;
    dialog->gotDataEvent(event);
}

// SLOT: Triggered when the existing queue dialog is accepted
// Set the queue name on the screen and create a msg_tool_q object
void MainWindow::setCurrentObject(const qmf::Data &q)
{
    std::string name = q.getProperty("name").asString();
    ui->centralWidget->setEnabled(true);
    ui->labelQueue->setText(QString(name.c_str()));

    emit stopTimer();
    emit initSink();
}

void MainWindow::setCurrentQueue(const QString& name)
{
    ui->centralWidget->setEnabled(true);
    ui->labelQueue->setText(name);

    emit stopTimer();
    emit initSink();
}

void MainWindow::createQueue()
{
    QString name = ui->labelQueue->text();
    sink->createQueue(qmf->session(), name);
}

// SLOT: Triggered when the declare new queue dialog is accepted.
// Creates a new queue with the arguments on the dialog box
void MainWindow::declareQueue()
{

    // get the queue name, this has already been vetted
    QString name = declareOptionsDialog->name();

    // declare the queue
    sink->createQueue(qmf->session(), name, declareOptionsDialog->args().toStdString().c_str());
    setCurrentQueue(name);

}

MainWindow::~MainWindow()
{
    // save the window size and location
    QSettings settings;
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());

    delete openDialog;
    delete queuesDialog;
    delete label_connection_status;
    delete label_connection_prompt;

    qmf->cancel();
    qmf->wait();
    delete qmf;

    sink->cancel();
    sink->wait();
    delete sink;

    delete ui;
}
