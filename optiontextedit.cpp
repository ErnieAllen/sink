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

#include "optiontextedit.h"
#include <QTextStream>

OptionTextEdit::OptionTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
}

void OptionTextEdit::setOption(const QString &name, const QString &value)
{
    options[name] = value;
}

QString OptionTextEdit::constructString()
{
    QString buf;
    QString sep = "";
    QHash<QString, QString>::const_iterator i = options.constBegin();
    while (i != options.constEnd()) {
        buf.append(sep);
        buf.append("'qpid.");
        buf.append(i.key());
        buf.append("':");
        buf.append(i.value());
        sep = ",";
        ++i;
    }
    return buf;
}

void OptionTextEdit::clearOption(const QString &name)
{
    if (options.contains(name))
        options.remove(name);
}

void OptionTextEdit::regenText()
{
    setPlainText(constructString());
}
