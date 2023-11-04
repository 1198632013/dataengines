#ifndef RANDOM_H
#define RANDOM_H

#include "dataengineinterface.h"

class Random : public QObject, public DataEngineInterface
{
    Q_OBJECT
    Q_INTERFACES(DataEngineInterface)
    Q_PLUGIN_METADATA(IID "VOFA+.Plugin.Random")

public:
    explicit Random();
    ~Random();
    void ProcessingDatas(char *data, int count);
private:
    uint32_t image_count_mutation_count_ = 0;
};
#endif // RANDOM_H
