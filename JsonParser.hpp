#pragma once
#include <vector>
#include "ObjModel.hpp"
#include <string>
#include "EngineUtils.hpp"

/* constants because I don't like the enum stuff*/
std::string J_COMMA = "COMMA";
std::string J_COLON = "COLON";
std::string J_CURLY_BRACE = "CURLY_BRACE";
std::string J_SQAURE_BRACE = "SQAURE_BRACE";
std::string J_IDENTIFIER = "IDENTIFIER";
std::string J_INT = "INT";
std::string J_FLOAT = "FLOAT";
std::string J_KEYWORD = "KEYWORD";

typedef struct JsonLexeme {
	std::string type, value;
} JsonLexeme;

typedef struct JsonTransformation {
	glm::vec3 location, rotation, scale;
} JsonTransformation;

typedef struct JsonSceneComponent {
	std::string modelPath;
	std::vector<JsonTransformation> transformations; //component can have more than one transformation if there are several instances of the same model
	
} JsonSceneComponent;

class JsonParser {
public:
	std::vector<JsonLexeme> lexemes;

	JsonParser() {}

	void lexFile(std::string raw_json) {
		int pos=0, forward=0;
		char cur;
		std::string val;

		while(pos < raw_json.length()) {
			cur = raw_json.at(pos);
			forward = pos;
			val = std::string();

			if (cur == ',') {
				val += cur;
				lexemes.push_back(JsonLexeme{ J_COMMA, val });
				pos++;
			}
			else if (cur == ':') {
				val += cur;
				lexemes.push_back(JsonLexeme{ J_COLON, val });
				pos++;
			}
			else if (cur == '{' || cur == '}') {
				val += cur;
				lexemes.push_back(JsonLexeme{ J_CURLY_BRACE, val });
				pos++;
			}
			else if (cur == '[' || cur == ']') {
				val += cur;
				lexemes.push_back(JsonLexeme{ J_SQAURE_BRACE, val });
				pos++;
			}
			else if (cur == '"') {
				/*read in an identifier or a keyword*/
				forward++;
				while (raw_json.at(forward) != '"')
					forward++;

				val = raw_json.substr(pos + 1, forward - (pos + 1));
				lexemes.push_back(JsonLexeme{ isKeyword(val) ? J_KEYWORD : J_IDENTIFIER, val});

				pos = forward + 1;
			}
			else if (in(cur, " \t\n")) {
				while (in(cur, " \t\n"))
					cur = raw_json.at(pos++);
				pos--;
			}
			else if (cur == '-' || in(cur, "0123456789")) {
				/*read a float in*/

				if(cur == '-')
					cur = raw_json.at(++forward);

				while (in(cur, "0123456789"))
					cur = raw_json.at(forward++);

				if(cur == '.')
					cur = raw_json.at(forward++);

				while (in(cur, "0123456789"))
					cur = raw_json.at(forward++);

				if(cur == 'e')
					cur = raw_json.at(forward++);

				if (cur == '-' || cur == '+')
					cur = raw_json.at(forward++);

				while (in(cur, "0123456789"))
					cur = raw_json.at(forward++);

				forward--;

				lexemes.push_back(JsonLexeme{ J_FLOAT, raw_json.substr(pos, forward - pos) });
				pos = forward;
			}
			else {
				printf("error on this character: %c at this position: %d\n", cur, pos);
				exit(0);
			}
		}
	}

	bool isKeyword(std::string val) {
		std::vector<std::string> keywords{ "location", "rotation", "scale"};
		for (std::string keyword : keywords) {
			if (!keyword.compare(val))
				return true;
		}
		return false;
	}

