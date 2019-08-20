#include "fileformat.h"

FileFormat::FMT FileFormat::getType(const QMimeType mimeType) {
    if (mimeType.isValid() &&
            (mimeType.inherits(QStringLiteral("text/plain"))
                               || mimeType.inherits(QStringLiteral("text/markdown"))
                               || mimeType.inherits(QStringLiteral("text/x-markdown")))) {
        return FMT::MarkDown;
    }
    return FMT::UNKNOWN;
}
