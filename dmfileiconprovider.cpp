#include "dmfileiconprovider.h"

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
        }
    }
    return QIcon(":/icons/file_unknown.svg");
}

//QString DMFileIconProvider::type(const QFileInfo &info) const {

//}
