#ifndef TEXTURESOPTIMIZER_H
#define TEXTURESOPTIMIZER_H

#include "pch.h"
#include "QLogger.h"

class TexturesOptimizer : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(TexturesOptimizer)

public:
    static void texturesTgaToDds(const QString& filePath);
    static void texturesBc7Conversion(const QString& filePath);

    bool bTexturesNecessaryOptimization{};
    bool bTexturesFullOptimization{};
};

#endif // TEXTURESOPTIMIZER_H
