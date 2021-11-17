#ifndef DOCXDOCUMENT_H
#define DOCXDOCUMENT_H
#include <QString>
#include <vector>
class ZipFile;
class DocxDocument
{
public:
    enum Content
    {
        Unknown,
        Document,
        DocumentRelations
    };
    DocxDocument(const QString& filename);

    QString getContent(Content c);
    void setContent(Content c, const QString& data);

    bool save(const QString& filename);

    bool isValid() const { return _isValid; }
private:
    struct ContentData
    {
        QString name;
        QString data;
    };
    QString _filename;
    bool _isValid;
    std::vector<std::pair<Content, ContentData> > _contents;
};

#endif // DOCXDOCUMENT_H
