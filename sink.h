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

#ifndef SINK_H
#define SINK_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <deque>

#include "msg_tool.h"
#include <qpid/messaging/Session.h>

class Sink : public QThread
{
    Q_OBJECT
public:
    explicit Sink(QObject *parent = 0);

    void cancel();
    void createQueue(qpid::messaging::Session& session, const QString &name, QueueType qType);

protected:
    void run();

private:
    mutable QMutex lock;
    QWaitCondition cond;
    bool cancelled;

    msg_tool_queue*  msg_tool_q;

    struct Command {
        bool spout;
        int count;

        Command(bool _b, int _c) :
            spout(_b), count(_c) {}
    };
    typedef std::deque<Command> command_queue_t;
    command_queue_t command_queue;


    void send(int count);
    void receive(int count);

signals:
    void spouted(int);
    void drained(int);

public slots:
    void spout(int count);
    void drain(int count);
    void destroyQueue();

};

#endif // SINK_H
