#include "multidimvar.h"

// Receives a string with a value with multiple dimensions separated by space
// Example of input "5 4 6". Variable with dim1=5, dim2=4, dim3=6.
MultiDimVar::MultiDimVar(QString multiDimValue)
{
    this->multiDimValue=UtilXmlTools::qListDoubleFromSpacedString(multiDimValue);
}

// Inicializes a new MultiDimVar with the dimensions specified with all zero values
MultiDimVar::MultiDimVar(int dim){

    for(int i=0; i<dim; i++){
        this->multiDimValue.append(0);
    }

}

// Sums 2 MultiDimVar values
MultiDimVar MultiDimVar::sum(const MultiDimVar &val1, const MultiDimVar &val2){
    MultiDimVar result(val1.multiDimValue.size());

    if(val1.multiDimValue.size()!=val2.multiDimValue.size()){
        UtilXmlTools::displayErrorMessage("MultiDimVar sum","Error summing MultiDimVar, variables don't have the same dimensions.");
    }

    for(int i=0; i<result.multiDimValue.size(); i++){
        result.multiDimValue[i]=val1.multiDimValue[i]+val2.multiDimValue[i];
    }

    return result;
}

// Subtract 2 MultiDimVar values
MultiDimVar MultiDimVar::sub(const MultiDimVar &val1, const MultiDimVar &val2){
    MultiDimVar result(val1.multiDimValue.size());

    if(val1.multiDimValue.size()!=val2.multiDimValue.size()){
        UtilXmlTools::displayErrorMessage("MultiDimVar subtraction","Error subtracting MultiDimVar, variables don't have the same dimensions.");
    }

    for(int i=0; i<result.multiDimValue.size(); i++){
        result.multiDimValue[i]=val1.multiDimValue[i]-val2.multiDimValue[i];
    }

    return result;
}

// Returns a string representation of this multidimensional value as string.
// Each dimension is separated by a space.
QString MultiDimVar::toString(){

    QString result;

    for(int i=0; i<this->multiDimValue.size()-1; i++){
        result += QString::number(this->multiDimValue[i],'G',9) + " "; // G is the option used by onisplit
    }

    result += QString::number(this->multiDimValue[this->multiDimValue.size()-1],'G',9); // last one does not have a space at the end

    return result;

}
