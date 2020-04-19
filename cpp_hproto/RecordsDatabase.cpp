#include "RecordsDatabase.h"

void RecordsDatabase::pushInfo(const RecordInfo& Info)
{
    RecordInfos.push_back(Info);
}

std::vector<RecordInfo> RecordsDatabase::getAll() const
{
    return RecordInfos;
}
