#include "xmlfilter.h"

XmlFilter::XmlFilter()
{

}

XmlFilter::XmlFilter(QString elementName, QString parentElementName, QString attributeName, QString attributeValue){
    this->elementName=elementName;
    this->parentElementName=parentElementName;
    this->attributeName=attributeName;
    this->attributeValue=attributeValue;
}

// QStrings

QString XmlFilter::getElementName(){
    return this->elementName;
}

QString XmlFilter::getParentElementName(){
    return this->parentElementName;
}

QString XmlFilter::getAttributeName(){
    return this->attributeName;
}

QString XmlFilter::getAttributeValue(){
    return this->attributeValue;
}

void XmlFilter::setElementName(QString elementName){
    this->elementName=elementName;
}

void XmlFilter::setParentElementName(QString parentElementName){
    this->parentElementName=parentElementName;
}

void XmlFilter::setAttributeName(QString attributeName){
    this->attributeName=attributeName;
}

void XmlFilter::setAttributeValue(QString attributeValue){
    this->attributeValue=attributeValue;
}

// Clears filter
void XmlFilter::clear(){
    this->elementName.clear();
    this->parentElementName.clear();
    this->attributeName.clear();
    this->attributeValue.clear();
}
