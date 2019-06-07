#include "BSArchiveEntries.h"

BSArchiveEntries::BSArchiveEntries() : m_entries (bsa_entry_list_create()) {}

BSArchiveEntries::BSArchiveEntries(const QStringList& QSLEntries) : m_entries (bsa_entry_list_create())
{
    for(auto entry : QSLEntries)
        add (entry);
}

BSArchiveEntries::BSArchiveEntries(const bsa_entry_list_t& entries) : m_entries(entries) {}

BSArchiveEntries::~BSArchiveEntries()
{
    free();
}

void BSArchiveEntries::free()
{
    bsa_entry_list_free(m_entries);
}

void BSArchiveEntries::reset()
{
    free();
    m_entries = bsa_entry_list_create();
}

void BSArchiveEntries::add(const QString &filepath)
{
    const wchar_t *path = QStringToWchar( QDir::toNativeSeparators(filepath) );
    auto result = bsa_entry_list_add(m_entries, path);
    delete path;

    if(result.code == BSA_RESULT_EXCEPTION)
        throw std::runtime_error(wcharToString(result.text));
}

uint32_t BSArchiveEntries::count()
{
    return bsa_entry_list_count(m_entries);
}

QStringList BSArchiveEntries::list()
{
    QStringList list;
    for(uint32_t i = 0 ; i < count() ; ++i)
    {
        wchar_t buffer[1024];
        bsa_entry_list_get(m_entries, i, 1024, buffer);
        list << QString::fromWCharArray(buffer);
    }
    return list;
}

bsa_entry_list_t BSArchiveEntries::getEntries() const
{
    return m_entries;
}

