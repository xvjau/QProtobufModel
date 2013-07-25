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
#include <qabstractitemmodel.h>

#include <google/protobuf/io/zero_copy_stream.h>

#include <QDebug>
#include <QFile>

#include <memory>

namespace pb = ::google::protobuf;

QProtobufModel::QProtobufModel(QObject *_parent):
    QAbstractItemModel(_parent)
{

}

QProtobufModel::~QProtobufModel()
{

}

QModelIndex QProtobufModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(0, 0);
}

QModelIndex QProtobufModel::parent(const QModelIndex &child) const
{
    return createIndex(0, 0);
}

int QProtobufModel::rowCount(const QModelIndex &) const
{
    return 0;
}

int QProtobufModel::columnCount(const QModelIndex &) const
{
    return 0;
}

Qt::ItemFlags QProtobufModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index);
}

QVariant QProtobufModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
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
    checkSourceChange();
}

void QProtobufModel::setMessage(const QString &_message)
{
    m_message = _message;
    emit messageChanged(m_message);
    checkSourceChange();
}

void QProtobufModel::setSource(const QUrl &_url)
{
    m_source = _url;
    emit sourceChanged(m_source);
    checkSourceChange();
}

class LocalFileStream: public ::pb::io::ZeroCopyInputStream
{
protected:
    QFile m_file;
    QByteArray m_data;

    static const int PAGE_SIZE = 16 * 1024;

public:
    LocalFileStream(const QUrl& _url)
    {
        QString fileName = _url.toString();

        if (fileName.startsWith("qrc:/"))
            fileName.remove(0, 3);
        else if (fileName.startsWith("file://"))
            fileName.remove(0, 7);

        m_file.setFileName(fileName);

        m_data.resize(PAGE_SIZE);
        m_file.open(QIODevice::ReadOnly);

        if (m_file.error() != QFile::NoError)
        {
            throw m_file.fileName() + ": " + m_file.errorString();
        }
    }

    virtual void BackUp(int count) override
    {
        m_file.seek(m_file.pos() - count);
    }

    virtual ::pb::int64 ByteCount() const override
    {
        return m_file.size();
    }

    virtual bool Next(const void **data, int *size) override
    {
        if (m_file.atEnd() || m_file.error() != QFile::NoError)
            return false;

        *size = m_file.read(m_data.data(), PAGE_SIZE);
        *data = m_data.data();

        return true;
    }

    virtual bool Skip(int count) override
    {
        return m_file.seek(m_file.pos() + count);
    }
};

class QProtobufModelErrorCollector: public ::pb::compiler::MultiFileErrorCollector
{
private:
    QProtobufModel* m_parent;

public:
    QProtobufModelErrorCollector(QProtobufModel* _parent): m_parent(_parent) {}

    virtual void AddError(const std::string & filename, int line, int column, const std::string & message) override
    {
        qDebug() << "Protobuf compiler error: " << filename.c_str() << ':' << line << ':' << column << ": " << message.c_str();
    }
};

class QProtobufModelSourceTree: public ::pb::compiler::SourceTree
{
private:
    QProtobufModel *m_parent;
    ::pb::io::ZeroCopyInputStream* m_stream = nullptr;

public:
    QProtobufModelSourceTree(const QUrl &_url, QProtobufModel* _parent):
        m_parent(_parent)
    {
        if (_url.isLocalFile())
        {
            m_stream = new LocalFileStream(_url);
        }
        else
        {
            QString scheme = _url.scheme();

            if (!scheme.isEmpty())
            {
                switch(scheme.at(0).toLower().toLatin1())
                {
                    case 'q':
                        if (scheme.compare("qrc", Qt::CaseInsensitive) == 0)
                        {
                            m_stream = new LocalFileStream(_url);
                            goto FOUND_STREAM;
                        }
                        break;
                }
            }
        }

        qDebug() << "Unable to open stream: " << _url.toString();

        FOUND_STREAM:
        {}
    }

    virtual ~QProtobufModelSourceTree()
    {
        delete m_stream;
    }

    virtual ::pb::io::ZeroCopyInputStream *Open(const std::string&) override
    {
        ::pb::io::ZeroCopyInputStream *result = m_stream;
        m_stream = nullptr;
        return result;
    }
};

void QProtobufModel::checkSourceChange()
{
    if (m_metadata.isEmpty())
        return;

    if (m_message.isEmpty())
        return;

    if (m_source.isEmpty())
        return;

    QProtobufModelErrorCollector errorCollector(this);
    QProtobufModelSourceTree sourceTree(m_metadata, this);

    ::pb::compiler::Importer importer(&sourceTree, &errorCollector);

    auto fileDescriptor = importer.Import(m_metadata.toString().toStdString());

    auto descriptor = fileDescriptor->FindMessageTypeByName(m_message.toStdString());

    m_prototype = QSharedPointer<const ::pb::Message>(m_messageFactory.GetPrototype(descriptor));

    m_rootItem = QSharedPointer<::pb::Message>(m_prototype->New());

    LocalFileStream sourceStream(m_source);

    m_rootItem->ParseFromZeroCopyStream(&sourceStream);
}


#include "qprotobufmodel.moc"
