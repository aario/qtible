#include "utils.h"

#include <cmath>

Utils::Utils()
{

}

QString Utils::formatTime(float seconds)
{
    int m = floor(seconds / 60);
    int s = round(fmod(seconds, 60));
    return QString::number(m).rightJustified(2, '0')
            + ":" + QString::number(s).rightJustified(2, '0');
}
