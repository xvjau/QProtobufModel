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

class QProtobufModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(QUrl metadata
                READ metadata
                WRITE setMetadata
                NOTIFY metadataChanged
    )

    Q_PROPERTY(QUrl srouce
                READ source
                WRITE setSource
                NOTIFY sourceChanged
    )

public:
    QProtobufModel(QObject *_parent);
    virtual ~QProtobufModel();

    virtual int rowCount(const QModelIndex& = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex& = QModelIndex()) const override;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const override;

    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    virtual bool canFetchMore(const QModelIndex &) const override;
    virtual void fetchMore(const QModelIndex &parent) override;

    const QUrl& metadata() const { return m_metadata; }
    void setMetadata(const QUrl& _url);

    const QUrl& source() const { return m_source; }
    void setSource(const QUrl& _url);

signals:
    void metadataChanged(const QUrl& _url);
    void sourceChanged(const QUrl& _url);

private:
    QUrl m_metadata;
    QUrl m_source;
};

#endif // QPROTOBUFMODEL_H
