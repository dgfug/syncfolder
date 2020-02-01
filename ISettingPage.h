//
// Created by faywong on 2020/2/1.
//

#ifndef SYNCFOLDER_ISETTINGPAGE_H
#define SYNCFOLDER_ISETTINGPAGE_H
class ISettingPage {
public:
    virtual void save() = 0;
    virtual void reset() = 0;
};
#endif //SYNCFOLDER_ISETTINGPAGE_H
