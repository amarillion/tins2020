#include "json.h"
#include <allegro5/allegro.h>
#include "util.h"
#include <sstream>

using namespace std;

int JsonNode::getInt(const string &key) {
	assert(nodeType == JSON_OBJECT);
	assert(objectValues.find(key) != objectValues.end());
	JsonNode child = objectValues[key];
	assert(child.nodeType == JSON_INT_LITERAL);
	return child.intLiteral;
}

bool JsonNode::getBool(const string &key) {
	assert(nodeType == JSON_OBJECT);
	assert(objectValues.find(key) != objectValues.end());
	JsonNode child = objectValues[key];
	assert(child.nodeType == JSON_BOOLEAN);
	return child.boolLiteral;
}

string JsonNode::getString(const string &key) {
	assert(nodeType == JSON_OBJECT);
	assert(objectValues.find(key) != objectValues.end());
	JsonNode child = objectValues[key];
	assert(child.nodeType == JSON_STRING_LITERAL);
	return child.stringLiteral;
}

vector<JsonNode> JsonNode::getArray(const string &key) {
	assert(nodeType == JSON_OBJECT);
	assert(objectValues.find(key) != objectValues.end());
	JsonNode child = objectValues[key];
	assert(child.nodeType == JSON_ARRAY);
	return child.listValues;
}

void JsonNode::arrayAdd(const JsonNode &item) {
	assert(nodeType == JSON_ARRAY);
	listValues.push_back(item);
}

void JsonNode::toStream(ostream &ss) {
	switch (nodeType) {
		case JSON_ARRAY: {
				string sep = "[ ";
				for (auto i : listValues) {
					ss << sep;
					sep = ", ";
					i.toStream(ss);
				}
				ss << " ]";
			}
			break;
		case JSON_OBJECT: {
				string sep = "{ ";
				// TODO: sort keys!
				for (auto pair : objectValues) {
					ss << sep;
					sep = ", ";
					ss << "\"" << pair.first << "\": "; 
					pair.second.toStream(ss);
				}
				ss << " }";
			}
			break;
		case JSON_BOOLEAN:
				ss << (boolLiteral ? "true" : "false");
			break;
		case JSON_DOUBLE:
				ss << doubleLiteral;
			break;
		case JSON_INT_LITERAL:
				ss << intLiteral;
			break;
		case JSON_STRING_LITERAL:
				ss << "\"" << stringLiteral << "\"";
			break;
	}
}

std::string JsonNode::toString() {
	stringstream ss;
	toStream(ss);
	return ss.str();
}

void JsonNode::setString(const std::string &key, const std::string &value) {
	assert(nodeType == JSON_OBJECT);
	objectValues[key] = JsonNode::fromString(value);
}

void JsonNode::setDouble(const std::string &key, double value) {
	assert(nodeType == JSON_OBJECT);
	objectValues[key] = JsonNode::fromDouble(value);
}

enum class TokenType { /** value that can not actually be produced by tokenizer */ DUMMY ,
		BRACE_OPEN, BRACE_CLOSE, COMMA, COLON, BRACKET_OPEN, BRACKET_CLOSE, QUOTED_STRING, KEYWORD, NUMBER, END };


class JsonException: public exception
{
private:
	std::string msg;
public:
	JsonException(const std::string &msg) : msg(msg) {}
	virtual const char* what() const throw()
	{
		return msg.c_str();
	}
};

class Token {
public:
	TokenType type;
	string value;

	Token() : type(TokenType::DUMMY), value() {}
	Token(TokenType type) : type(type), value() {}
};

class JsonTokenizer {
public:
	JsonTokenizer(const string &data) : data(data) {
		next = nextToken();
	}
private:
	string data;
	int pos = 0;
	int lineno = 1;
	int col = 1;

	Token next;

	void advancePos() {
		if (data[pos] == '\n') {
			lineno++;
			col = 0;
		}
		col++;
		pos++;
	}

	void skipWhitespace() {
		while (true) {
			if (pos >= data.size()) break;

			int c = data[pos];
			if (c == '\r' || c == '\n' || c == '\t' || c == ' ') {
				advancePos();
			}
			else {
				break;
			}
		}
	}

	Token readSingleToken() {
		Token result;
		switch (data[pos]) {
			case ':':
				result = Token(TokenType::COLON); break;
			case ',':
				result = Token(TokenType::COMMA); break;
			case '[':
				result = Token(TokenType::BRACKET_OPEN); break;
			case ']':
				result = Token(TokenType::BRACKET_CLOSE); break;
			case '{':
				result = Token(TokenType::BRACE_OPEN); break;
			case '}':
				result = Token(TokenType::BRACE_CLOSE); break;
			default:
				assert(false);
		}
		advancePos();
		return result;
	}

	Token readQuotedString() {
		int quote = data[pos];
		advancePos();
		int start = pos;
		Token result = Token(TokenType::QUOTED_STRING);

		while (true) {
			assert (pos < data.size()); // unterminated quoted string

			int c = data[pos];
			if (c == '\r' || c == '\n') {
				assert(false); // newline inside quoted string not allowed
			}
			else if (c == quote) {
				result.value = data.substr(start, pos-start);
				advancePos();
				break;
			}
			else {
				advancePos();
			}
		}
		return result;
	}

