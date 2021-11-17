#ifndef ZIPFILE_H
#define ZIPFILE_H
#include <QString>
#include <vector>
struct zip;
struct zip_file;

class ZipFileStream
{
public:
    enum Flags
    {
        Read,
        Write
    };
    ZipFileStream(struct zip_file*, size_t);
    ZipFileStream(struct zip*, const QString&);
    ~ZipFileStream();
    size_t read(char*, size_t);
    char* readAll();
    size_t write(char*, size_t);
    void close();
    size_t length() const { return _len; }
private:
    struct zip_file* _zFile;
    struct zip* _zip;
    QString _name;
    size_t _len;
    Flags _flags;
    char* _buffer;
};

class ZipFile
{
public:
    enum OpenFlags
    {
        Read,
        Write
    };
    struct FileInfo
    {
        QString name;
        unsigned long size;
        int index;
    };
    ZipFile(const QString& filename, OpenFlags flags);
    ~ZipFile();
    void extractAll();
    void compressAll();
    void close();


    ZipFileStream* addFile(const QString& name);
    ZipFileStream* openFile(int index);
    ZipFileStream* openFile(const QString& name);


    size_t getFilesCount() const { return _files.size(); }
    const FileInfo* getFileInfo(size_t index) const { return &_files.at(index); }
    const FileInfo* getFileInfo(const QString& name) const;
    bool isOpen() const { return _zip != nullptr; }

    void setExtractPoint(const QString& path) { _extPoint = path; }
private:
    void open();
    QString _filename;
    QString _extPoint;
    zip* _zip;
    OpenFlags _flags;
    std::vector<FileInfo> _files;
};

#endif // ZIPFILE_H
