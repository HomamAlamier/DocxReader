#include <docxdocument.h>
#include <zipfile.h>

const QString FN_doc = "word/document.xml";
const QString FN_docRels = "word/_rels/document.xml.rels";


DocxDocument:: DocxDocument(const QString& filename)
    : _filename(filename)
    , _isValid()
{
    ZipFile zf(filename, ZipFile::Read);
    if (zf.isOpen())
    {
        size_t count = zf.getFilesCount();
        for(size_t i = 0; i < count; i++)
        {
            auto inf = zf.getFileInfo(i);
            if (inf)
            {
                auto stream = zf.openFile(inf->index);
                if (stream)
                {
                    char* buf = new char[stream->length() + 1];
                    stream->read(buf, stream->length());
                    buf[stream->length()] = 0;
                    if (inf->name == FN_doc)
                    {
                        _contents.push_back(std::make_pair(Document, (ContentData) { inf->name, QString(buf) }));
                    }
                    else if (inf->name == FN_docRels)
                    {
                        _contents.push_back(std::make_pair(DocumentRelations, (ContentData) { inf->name, QString(buf) }));
                    }
                    else
                    {
                        _contents.push_back(std::make_pair(Unknown, (ContentData) { inf->name, QString(buf) }));
                    }
                    delete[] buf;
                    stream->close();
                }
            }
        }
        _isValid = true;
    }
    zf.close();
}

QString DocxDocument::getContent(Content c)
{
    if (c == Unknown)
        return QString();

    for(auto& cd : _contents)
    {
        if (cd.first == c)
        {
            return cd.second.data;
        }
    }
    return QString();
}

void DocxDocument::setContent(Content c, const QString &data)
{
    if (c == Unknown)
        return;

    for(auto& cd : _contents)
    {
        if (cd.first == c)
        {
            cd.second.data = data;
        }
    }
}

bool DocxDocument::save(const QString &filename)
{
    ZipFile f(filename, ZipFile::Write);
    if (!f.isOpen())
        return false;
    for(const auto& c : _contents)
    {
        auto stream = f.addFile(c.second.name);
        if (stream)
        {
            auto byteArray = c.second.data.toUtf8();
            stream->write(byteArray.data(), byteArray.size());
            delete stream;
        }
    }
    f.close();
    return true;
}
