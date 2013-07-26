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

#include <cassert>

namespace pb = ::google::protobuf;

QProtobufModel::QProtobufModel(QObject *_parent):
    QAbstractItemModel(_parent)
{

}

QProtobufModel::~QProtobufModel()
{

}

QProtobufModel::PBMessage *QProtobufModel::getMessage(const QModelIndex &_index) const
{
    if (_index.isValid())
    {
        PBMessage* result = static_cast<PBMessage*>(_index.internalPointer());
        if (result)
            return result;
    }

    return m_rootItem;
}

QModelIndex QProtobufModel::index(int _row, int _column, const QModelIndex &_parent) const
{
    return createIndex(0, 0);
}

QModelIndex QProtobufModel::parent(const QModelIndex &_child) const
{
    return createIndex(0, 0);
}

int QProtobufModel::rowCount(const QModelIndex &_index) const
{
    PBMessage* msg = getMessage(_index);

    if (msg == m_rootItem)
        return 1;

    PBMessage* parent = getMessage(_index.parent());

    auto descr = parent->GetDescriptor();

    auto field = descr->FindFieldByName(msg->GetTypeName());

    if (field->is_repeated())
    {
        int count = parent->GetReflection()->FieldSize(*parent, field);
        return count;
    }
    else
        return 1;
}

int QProtobufModel::columnCount(const QModelIndex &_index) const
{
    PBMessage* msg = getMessage(_index);
    return msg->GetDescriptor()->field_count();
}

Qt::ItemFlags QProtobufModel::flags(const QModelIndex &_index) const
{
    return QAbstractItemModel::flags(_index);
}

QVariant QProtobufModel::data(const QModelIndex &_index, int _role) const
{
    switch(_role)
    {
        case Qt::EditRole:
        case Qt::DisplayRole:
        {
            PBMessage* msg = getMessage(_index);


        }
    }


    return QVariant();
}

bool QProtobufModel::setData(const QModelIndex &_index, const QVariant &_value, int _role)
{
    return QAbstractItemModel::setData(_index, _value, _role);
}

QVariant QProtobufModel::headerData(int _section, Qt::Orientation _orientation, int _role) const
{
    return QVariant();
}

bool QProtobufModel::insertRows(int _row, int _count, const QModelIndex &_parent)
{
    return QAbstractItemModel::insertRows(_row, _count, _parent);
}

bool QProtobufModel::removeRows(int _row, int _count, const QModelIndex &_parent)
{
    return QAbstractItemModel::removeRows(_row, _count, _parent);
}

bool QProtobufModel::canFetchMore(const QModelIndex &_index) const
{
    return false;
}

void QProtobufModel::fetchMore(const QModelIndex &_parent)
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

    m_prototype = m_messageFactory.GetPrototype(descriptor);

    m_rootItem = m_prototype->New();

    LocalFileStream sourceStream(m_source);

    m_rootItem->ParseFromZeroCopyStream(&sourceStream);
}


#include "qprotobufmodel.moc"
