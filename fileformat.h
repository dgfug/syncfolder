#ifndef FILEFORMAT_H
#define FILEFORMAT_H

#include <QMimeType>

class FileFormat
{
public:
    typedef enum {
        MarkDown,
        Rst,
        OrgMode,
        UNKNOWN
    } FMT;
    static FMT getType(const QMimeType mimeType);
};

#endif // FILEFORMAT_H
