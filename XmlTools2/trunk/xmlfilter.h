#ifndef XMLFILTER_H
#define XMLFILTER_H

#include <QString>

//class to simplify XML filtering
class XmlFilter
{
public:

    XmlFilter();
    XmlFilter(QString elementName, QString parentElementName, QString attributeName, QString attributeValue);

    QString getElementName();
    QString getParentElementName();
    QString getAttributeName();
    QString getAttributeValue();

    void setElementName(QString elementName);
    void setParentElementName(QString parentElementName);
    void setAttributeName(QString attributeName);
    void setAttributeValue(QString attributeValue);

    void clear();
private:
    QString elementName;
    QString parentElementName;
    QString attributeName;
    QString attributeValue;
};

#endif // XMLFILTER_H
