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

#ifndef MSG_TOOL_H
#define MSG_TOOL_H

#include <vector>

#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>

#include <stdio.h>
#include <unistd.h>     // For pause()
#include <sys/time.h>
#include <signal.h>     // For signal


#include <unistd.h>
#include <cstdlib>
#include <iostream>

#include <sstream>

using namespace std;
using namespace qpid::messaging;

enum QueueType
{
  priorityQueue,
  lastValueQueue
};


struct msg_tool_queue
{
  string     queue_name;
  string     address;
  Sender     sender;
  Receiver   receiver;
  int        sent_count,
             received_count;

  msg_tool_queue ( Session & _session,
                   char const * _queue_name,
                   char const * arguments
                 );
};

#endif // MSG_TOOL_H
