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

#ifndef QPROTOBUFMODEL_H
#define QPROTOBUFMODEL_H

#include <QAbstractItemModel>
#include <QUrl>
#include <QSharedPointer>

#include <google/protobuf/message.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>

class QProtobufModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(QUrl metadata
                READ metadata
                WRITE setMetadata
                NOTIFY metadataChanged
    )

    Q_PROPERTY(QString message
                READ message
                WRITE setMessage
                NOTIFY messageChanged
    )

    Q_PROPERTY(QUrl source
                READ source
                WRITE setSource
                NOTIFY sourceChanged
    )

public:
    QProtobufModel(QObject *_parent = nullptr);
    virtual ~QProtobufModel();

    virtual QModelIndex index(int _row, int _column,
                              const QModelIndex &_parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &_child) const override;

    virtual int rowCount(const QModelIndex &_index = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &_index = QModelIndex()) const override;

    virtual Qt::ItemFlags flags(const QModelIndex &_index) const override;
    virtual QVariant data(const QModelIndex &_index, int _role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &_index, const QVariant &_value, int _role = Qt::EditRole) override;

    virtual QVariant headerData(int _section, Qt::Orientation _orientation,
                                int _role = Qt::DisplayRole) const override;

    virtual bool insertRows(int _row, int _count, const QModelIndex &_parent = QModelIndex()) override;
    virtual bool removeRows(int _row, int _count, const QModelIndex &_parent = QModelIndex()) override;

    virtual bool canFetchMore(const QModelIndex &_index) const override;
    virtual void fetchMore(const QModelIndex &_parent) override;

    const QUrl& metadata() const { return m_metadata; }
    void setMetadata(const QUrl& _url);

    const QString& message() const { return m_message; }
    void setMessage(const QString &_message);

    const QUrl& source() const { return m_source; }
    void setSource(const QUrl& _url);

signals:
    void metadataChanged(const QUrl& _url);
    void messageChanged(const QString& _url);
    void sourceChanged(const QUrl& _url);

private:
    typedef ::google::protobuf::Message PBMessage;

    QUrl m_metadata;
    QString m_message;
    QUrl m_source;

    ::google::protobuf::DynamicMessageFactory   m_messageFactory;
    const PBMessage* m_prototype;
    PBMessage* m_rootItem;

    void checkSourceChange();

    PBMessage* getMessage(const QModelIndex &_index) const;
};

#endif // QPROTOBUFMODEL_H