	std::vector<JsonSceneComponent> parseJson() {
		/*might want to hide the lex part and just make that call at the top of this function*/
		
		/*
		Actual grammar (which at the moment doesn't seem necessary to do a legit parse of)
		JSON -> { model } |  ---- { model add_model }
		add_model -> , model | , model add_model
		model -> { model_name : [ transformation ] }  | { model_name : [ transformation add_transformation ] }
		add_transformation -> , transformation | , transformation add_transformation
		transformation -> { float , float , float }

		model_name -> is a string
		float -> is a float
		*/

		/*Do a regular parse, but then also realize that you can build the scene as you go
		by keeping track of the state of your position in a parse tree. E.g., if you're parsing
		a transformation, you know that it's applied to whatever the current model is. 
		
		Basically can just go through and identify IDENTIFIER and then you'll necessarily
		have groups of three keywords with LOCATION, ROTATION, and SCALE, and*/

		std::vector<JsonSceneComponent> sceneComponents;
		int i = 0;
		while( i < lexemes.size()) { //shouldn't need this to be by reference as of now
			JsonLexeme lexeme = lexemes.at(i);

			if (!lexeme.type.compare(J_IDENTIFIER)) {
				JsonSceneComponent component;
				lexeme.value.append(".obj");
				component.modelPath = lexeme.value;
				sceneComponents.push_back(component);
				i++;
				continue;
			}

			if (!lexeme.type.compare(J_KEYWORD) && !lexeme.value.compare("location")) {
				float x, y, z; /*Todo: this can all be moved into a function, I'm just too tired to think about how references should work nicely right now*/
				sceneComponents.back().transformations.push_back(JsonTransformation{}); //push back only happens on location. Rotation and scale are part of the same transformation

				while (lexeme.type.compare(J_FLOAT))
					lexeme = lexemes.at(i++);
				x = std::stof(lexeme.value);
				lexeme = lexemes.at(i++);
				while (lexeme.type.compare(J_FLOAT))
					lexeme = lexemes.at(i++);
				y = std::stof(lexeme.value);
				lexeme = lexemes.at(i++);
				while (lexeme.type.compare(J_FLOAT))
					lexeme = lexemes.at(i++);
				z = std::stof(lexeme.value);
				lexeme = lexemes.at(i++);

				sceneComponents.back().transformations.back().location = glm::vec3(x, y, z);
				continue;
			}
			if (!lexeme.type.compare(J_KEYWORD) && !lexeme.value.compare("rotation")) {
				float x, y, z; /*Todo: this can all be moved into a function, I'm just too tired to think about how references should work nicely right now*/

				while (lexeme.type.compare(J_FLOAT))
					lexeme = lexemes.at(i++);
				x = std::stof(lexeme.value);
				lexeme = lexemes.at(i++);
				while (lexeme.type.compare(J_FLOAT))
					lexeme = lexemes.at(i++);
				y = std::stof(lexeme.value);
				lexeme = lexemes.at(i++);
				while (lexeme.type.compare(J_FLOAT))
					lexeme = lexemes.at(i++);
				z = std::stof(lexeme.value);
				lexeme = lexemes.at(i++);

				sceneComponents.back().transformations.back().rotation = glm::vec3(x, y, z);
				continue;
			}
			if (!lexeme.type.compare(J_KEYWORD) && !lexeme.value.compare("scale")) {
				float x, y, z; /*Todo: this can all be moved into a function, I'm just too tired to think about how references should work nicely right now*/

				while (lexeme.type.compare(J_FLOAT))
					lexeme = lexemes.at(i++);
				x = std::stof(lexeme.value);
				lexeme = lexemes.at(i++);
				while (lexeme.type.compare(J_FLOAT))
					lexeme = lexemes.at(i++);
				y = std::stof(lexeme.value);
				lexeme = lexemes.at(i++);
				while (lexeme.type.compare(J_FLOAT))
					lexeme = lexemes.at(i++);
				z = std::stof(lexeme.value);
				lexeme = lexemes.at(i++);

				sceneComponents.back().transformations.back().scale = glm::vec3(x, y, z);
				continue;
			}

			i++; //for all the other lexemes like commas that aren't currently used. At the top of the next loop, the lexeme will be gotten
		}

		return sceneComponents;
	}
};