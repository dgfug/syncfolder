#ifndef DISPLAYQUEUEDFILESACTION_H
#define DISPLAYQUEUEDFILESACTION_H

#include <QWidgetAction>

class DisplayQueuedFilesAction : public QWidgetAction
{
    Q_OBJECT
public:
    DisplayQueuedFilesAction(const QSet<QString> &queuedFiles);
};

#endif // DISPLAYQUEUEDFILESACTION_H
