/******************************************************************
*
*	CyberXML for C++
*
*	Copyright (C) Satoshi Konno 2002-2003
*
*	File: Node.h
*
*	Revision;
*
*	04/22/03
*		- first revision
*
******************************************************************/

#ifndef _CXML_NODE_H_
#define _CXML_NODE_H_

#include <iostream>
#include <sstream>
#include <string>

#include <cybergarage/xml/AttributeList.h>
#include <cybergarage/xml/NodeList.h>
#include <cybergarage/xml/NodeData.h>

namespace CyberXML {

class Node 
{

	Node *parentNode;

	std::string name;
	std::string value;
	
	AttributeList attrList;
	NodeList nodeList;
	
	NodeData *userData; 
	
public:
	
	Node();

	Node(const char *name);

	Node(const char *ns, const char *name);

	virtual ~Node();

	////////////////////////////////////////////////
	//	parent node
	////////////////////////////////////////////////

	void setParentNode(Node *node) 
	{
		parentNode = node;
	}

	Node *getParentNode() 
	{
		return parentNode;
	}

	////////////////////////////////////////////////
	//	root node
	////////////////////////////////////////////////

	Node *getRootNode()
	{
		Node *rootNode = NULL;
		Node *parentNode = getParentNode();
		while (parentNode != NULL) {
			 rootNode = parentNode;
			 parentNode = rootNode->getParentNode();
		}
		return rootNode;
	}

	////////////////////////////////////////////////
	//	name
	////////////////////////////////////////////////

	void setName(const char *val) 
	{
		name = val;
	}

	void setName(const char *ns, const char *val) 
	{
		name = ns;
		name += ":";
		name += val;
	}

	const char *getName()
	{
		return name.c_str();
	}

	bool isName(const char *value)
	{
		return (name.compare(value) == 0) ? true : false;
	}

	////////////////////////////////////////////////
	//	value (set)
	////////////////////////////////////////////////

	void setValue(const char *val)
	{
		value = val;
	}

	void setValue(int val);

	const char *getValue()
	{
		return value.c_str();
	}

	////////////////////////////////////////////////
	//	value (add)
	////////////////////////////////////////////////

	void addValue(const char *val)
	{
		value.append(val);
	}

	void addValue(const char *val, int len)
	{
		value.append(val, 0, len);
	}

	////////////////////////////////////////////////
	//	Attribute (Basic)
	////////////////////////////////////////////////

	int getNAttributes() {
		return attrList.size();
	}

	Attribute *getAttribute(int index) {
		return attrList.getAttribute(index);
	}

	Attribute *getAttribute(const char * name)
	{
		return attrList.getAttribute(name);
	}

	void addAttribute(Attribute *attr) {
		attrList.addAttribute(attr);
	}

	void insertAttributeAt(Attribute *attr, int index) {
		attrList.insertAttribute(attr, index);
	}

	void addAttribute(const char * name, const char * value) {
		Attribute *attr = new Attribute(name, value);
		addAttribute(attr);
	}

	bool removeAttribute(Attribute *attr) {
		return attrList.removeAttribute(attr);
	}

	bool removeAttribute(const char * name) {
		return removeAttribute(getAttribute(name));
	}

	bool hasAttributes()
	{
		if (0 < getNAttributes())
			return true;
		return false;
	}

	////////////////////////////////////////////////
	//	Attribute (Extention)
	////////////////////////////////////////////////

public:

	void setAttribute(const char * name, const char * value);

	void setAttribute(const char *name, int value);

	const char *getAttributeValue(const char * name);

	int getAttributeIntegerValue(const char *name);

	////////////////////////////////////////////////
	//	Attribute (xmlns)
	////////////////////////////////////////////////

	void setNameSpace(const char * ns, const char * val)
	{
		std::string nspace;
		nspace = "xmlns:";
		nspace += ns;
		setAttribute(nspace.c_str(), val);
	}

	////////////////////////////////////////////////
	//	Child node
	////////////////////////////////////////////////

	int getNNodes() {
		return nodeList.size();
	}

	Node *getNode(int index) {
		return nodeList.getNode(index);
	}

	Node *getNode(const char * name)
	{
		return nodeList.getNode(name);
	}

	Node *getNodeEndsWith(const char * name)
	{
		return nodeList.getEndsWith(name);
	}

	void addNode(Node *node) {
		node->setParentNode(this);
		nodeList.addNode(node);
	}

	void insertNode(Node *node, int index) {
		node->setParentNode(this);
		nodeList.insertNode(node, index);
	}

	bool removeNode(Node *node) {
		node->setParentNode(NULL);
		return nodeList.removeNode(node);
	}

	bool removeNode(const char * name) {
		return nodeList.removeNode(getNode(name));
	}

	bool hasNodes()
	{
		if (0 < getNNodes())
			return true;
		return false;
	}

	void removeAllNodes(void)
	{
		nodeList.clear();
	}

	////////////////////////////////////////////////
	//	Element (Child Node)
	////////////////////////////////////////////////

	void setNode(const char * name, const char * value) {
		Node *node = getNode(name);
		if (node != NULL) {
			node->setValue(value);
			return;
		}
		node = new Node(name);
		node->setValue(value);
		addNode(node);
	}

	const char *getNodeValue(const char * name) {
		Node *node = getNode(name);
		if (node != NULL)
			return node->getValue();
		return "";
	}

	////////////////////////////////////////////////
	//	userData
	////////////////////////////////////////////////

	void setUserData(NodeData *data);

	NodeData *getUserData()
	{
		return userData;
	}

	////////////////////////////////////////////////
	//	toString
	////////////////////////////////////////////////

protected:

	const char *getIndentLevelString(int nIndentLevel, std::string &buf)
	{
		buf = "";
		for (int n=0; n<nIndentLevel; n++)
			buf.append("\t");
		return buf.c_str();
	}

public:

#ifndef NO_USE_OSTRINGSTREAM
	void outputAttributes(std::ostream& ps);
#else
	void outputAttributes(std::string& ps);
#endif

public:

#ifndef NO_USE_OSTRINGSTREAM
	virtual void output(std::ostream& ps, int indentLevel, bool hasChildNode);
#else
	virtual void output(std::string& ps, int indentLevel, bool hasChildNode);
#endif

	const char *toString(std::string &buf, bool hasChildNode = true);

	const char *toUnicodeString(std::string &buf, bool hasChildNode = true);

	void print(bool hasChildNode = true);

};

}

#endif
