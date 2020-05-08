#ifndef __TWIST5_JSON_H__
#define __TWIST5_JSON_H__

#include <map>
#include <string>
#include <vector>

enum { JSON_STRING_LITERAL, JSON_ARRAY, JSON_INT_LITERAL, JSON_DOUBLE, JSON_OBJECT, JSON_BOOLEAN };

class JsonNode {
public:
	int nodeType;

	union {
		int intLiteral;
		double doubleLiteral;
		bool boolLiteral;
	};

	std::string stringLiteral;
	std::map<std::string, JsonNode> objectValues;
	std::vector<JsonNode> listValues;

	JsonNode () {
		//TODO - remove me. should not be possible to instantiate...
	};

	JsonNode (int type) : nodeType(type) {}

	JsonNode (int type, std::string stringLiteral) {
		nodeType = type; this->stringLiteral = stringLiteral;
	}

	static JsonNode fromInt(int val) {
		JsonNode result(JSON_INT_LITERAL);
		result.intLiteral = val;
		return result;
	}

	static JsonNode fromBool(bool val) {
		JsonNode result(JSON_BOOLEAN);
		result.boolLiteral = val;
		return result;
	}

	static JsonNode fromString(const std::string &val) {
		JsonNode result(JSON_STRING_LITERAL);
		result.stringLiteral = val;
		return result;
	}

	static JsonNode fromDouble(double val) {
		JsonNode result(JSON_DOUBLE);
		result.doubleLiteral = val;
		return result;
	}

	int getInt(const std::string &key);
	bool getBool(const std::string &key);
	std::string getString(const std::string &key);
	std::vector<JsonNode> getArray(const std::string &key);
	void arrayAdd(const JsonNode &item);

	// render Json body recursively to string
	std::string toString();
	void toStream(std::ostream &os);
	void setString(const std::string &key, const std::string &value);
	void setDouble(const std::string &key, double value);
};

JsonNode jsonParseFile(const std::string &fileName);
JsonNode jsonParseString(const std::string &data);

#endif
