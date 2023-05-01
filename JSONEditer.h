#pragma once

#include "resource.h"
#include <list>
#include <map>
#include <string>
#include <cstdint>
#include <vector>
#include <typeinfo>

enum class jsonType
{
	jInit = 0,
	jBool = 1,
	jNumber = 2,
	jString = 3,
	jNull = 4,
	jArray = 5,
	jObject = 6,
};

class JsonElement {
private:
	std::string m_name;
	bool nextFlag;
	jsonType nextType;
	void* next;
public:
	JsonElement(std::string name) :m_name(name), nextFlag(false), nextType(jsonType::jInit), next(nullptr) {

	}
	virtual jsonType getType() {
		return jsonType::jInit;
	}
	void* getNext() {
		return next;
	}
	jsonType getNextType() {
		return nextType;
	}
	void setNext(void* other, jsonType _type) {
		nextType = _type;
		next = other;
		nextFlag = true;
	}
	void removeNext() {
		end();
	}
	bool hasNext() {
		return nextFlag;
	}
	virtual void end() = 0;
};

class JsonObject;
class JsonArray;
class JsonBool;
class JsonNumber;
class JsonString;
class JsonNull;

class JsonObject:public JsonElement {
private:
	int m_size;
	std::vector<void*> m_elementVect;
	jsonType endType;
public:
	JsonObject(std::string name) :JsonElement(name), m_size(0), m_elementVect(std::vector<void*>()), endType(jsonType::jInit) {
		
	}
private:
	void add(jsonType type, void* other);
public:
	void add(std::string name, bool flag);
	void add(std::string name, std::int64_t num);
	void add(std::string name, std::string str);
	void add(std::string name, int _null);
	template<typename TYPE>
	void add(std::string name, TYPE element);
	void add(std::string name);
	jsonType getType() override {
		return jsonType::jObject;
	}
	void end() override;
};

class JsonArray:public JsonElement {
private:
	std::vector<void*> m_vector;
	jsonType elementType;
public:
	template<typename TYPE>
	JsonArray(std::string name, TYPE* other);
	void* get(int index) {
		if (m_vector.size() <= (unsigned int)index || index < 0) {
			index = 0;
		}
		/*
		switch (elementType)
		{
		case jsonType::jInit:
			break;
		case jsonType::jBool:
			return static_cast<JsonBool*>(m_vector[index]);
			break;
		case jsonType::jNumber:
			return static_cast<JsonNumber*>(m_vector[index]);
			break;
		case jsonType::jString:
			return static_cast<JsonString*>(m_vector[index]);
			break;
		case jsonType::jNull:
			return static_cast<JsonNull*>(m_vector[index]);
			break;
		case jsonType::jArray:
			return static_cast<JsonArray*>(m_vector[index]);
			break;
		case jsonType::jObject:
			return static_cast<JsonObject*>(m_vector[index]);
			break;
		default:
			break;
		}
		*/
		return m_vector[index];
	}
	std::vector<void*> getAll() {
		return m_vector;
	}
	template<typename TYPE>
	void add(TYPE* other);
	template<typename TYPE>
	void insert(TYPE* other, int index);
	void remove(int index);
	jsonType getType() override {
		return jsonType::jArray;
	}
	void end() override;
};

class JsonBool:public JsonElement {
private:
	bool m_flag;
public:
	JsonBool(std::string name, bool flag) : JsonElement(name) {
		m_flag = flag;
	}
	bool get() {
		return m_flag;
	}
	void set(bool flag) {
		m_flag = flag;
	}
	jsonType getType() override {
		return jsonType::jBool;
	}
	void end() override;
};

class JsonNumber:public JsonElement {
private:
	std::int64_t m_num;
public:
	JsonNumber(std::string name) : JsonElement(name), m_num(0) {

	}
	JsonNumber(std::string name, std::int64_t num) :JsonElement(name) {
		m_num = num;
	}
	/*
	JsonNumber(std::string name, int num):JsonElement(name) {
		m_num = static_cast<std::int64_t>(num);
	}
	*/
	std::int64_t get() {
		return m_num;
	}
	void set(std::int64_t num) {
		m_num = num;
	}
	/*
	void set(int num) {
		m_num = static_cast<std::int64_t>(num);
	}
	*/
	jsonType getType() override {
		return jsonType::jNumber;
	}
	void end() override;
};

class JsonString:public JsonElement {
private:
	std::string m_str;
public:
	JsonString(std::string name) : JsonElement(name), m_str("") {
		
	}
	JsonString(std::string name, std::string str) :JsonElement(name) {
		m_str = str;
	}
	std::string get() {
		return m_str;
	}
	void set(std::string str) {
		m_str = str;
	}
	jsonType getType() override {
		return jsonType::jString;
	}
	void end() override;
};

class JsonNull:public JsonElement {
public:
	JsonNull(std::string name) : JsonElement(name) {

	}
	int get() {
		return NULL;
	}
	jsonType getType() override {
		return jsonType::jNull;
	}
	void end() override;
};

