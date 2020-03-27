#include "FileIconProvider.h"

DMFileIconProvider::DMFileIconProvider() : QFileIconProvider ()
{

}

QIcon DMFileIconProvider::icon(const QFileInfo &info) const {
    QString fileName = info.fileName();
    if (info.isDir()) {
        return QIcon(":/icons/folder.svg");
    } else if (info.isFile()) {
        if (fileName.endsWith(".txt")) {
            return QIcon(":/icons/file_text.svg");
        } else if (fileName.endsWith(".md")) {
            return QIcon(":/icons/file_markdown.svg");
        } else if (fileName.endsWith(".csv")) {
            return QIcon(":/icons/csv.svg");
        } else if (fileName.endsWith(".svg")) {
            return QIcon(":/icons/svg.svg");
        } else if (fileName.endsWith(".jpg") || fileName.endsWith(".jpeg")) {
            return QIcon(":/icons/jpg.svg");
        } else if (fileName.endsWith(".json")) {
            return QIcon(":/icons/json.svg");
        } else if (fileName.endsWith(".png")) {
            return QIcon(":/icons/png.svg");
        }
    }
    return QIcon(":/icons/file_unknown.svg");
}
