#include "QLibbsarch.h"
const wchar_t *QStringToWchar(const QString& qs)
{
    wchar_t *array = new wchar_t [static_cast<unsigned int>(qs.length() + 1)];
    qs.toWCharArray(array);
    array [qs.length()] = '\0';
    return array;
}

const std::string wcharToString(const wchar_t *text)
{
    QString qs = QString::fromWCharArray(text) ;
    return qs.toStdString();
}



