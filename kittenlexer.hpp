#ifndef KITTEN_LEXER_HPP
#define KITTEN_LEXER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <stack>
#include <map>

struct KittenToken {
	std::string src;
	bool str = false;
    unsigned long line = 0;
};

using lexed_kittens = std::vector<KittenToken>;

class KittenLexer {
	using storage = std::map<char,bool>;
	storage stringqs;
	std::vector<std::tuple<char,char>> capsules;
	storage ignores;
    storage as_newline;
	storage extracts;
	bool erase_emptys = false;
public:

	inline bool is_stringq(char c) const { 
		return stringqs.count(c) > 0;
	}
	inline bool is_capsule(char open, char close) const {
		for(auto tu : capsules) {
            auto [o,c] = tu;
			if(o == open || o == close || c == open || c == close) {
				return true;
			}
		}
		return false;
	}
	inline bool is_capsule_open(char ch) const {
		for(auto tu : capsules) {
            auto [o,c] = tu;
			if(o == ch) {
				return true;
			}
		}
		return false;
	}
	inline bool is_capsule_close(char ch) const {
		for(auto tu : capsules) {
            auto [o,c] = tu;
			if(c == ch) {
				return true;
			}
		}
		return false;
	}
	inline bool match_closure(char open, char close) const {
		for(auto tu : capsules) {
            auto [o,c] = tu;
			if(o == open && c == close) {
				return true;
			}
		}
		return false;
	}
	inline bool is_ignore(char c) const {
		return ignores.count(c) > 0;
	}
    inline bool is_newline(char c) const {
        return as_newline.count(c) > 0;
    }

	inline bool is_extract(char c) const {
		return extracts.count(c) > 0;
	}

	inline KittenLexer& add_capsule(char open, char close) {
		if(!is_capsule(open,close))
			capsules.push_back(std::make_tuple(open,close));
		return *this;
	}

	inline KittenLexer& add_stringq(char c) {
		if(!is_stringq(c))
			stringqs[c] = true;
		return *this;
	}

	inline KittenLexer& add_ignore(char c) {
		if(!is_ignore(c))
			ignores[c] = true;
		return *this;
	}

	inline KittenLexer& add_extract(char c) {
		if(!is_extract(c))
			extracts[c] = true;
		return *this;
	}

	inline KittenLexer& erase_empty() {
		this->erase_emptys;
		return *this;
	}

	inline lexed_kittens lex(std::string src) {
		lexed_kittens ret;
		std::stack<char> opens;
		std::string tmp;
		bool next_string = false;
		std::stack<char> stringqs;	

		for(auto i : src) {
			if(stringqs.empty() && opens.empty() && is_ignore(i)) {
				if(next_string || (tmp != "" || !erase_emptys)) {
					ret.push_back(KittenToken{tmp,next_string});
					tmp = "";
				}
			}
			else if(stringqs.empty() && is_capsule_open(i)) {
                opens.push(i);
                tmp += i;
			}
			else if(stringqs.empty() && is_capsule_close(i)) {
				if(opens.empty()) {
					return {};
				}
				if(!match_closure(opens.top(),i)) {
					return {};	
				}
				opens.pop();
                tmp += i;
                ret.push_back(KittenToken{tmp,next_string});
				next_string = false;
				tmp = "";
			}
			else if(opens.empty() && is_stringq(i)) {
				if(stringqs.empty()) {
					stringqs.push(i);
				}
				else if(stringqs.top() == i) {
					stringqs.pop();
					next_string = true;
				}
				else {
					tmp += i;
				}
			}
			else if(opens.empty() && stringqs.empty() && is_extract(i)) {
				ret.push_back(KittenToken{tmp,next_string});
				next_string = false;
				tmp = i;
				ret.push_back(KittenToken{tmp,next_string});
				tmp = "";
			}
			else {
				tmp += i;
			}
		}
		if(tmp != "" || !erase_emptys) {
			ret.push_back(KittenToken{tmp,next_string});
			next_string = false;
		}
		return ret;
	}
};

#endif
