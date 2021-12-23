#include "progress.h"

Progress::Progress()
{
}

void Progress::setProgress(int progress)
{
    this->progress = progress;
    changed = true;
}

int Progress::getProgress()
{
    return progress;
}

int Progress::getProgressPercent()
{
    if (total == 0)
        return 0;

    return progress * 100 / total;
}

void Progress::setTotal(int total)
{
    this->total = total;
    changed = true;
}

int Progress::getTotal()
{
    return total;
}

bool Progress::isChanged()
{
    return changed;
}

void Progress::setChanged(bool changed)
{
    this->changed = changed;
}
