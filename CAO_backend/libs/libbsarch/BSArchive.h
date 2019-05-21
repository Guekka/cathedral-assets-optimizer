#ifndef BSARCHIVE_H
#define BSARCHIVE_H

#include "QLibbsarch.h"
#include "BSArchiveEntries.h"
#include <QDataStream>

class BSArchive
{
public:
    BSArchive();
    virtual ~BSArchive();

    void open(const QString& archivePath);
    void close();
    virtual void create(const QString& archiveName, const bsa_archive_type_e &type, const BSArchiveEntries &entries);
    void save();
    virtual void addFileFromDiskRoot(const QString& rootDir, const QString& filename);
    virtual void addFileFromDiskRoot(const QString& rootDir, const QStringList& files);
    void addFileFromDisk(const QString& pathInArchive, const QString& filePath);
    void addFileFromMemory(const QString& filename, const QByteArray& data);
    void setCompressed(bool value);
    void setShareData(bool value);
    bsa_file_record_t findFileRecord (const QString& filename);
    QByteArray extractFileDataByRecord(bsa_file_record_t record);
    QByteArray extractFileDataByFilename(const QString& filename);
    void extract(const QString& filename, const QString& saveAs);
    QStringList listFiles();
    void free();
    void reset();

    bsa_archive_t getArchive() const;

private:
    bsa_archive_t m_archive;

};

#endif // BSARCHIVE_H
