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

#include "object-model.h"
#include <iostream>

using std::cout;
using std::endl;

ObjectListModel::ObjectListModel(QObject* parent, std::string unique) :
        QAbstractListModel(parent), uniqueProperty(unique)
{
}

void ObjectListModel::addObject(const qmf::Data& object, uint correlator)
{
    Q_UNUSED(correlator);
    if (!object.isValid())
        return;

    // don't add queue's that are exclusive
    const qpid::types::Variant& exclusive = object.getProperty("exclusive");
    if (exclusive.asBool())
        return;

    // get the uniqueu property for this object
    const qpid::types::Variant& name = object.getProperty(uniqueProperty);

    // see if the object exists in the list
    for (int idx=0; idx<dataList.size(); idx++) {
        qmf::Data existing = dataList.at(idx);
        if (name.isEqualTo(existing.getProperty(uniqueProperty))) {

            qpid::types::Variant::Map map = qpid::types::Variant::Map(object.getProperties());
            map["correlator"] = correlator;
            existing.overwriteProperties(map);
            return;
        }
    }

    qmf::Data o = qmf::Data(object);
    qpid::types::Variant corr =  qpid::types::Variant(correlator);
    o.setProperty("correlator", corr);

    // this is a new queue
    int last = dataList.size();
    beginInsertRows(QModelIndex(), last, last);
    dataList.append(o);
    endInsertRows();
}

void ObjectListModel::refresh(uint correlator)
{
    // remove any old queues that were not added/updated with this correlator
    for (int idx=0; idx<dataList.size(); idx++) {
        uint corr = dataList.at(idx).getProperty("correlator").asUint32();
        if (corr != correlator) {
            beginRemoveRows( QModelIndex(), idx, idx );
            dataList.removeAt(idx--);
            endRemoveRows();
        }
    }

    // force a refresh of the display
    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(dataList.size() - 1, 2);
    emit dataChanged ( topLeft, bottomRight );
}


void ObjectListModel::connectionChanged(bool isConnected)
{
    if (!isConnected)
        clear();
}

void ObjectListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, dataList.count() - 1);
    dataList.clear();
    endRemoveRows();
}


int ObjectListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return (int) dataList.size();
}


QVariant ObjectListModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();

    if (role == Qt::UserRole) {
        return QVariant(dataList.at(index.row()));
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    const qmf::Data& object= dataList.at(index.row());
    const qpid::types::Variant& name = object.getProperty(uniqueProperty);

    QString n = QString(name.asString().c_str());
    return n;
}

std::string ObjectListModel::fieldValue(int row, const std::string& field)
{
    const qmf::Data& object= dataList.at(row);
    qpid::types::Variant value = object.getProperty(field);
    if ((field == "queueRef") ||
        (field == "exchangeRef") ||
        (field == "sessionRef") ||
        (field == "connectionRef") ||
        (field == "vhostRef")) {
        return value.asMap()["_object_name"].asString();
    }
    return value.asString();
}

const qmf::Data& ObjectListModel::qmfData(int row)
{
    return dataList.at(row);
}

QVariant ObjectListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section == 0) {
        if (role == Qt::DisplayRole && orientation == Qt::Vertical) {
            return QString("List");
        }
    }
    return QVariant();
}

// SLOT
void ObjectListModel::selected(const QModelIndex &index)
{
    if (index.isValid()) {
        const qmf::Data& object = dataList.at(index.row());
        // show the details for this object in the dialog's object table
        emit objectSelected(object);
    }
}

const qmf::Data& ObjectListModel::getSelected(const QModelIndex &index)
{
    return dataList.at(index.row());
}

std::ostream& operator<<(std::ostream& out, const qmf::Data& object)
{
    if (object.isValid()) {
        qpid::types::Variant::Map::const_iterator iter;
        const qpid::types::Variant::Map& attrs(object.getProperties());

        out << " <properties>\n";
        for (iter = attrs.begin(); iter != attrs.end(); iter++) {
            if (iter->first != "name") {
                out << "  <property>\n";
                out << "   <name>" << iter->first << "</name>\n";
                out << "   <value>" << iter->second << "</value>\n";
                out << "  </property>\n";
            }
        }
        out << " </properties>\n";
    }
    return out;
}
