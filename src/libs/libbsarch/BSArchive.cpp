#include "BSArchive.h"
#include <QDebug>

BSArchive::BSArchive()
    : _archive(bsa_create())
{
}

BSArchive::~BSArchive()
{
    if (_openedArchive)
        close();
    free();
}

void BSArchive::free()
{
    const auto &result = bsa_free(_archive);
    _openedArchive = false;

    if(result.code == BSA_RESULT_EXCEPTION)
        throw std::runtime_error(wcharToString(result.text));
}

void BSArchive::open(const QString &archivePath)
{
    const auto &result = bsa_load_from_file(_archive, PREPARE_PATH_LIBBSARCH(archivePath));
    _openedArchive = true;

    if (result.code == BSA_RESULT_EXCEPTION)
        throw std::runtime_error(wcharToString(result.text));
}

void BSArchive::close()
{
    bsa_close(_archive);
    _openedArchive = false;
}

void BSArchive::create(const QString &archiveName, const bsa_archive_type_e& type, const BSArchiveEntries& entries)
{
    bsa_create_archive(_archive, PREPARE_PATH_LIBBSARCH(archiveName), type, entries.getEntries());
}

void BSArchive::save()
{
    const auto &result = bsa_save(_archive);

    if(result.code == BSA_RESULT_EXCEPTION)
        throw std::runtime_error(wcharToString(result.text));
}

void BSArchive::addFileFromDiskRoot(const QString &rootDir, const QString &filename)
{
    const auto &result = bsa_add_file_from_disk_root(_archive,
                                                     PREPARE_PATH_LIBBSARCH(rootDir),
                                                     PREPARE_PATH_LIBBSARCH(filename));

    if (result.code == BSA_RESULT_EXCEPTION)
        throw std::runtime_error(wcharToString(result.text));
}


void BSArchive::addFileFromDiskRoot(const QString &rootDir, const QStringList& files)
{
    for (const auto &file : files)
        addFileFromDiskRoot(rootDir, file);
}

void BSArchive::addFileFromDisk(const QString &pathInArchive, const QString &filePath)
{
    const auto &result = bsa_add_file_from_disk_root(_archive,
                                                     PREPARE_PATH_LIBBSARCH(pathInArchive),
                                                     PREPARE_PATH_LIBBSARCH(filePath));

    if (result.code == BSA_RESULT_EXCEPTION)
        throw std::runtime_error(wcharToString(result.text));
}

void BSArchive::addFileFromMemory(const QString &filename, const QByteArray &data) //NOTE UNTESTED
{
    uint32_t size = static_cast<uint32_t>(data.size());
    bsa_buffer_t buffer = const_cast<char *>(data.data());
    const auto &result = bsa_add_file_from_memory(_archive, PREPARE_PATH_LIBBSARCH(filename), size, buffer);

    if (result.code == BSA_RESULT_EXCEPTION)
        throw std::runtime_error(wcharToString(result.text));
}
void BSArchive::setCompressed(bool value)
{
    bsa_compress_set(_archive, value);
}

void BSArchive::setShareData(bool value)
{
    bsa_share_data_set(_archive, value);
}

bsa_file_record_t BSArchive::findFileRecord(const QString &filename)
{
    const auto &result = bsa_find_file_record(_archive, PREPARE_PATH_LIBBSARCH(filename));
    return result;
}

QByteArray BSArchive::extractFileDataByRecord(bsa_file_record_t record)
{
    const auto &result = bsa_extract_file_data_by_record(_archive, record);

    if(result.message.code == BSA_RESULT_EXCEPTION)
        throw std::runtime_error(wcharToString(result.message.text));

    char* buffer = static_cast<char*>(result.buffer.data);
    int size = static_cast<int>(result.buffer.size);

    QByteArray byte_array(buffer, size);
    return byte_array;
}

QByteArray BSArchive::extractFileDataByFilename(const QString &filename)
{
    const auto &result = bsa_extract_file_data_by_filename(_archive, PREPARE_PATH_LIBBSARCH(filename));

    if(result.message.code == BSA_RESULT_EXCEPTION)
        throw std::runtime_error(wcharToString(result.message.text));

    char *buffer = static_cast<char *>(result.buffer.data);
    int size = static_cast<int>(result.buffer.size);

    QByteArray byte_array(buffer, size);
    return byte_array;
}

void BSArchive::extract(const QString &filename, const QString &saveAs)
{
    qDebug() << "Extracting: " << filename << " saved as " << saveAs;
    const auto &result = bsa_extract_file(_archive, PREPARE_PATH_LIBBSARCH(filename), PREPARE_PATH_LIBBSARCH(saveAs));

    if (result.code == BSA_RESULT_EXCEPTION)
        throw std::runtime_error(wcharToString(result.text));
}

QStringList BSArchive::listFiles()
{
    bsa_entry_list_t list = bsa_entry_list_create();
    const auto &result = bsa_get_resource_list(_archive, list, L"");

    if(result.code == BSA_RESULT_EXCEPTION)
        throw std::runtime_error(wcharToString(result.text));

    return BSArchiveEntries(list).list();
}

bsa_archive_t BSArchive::getArchive() const
{
    return _archive;
}

void BSArchive::reset()
{
    free();
    _archive = bsa_create();
}
