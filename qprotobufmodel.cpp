/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2013  Gianni Rossi <gianni.rossi@bitforge.com.br>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "qprotobufmodel.h"

#include <google/protobuf/message.h>

QProtobufModel::QProtobufModel(QObject *_parent):
    QAbstractItemModel(_parent)
{

}

QProtobufModel::~QProtobufModel()
{

}

QModelIndex QProtobufModel::index(int row, int column, const QModelIndex &parent) const
{

}

QModelIndex QProtobufModel::parent(const QModelIndex &child) const
{

}

int QProtobufModel::rowCount(const QModelIndex &) const
{

}

int QProtobufModel::columnCount(const QModelIndex &) const
{

}

Qt::ItemFlags QProtobufModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index);
}

QVariant QProtobufModel::data(const QModelIndex &index, int role) const
{

}

bool QProtobufModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return QAbstractItemModel::setData(index, value, role);
}

QVariant QProtobufModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QAbstractItemModel::headerData(section, orientation, role);
}

bool QProtobufModel::insertRows(int row, int count, const QModelIndex &parent)
{
    return QAbstractItemModel::insertRows(row, count, parent);
}

bool QProtobufModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return QAbstractItemModel::removeRows(row, count, parent);
}

bool QProtobufModel::canFetchMore(const QModelIndex &) const
{
    return false;
}

void QProtobufModel::fetchMore(const QModelIndex &parent)
{
}

void QProtobufModel::setMetadata(const QUrl &_url)
{
    m_metadata = _url;
    emit metadataChanged(m_metadata);
}

void QProtobufModel::setMessage(const QString &_message)
{
    m_message = _message;
    emit messageChanged(m_message);
}

void QProtobufModel::setSource(const QUrl &_url)
{
    m_source = _url;
    emit sourceChanged(m_source);
}

#include "qprotobufmodel.moc"