	Token readNumber() {
		int start = pos;
		Token result = Token(TokenType::NUMBER);

		while (true) {

			if (pos >= data.size()) { break; }

			int c = data[pos];
			if ((c >= '0' && c <= '9') || c == '-' || c == '+' || c == 'e' || c == 'E' || c == '.') {
				advancePos();
			}
			else {
				break;
			}
		}
		result.value = data.substr(start, pos - start);
		return result;

	}

	Token readKeyword() {
		int start = pos;
		Token result = Token(TokenType::KEYWORD);

		while (true) {

			if (pos >= data.size()) { break; }

			int c = data[pos];
			if (c >= 'a' && c <= 'z') {
				advancePos();
			}
			else {
				break;
			}
		}
		result.value = data.substr(start, pos - start);
		return result;
	}

	Token nextToken() {
		skipWhitespace();

		if (pos >= data.size()) {
			return Token(TokenType::END);
		}

		int c = data[pos];
		switch (c) {

		case ':': case ',': case '[': case ']': case '{': case '}':
			return readSingleToken(); break;
		case '"': case '\'':
			return readQuotedString(); break;
		case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
		case '-': case '.':
			return readNumber(); break;
		case 't':
		case 'f':
		case 'n':
			return readKeyword(); break;
		default:
			//TODO: this message is not copied correctly to function???
			throw JsonException("Unexpected character: " + data[pos]);
		}
	}
public:
	TokenType peek() {
		return next.type;
	}

	Token get() {
		Token result = next;
		next = nextToken();
		return result;
	}
};

class JsonParser {
private:
	JsonTokenizer &tokenizer;

	JsonNode parseObjectNode() {
		consume(TokenType::BRACE_OPEN);

		JsonNode parent = JsonNode(JSON_OBJECT);

		while(true) {
			Token key = tokenizer.get();
			assert (key.type == TokenType::QUOTED_STRING);

			consume(TokenType::COLON);

			parent.objectValues[key.value] = parseNode();

			expect (TokenType::COMMA, TokenType::BRACE_CLOSE);

			TokenType type = tokenizer.peek();
			if (type == TokenType::BRACE_CLOSE) {
				break;
			}

			consume(TokenType::COMMA);
		}

		consume(TokenType::BRACE_CLOSE);
		return parent;
	}

	JsonNode parseArrayNode() {
		consume(TokenType::BRACKET_OPEN);

		JsonNode parent = JsonNode(JSON_ARRAY);

		while (true) {
			JsonNode n = parseNode();
			parent.listValues.push_back(n);

			expect (TokenType::COMMA, TokenType::BRACKET_CLOSE);
			TokenType type = tokenizer.peek();
			if (type == TokenType::BRACKET_CLOSE) {
				break;
			}

			consume(TokenType::COMMA);
		}
		consume(TokenType::BRACKET_CLOSE);
		return parent;
	}

	JsonNode parseStringNode() {
		Token t = tokenizer.get();
		return JsonNode(JSON_STRING_LITERAL, t.value);
	}

	JsonNode parseNumberNode() {
		Token t = tokenizer.get();
		return JsonNode::fromInt(stoi(t.value));
	}

	JsonNode parseBooleanNode() {
		Token t = tokenizer.get();
		if (t.value == "true") {
			return JsonNode::fromBool(true);
		}
		else if (t.value == "false") {
			return JsonNode::fromBool(false);
		}
		else {
			throw JsonException ("Unknown keyword when creating a boolean node");
		}
	}

	JsonNode parseNode() {

		TokenType type = tokenizer.peek();
		if (type == TokenType::BRACE_OPEN) {
			return parseObjectNode();
		}
		else if (type == TokenType::BRACKET_OPEN) {
			return parseArrayNode();
		}
		else if (type == TokenType::QUOTED_STRING) {
			return parseStringNode();
		}
		else if (type == TokenType::NUMBER) {
			return parseNumberNode();
		}
		else if (type == TokenType::KEYWORD) {
			//TODO: could be value 'true' or 'false', but also 'null', 'undefined' etc?
			return parseBooleanNode();
		}
		throw JsonException ("Unexpected token type");
	}

public:
	JsonParser(JsonTokenizer &tokenizer) : tokenizer(tokenizer) {

	}

	void consume(TokenType type) {
		Token t = tokenizer.get();
		assert (t.type == type);
	}

	void expect(TokenType opt1, TokenType opt2 = TokenType::DUMMY) {

		TokenType type = tokenizer.peek();
		assert (type == opt1 ||
				(opt2 == TokenType::DUMMY || type == opt2)
		);
	}

	JsonNode parse() {

		JsonNode result = parseNode();
		consume(TokenType::END);
		return result;
	}

};

JsonNode jsonParseString(const std::string &data) {
	JsonTokenizer tokenizer = JsonTokenizer(data);
	JsonParser parser = JsonParser(tokenizer);
	return parser.parse();
}

JsonNode jsonParseFile(const std::string &fileName) {
	string result("");

	char buffer[4096];
	ALLEGRO_FILE *fp = al_fopen(fileName.c_str(), "r");
	while(al_fgets(fp, buffer, 4096))
	{
		result += buffer;
	}
	al_fclose(fp);

	return jsonParseString(result);
}
