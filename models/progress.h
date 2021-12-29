#ifndef PROGRESS_H
#define PROGRESS_H


class Progress
{
public:
    Progress();
    void setProgress(int progress);
    int getProgress();
    int getProgressPercent();
    void setTotal(int total);
    int getTotal();
    bool isChanged();
    void setChanged(bool changed);
private:
    int progress = 0;
    int total = 0;
    bool changed = false;
};

#endif // PROGRESS_H
