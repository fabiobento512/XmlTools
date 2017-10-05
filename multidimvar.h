#ifndef MULTIDIMVAR_H
#define MULTIDIMVAR_H

#include "utilxmltools.h"

class MultiDimVar
{
public:
    QList<double> multiDimValue; // current this class only supports doubles.
    MultiDimVar(QString multiDimValue);
    MultiDimVar(int dim);
    static MultiDimVar sum(const MultiDimVar &val1, const MultiDimVar &val2);
    static MultiDimVar sub(const MultiDimVar &val1, const MultiDimVar &val2);
    QString toString();
};

#endif // MULTIDIMVAR_H
