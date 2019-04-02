#ifndef TEXTURESOPTIMIZER_H
#define TEXTURESOPTIMIZER_H

#include "pch.h"
#include "QLogger.h"

class TexturesOptimizer : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(TexturesOptimizer)

public:
    static void convertTgaToDds(const QString& filePath);
    static void convertToBc7IfUncompressed(const QString& filePath);
};

#endif // TEXTURESOPTIMIZER_H