//----------
// JsonObject method
//----------
void JsonObject::add(jsonType type, void* other) {
	switch (endType)
	{
	case jsonType::jInit:
		setNext(other, type);
		m_elementVect.push_back(other);
		endType = type;
		break;
	case jsonType::jBool:
	{
		JsonBool* endOther = static_cast<JsonBool*>(m_elementVect.back());
		endOther->setNext(other, type);
		m_elementVect.push_back(other);
		endType = type;
		break;
	}
	case jsonType::jNumber:
	{
		JsonNumber* endOther = static_cast<JsonNumber*>(m_elementVect.back());
		endOther->setNext(other, type);
		m_elementVect.push_back(other);
		endType = type;
		break;
	}
	case jsonType::jString:
	{
		JsonString* endOther = static_cast<JsonString*>(m_elementVect.back());
		endOther->setNext(other, type);
		m_elementVect.push_back(other);
		endType = type;
		break;
	}
	case jsonType::jNull:
	{
		JsonNull* endOther = static_cast<JsonNull*>(m_elementVect.back());
		endOther->setNext(other, type);
		m_elementVect.push_back(other);
		endType = type;
		break;
	}
	case jsonType::jArray:
	{
		JsonArray* endOther = static_cast<JsonArray*>(m_elementVect.back());
		endOther->setNext(other, type);
		m_elementVect.push_back(other);
		endType = type;
		break;
	}
	case jsonType::jObject:
	{
		JsonObject* endOther = static_cast<JsonObject*>(m_elementVect.back());
		endOther->setNext(other, type);
		m_elementVect.push_back(other);
		endType = type;
		break;
	}
	default:
		break;
	}
}
void JsonObject::add(std::string name, bool flag) {
	add(jsonType::jBool, new JsonBool(name, flag));
}
void JsonObject::add(std::string name, std::int64_t num) {
	add(jsonType::jNumber, new JsonNumber(name, num));
}
void JsonObject::add(std::string name, std::string str) {
	add(jsonType::jString, new JsonString(name, str));
}
void JsonObject::add(std::string name, int _null) {
	add(jsonType::jNull, new JsonNull(name));
}
template<typename TYPE>
void JsonObject::add(std::string name, TYPE element) {
	if (typeid(TYPE) == typeid(bool)) {
		add(jsonType::jArray, new JsonArray(name, new JsonBool("", element)));
	}
	else if (typeid(TYPE) == typeid(std::int64_t)) {
		add(jsonType::jArray, new JsonArray(name, new JsonNumber("", element)));
	}
	else if (typeid(TYPE) == typeid(std::string)) {
		add(jsonType::jArray, new JsonArray(name, new JsonString("", element)));
	}
}
void JsonObject::add(std::string name) {
	add(jsonType::jObject, new JsonObject(name));
}
void JsonObject::end() {
	if (this->hasNext()) {
		switch (this->getNextType())
		{
		case jsonType::jInit:
			break;
		case jsonType::jBool:
		{
			JsonBool* tempPointer = static_cast<JsonBool*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jNumber:
		{
			JsonNumber* tempPointer = static_cast<JsonNumber*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jString:
		{
			JsonString* tempPointer = static_cast<JsonString*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jNull:
		{
			JsonNull* tempPointer = static_cast<JsonNull*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jArray:
		{
			JsonArray* tempPointer = static_cast<JsonArray*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jObject:
		{
			JsonObject* tempPointer = static_cast<JsonObject*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		default:
			break;
		}
		delete this;
	}
}

//----------
// JsonArray method
//----------
template<typename TYPE>
JsonArray::JsonArray(std::string name, TYPE* other) :JsonElement(name) {
	elementType = other->getType();
	m_vector.push_back(other);
}
template<typename TYPE>
void JsonArray::add(TYPE* other) {
	if (other->getType() == elementType) {
		m_vector.push_back(other);
		return;
	}
	delete other;
}
template<typename TYPE>
void JsonArray::insert(TYPE* other, int index) {
	if (m_vector.size() >= index && index >= 0) {
		if (other->getType() == elementType) {
			m_vector.insert(m_vector.begin() + index, other);
			return;
		}
	}
	delete other;
}
void JsonArray::remove(int index) {
	if (m_vector.size() > (unsigned int)index && index >= 0) {
		void* other = this->get(index);
		m_vector.erase(m_vector.begin() + index);
		switch (elementType)
		{
		case jsonType::jInit:
			break;
		case jsonType::jBool:
			static_cast<JsonBool*>(m_vector[index])->end();
			break;
		case jsonType::jNumber:
			static_cast<JsonNumber*>(m_vector[index])->end();
			break;
		case jsonType::jString:
			static_cast<JsonString*>(m_vector[index])->end();
			break;
		case jsonType::jNull:
			static_cast<JsonNull*>(m_vector[index])->end();
			break;
		case jsonType::jArray:
			static_cast<JsonArray*>(m_vector[index])->end();
			break;
		case jsonType::jObject:
			static_cast<JsonObject*>(m_vector[index])->end();
			break;
		default:
			break;
		}
		delete other;
	}
}
void JsonArray::end() {
	if (this->hasNext()) {
		switch (this->getNextType())
		{
		case jsonType::jInit:
			break;
		case jsonType::jBool:
		{
			JsonBool* tempPointer = static_cast<JsonBool*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jNumber:
		{
			JsonNumber* tempPointer = static_cast<JsonNumber*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jString:
		{
			JsonString* tempPointer = static_cast<JsonString*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jNull:
		{
			JsonNull* tempPointer = static_cast<JsonNull*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jArray:
		{
			JsonArray* tempPointer = static_cast<JsonArray*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jObject:
		{
			JsonObject* tempPointer = static_cast<JsonObject*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		default:
			break;
		}
		delete this;
	}
}

//----------
// JsonBool
//----------
void JsonBool::end() {
	if (this->hasNext()) {
		switch (this->getNextType())
		{
		case jsonType::jInit:
			break;
		case jsonType::jBool:
		{
			JsonBool* tempPointer = static_cast<JsonBool*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jNumber:
		{
			JsonNumber* tempPointer = static_cast<JsonNumber*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jString:
		{
			JsonString* tempPointer = static_cast<JsonString*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jNull:
		{
			JsonNull* tempPointer = static_cast<JsonNull*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jArray:
		{
			JsonArray* tempPointer = static_cast<JsonArray*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jObject:
		{
			JsonObject* tempPointer = static_cast<JsonObject*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		default:
			break;
		}
		delete this;
	}
}

//----------
// JsonNumber
//----------
void JsonNumber::end() {
	if (this->hasNext()) {
		switch (this->getNextType())
		{
		case jsonType::jInit:
			break;
		case jsonType::jBool:
		{
			JsonBool* tempPointer = static_cast<JsonBool*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jNumber:
		{
			JsonNumber* tempPointer = static_cast<JsonNumber*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jString:
		{
			JsonString* tempPointer = static_cast<JsonString*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jNull:
		{
			JsonNull* tempPointer = static_cast<JsonNull*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jArray:
		{
			JsonArray* tempPointer = static_cast<JsonArray*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jObject:
		{
			JsonObject* tempPointer = static_cast<JsonObject*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		default:
			break;
		}
		delete this;
	}
}

//----------
// JsonString
//----------
void JsonString::end() {
	if (this->hasNext()) {
		switch (this->getNextType())
		{
		case jsonType::jInit:
			break;
		case jsonType::jBool:
		{
			JsonBool* tempPointer = static_cast<JsonBool*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jNumber:
		{
			JsonNumber* tempPointer = static_cast<JsonNumber*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jString:
		{
			JsonString* tempPointer = static_cast<JsonString*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jNull:
		{
			JsonNull* tempPointer = static_cast<JsonNull*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jArray:
		{
			JsonArray* tempPointer = static_cast<JsonArray*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jObject:
		{
			JsonObject* tempPointer = static_cast<JsonObject*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		default:
			break;
		}
		delete this;
	}
}

//----------
// JsonNull
//----------
void JsonNull::end() {
	if (this->hasNext()) {
		switch (this->getNextType())
		{
		case jsonType::jInit:
			break;
		case jsonType::jBool:
		{
			JsonBool* tempPointer = static_cast<JsonBool*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jNumber:
		{
			JsonNumber* tempPointer = static_cast<JsonNumber*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jString:
		{
			JsonString* tempPointer = static_cast<JsonString*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jNull:
		{
			JsonNull* tempPointer = static_cast<JsonNull*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jArray:
		{
			JsonArray* tempPointer = static_cast<JsonArray*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		case jsonType::jObject:
		{
			JsonObject* tempPointer = static_cast<JsonObject*>(this->getNext());
			tempPointer->end();
			//delete tempPointer;
			break;
		}
		default:
			break;
		}
		delete this;
	}
}

class JEData {
private:
	JsonObject* m_jsonData;
	std::map<std::string, JsonObject*> m_ObjectMap;
	JsonObject* m_currentObject;
public:
	JEData() : m_jsonData(new JsonObject("test.json")), m_ObjectMap(std::map<std::string, JsonObject*>()), m_currentObject(nullptr) {
		m_ObjectMap.insert(std::make_pair("test.json", m_jsonData));
		m_currentObject = m_ObjectMap["test.json"];
	}
	JEData(std::string name) : m_jsonData(new JsonObject(name)), m_ObjectMap(std::map<std::string, JsonObject*>()), m_currentObject(nullptr) {
		m_ObjectMap.insert(std::make_pair(name, m_jsonData));
		m_currentObject = m_ObjectMap[name];
	}
	~JEData() {
		m_jsonData->end();
		delete m_jsonData;
	}
};

class JEWindowList {
private:
	
};
