#include <zipfile.h>
#include <zip.h>
#include <QDebug>
#include <memory.h>
QString get_libzip_error(int err)
{
    char buf[100];
    zip_error_to_str(buf, sizeof(buf), err, errno);
    return QString(buf);
}
void reportLibZipError(int err)
{
    qDebug() << "ZipFile:" << get_libzip_error(err);
}

ZipFileStream::ZipFileStream(struct zip_file* f, size_t len)
    : _zFile(f)
    , _zip(nullptr)
    , _len(len)
    , _flags(Read)
    , _buffer(nullptr)
{
}

ZipFileStream::ZipFileStream(struct zip* zip, const QString& name)
    : _zFile(nullptr)
    , _zip(zip)
    , _name(name)
    , _len(0)
    , _flags(Write)
    , _buffer(nullptr)
{
}

ZipFileStream::~ZipFileStream()
{
    close();
}

size_t ZipFileStream::read(char *buffer, size_t size)
{
    if (!_zFile || _flags != Read)
        return -1;
    return zip_fread(_zFile, buffer, size);
}

char* ZipFileStream::readAll()
{
    if (!_zFile || _flags != Read)
        return nullptr;
    char* buf = new char[_len];
    read(buf, _len);
    return buf;
}

size_t ZipFileStream::write(char *buf, size_t l)
{
    if (_flags != Write)
        return -1;

    char* old = _buffer;
    _buffer = new char[_len + l];
    if (old)
        memcpy(_buffer, old, _len);
    memcpy(&_buffer[_len], buf, l);
    _len += l;
    return l;
}

void ZipFileStream::close()
{
    if (_flags == Read)
        zip_fclose(_zFile);
    else
    {
        zip_source* src = zip_source_buffer(_zip, _buffer, _len, 1);
        zip_file_add(_zip, _name.toStdString().c_str(), src, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8);
        //delete[] _buffer;
    }
}

ZipFile::ZipFile(const QString& filename, OpenFlags flags)
    : _filename(filename)
    , _flags(flags)
{
    open();
}

ZipFile::~ZipFile()
{
    close();
}

void ZipFile::open()
{
    if (_filename.isEmpty())
        return;

    int err = 0;
    _zip = zip_open(_filename.toStdString().c_str(), _flags == Write ? ZIP_CREATE : 0, &err);
    if (_zip == nullptr)
    {
        reportLibZipError(err);
        return;
    }
    _files.clear();
    if (_flags == Read)
    {
        int numEntries = zip_get_num_entries(_zip, 0);
        struct zip_stat st;
        for(int i = 0; i < numEntries; i++)
        {
            if (zip_stat_index(_zip, i, 0, &st) == 0)
            {
                FileInfo inf;
                inf.index = i;
                inf.name = QString(st.name);
                inf.size = st.size;
                qDebug() << "ZipFile: found " << st.name << " (Size = " << st.size << " Bytes)";
                _files.push_back(inf);
            }
        }
    }
}

void ZipFile::extractAll()
{

}

void ZipFile::compressAll()
{

}

void ZipFile::close()
{
    if (_zip != nullptr)
    {
        zip_close(_zip);
        _zip = nullptr;
    }
}

ZipFileStream* ZipFile::addFile(const QString &name)
{
    if (_flags != Write)
        return nullptr;
    return new ZipFileStream(_zip, name);
}

ZipFileStream* ZipFile::openFile(int index)
{
    if (!isOpen() || _flags != Read)
        return nullptr;

    const FileInfo& inf = _files[index];
    struct zip_file* f = zip_fopen_index(_zip, inf.index, 0);
    if (!f)
    {
        qDebug() << "ZipFile: cannot open file at index" << index;
        return nullptr;
    }
    return new ZipFileStream(f, inf.size);
}

ZipFileStream* ZipFile::openFile(const QString &name)
{
    if (!isOpen() || _flags != Read)
        return nullptr;

    for(const auto& inf : _files)
    {
        if (inf.name == name)
        {
            return openFile(inf.index);
        }
    }
    return nullptr;
}

const ZipFile::FileInfo* ZipFile::getFileInfo(const QString& name) const
{
    if (!isOpen())
        return nullptr;

    for(size_t i = 0; i < _files.size(); i++)
    {
        if (_files[i].name == name)
        {
            return &_files[i];
        }
    }
    return nullptr;
}
