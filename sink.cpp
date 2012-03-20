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

#include "sink.h"
#include <QCoreApplication>

Sink::Sink(QObject *parent) :
    QThread(parent),
    cancelled(false),
    msg_tool_q()
{
}


void Sink::cancel()
{
    cancelled = true;
}

void Sink::spout(int count)
{
    this->send(count);
    return;
    QMutexLocker locker(&lock);
    command_queue.push_back(Command(true, count));
    cond.wakeOne();
}

void Sink::drain(int count)
{
    this->receive(count);
    return;
    QMutexLocker locker(&lock);
    command_queue.push_back(Command(false, count));
    cond.wakeOne();
}

void Sink::createQueue(qpid::messaging::Session& session, const QString &name, const char *args)
{
    QMutexLocker locker(&lock);
    if (msg_tool_q)
        delete msg_tool_q;

    msg_tool_q = new msg_tool_queue ( session, name.toStdString().c_str(), args );
    cond.wakeOne();
}

void Sink::destroyQueue()
{
    QMutexLocker locker(&lock);
    if (msg_tool_q) {
        delete msg_tool_q;
        msg_tool_q = NULL;
    }
    cond.wakeOne();
}

void Sink::run()
{
    while(true) {
        if (msg_tool_q) {
            {
                int count = 0;
                bool bSend = false;
                QMutexLocker locker(&lock);
                if (command_queue.size() > 0) {
                    Command command(command_queue.front());
                    command_queue.pop_front();
                    bSend = command.spout;
                    count = command.count;
                }
                if (bSend)
                    send(count);
                else
                    receive(count);
            }
        }


        {
            QMutexLocker locker(&lock);
            if (command_queue.size() == 0) {
                cond.wait(&lock, 100);
            }
        }

        if (cancelled) {
            destroyQueue();
            break;
        }
    }
}


void Sink::send(int count)
{
    stringstream ss;
    if (msg_tool_q) {
        for (int i=0; i<count; ++i) {
            ss.str("");
            ss << "Message " << msg_tool_q->sent_count;
            msg_tool_q->sent_count ++;
            msg_tool_q->sender.send ( Message ( ss.str() ) );
            QCoreApplication::processEvents();
        }
        emit spouted(msg_tool_q->sent_count);
    }
}

void Sink::receive(int count)
{
    if (msg_tool_q) {
        qpid::messaging::Session session = msg_tool_q->receiver.getSession();
        for (int i=0; i<count; ++i) {
            try
            {
                msg_tool_q->receiver.fetch(Duration::IMMEDIATE);
                msg_tool_q->received_count ++;
                QCoreApplication::processEvents();
            }
            catch (const NoMessageAvailable& e)
            {
              // No need to inform user about this.
              break;
            }
        }
        session.acknowledge();
        emit drained(msg_tool_q->received_count);
    }
}

