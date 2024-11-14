// makes an easier interface for printing colored console text

#ifndef GLEEPYGLORPYTRAIL_STYLEBANK_H
#define GLEEPYGLORPYTRAIL_STYLEBANK_H

#include <map>
#include <vector>
#include <regex>
#include <string>
using namespace std;

class OutputManager {
    const map<string, string> STYLE_BANK = {
            // text styling options
            {"regular", "0"},
            {"b", "1"},
            {"i", "3"},
            {"ul", "4"},
            {"hl", "8"},
            {"st", "9"},

            // color options
            {"black", "30"},
            {"red", "31"},
            {"green", "32"},
            {"yellow", "33"},
            {"blue", "34"},
            {"magenta", "35"},
            {"cyan", "36"},
            {"white", "37"},
            {"b_black", "90"},
            {"b_red", "91"},
            {"b_green", "92"},
            {"b_yellow", "93"},
            {"b_blue", "94"},
            {"b_magenta", "95"},
            {"b_cyan", "96"},
            {"b_white", "97"},
            {"default", "97"}
    };

    const string ESCAPE_CODE = "\033[";

    string current_color = "default";
    vector<string> color_stack;

    string current_style = "regular";
    vector<string> style_stack;

    string escape(const string& code) const {
        return ESCAPE_CODE + this->STYLE_BANK.at(code) + "m";
    }

public:
    // no constructor necessary since default behavior is OK for our needs
    OutputManager() {
        c_push("default");
        s_push("regular");
    }

    // clears the console
    void clear() {
        system("clear");
    }

    // set the output text's color
    string c_set(string col = "default") {
        // find() returns .end() iterator when key does not exist
        current_color = (STYLE_BANK.find(col) == STYLE_BANK.end()) ? "default" : col;
        return escape(current_color);
    }

    // push the current output text color to the color stack, and set the current
    // color to a new one
    string c_push(string col = "default") {
        color_stack.push_back(current_color);
        return c_set(col);
    }

    // set the output text color to the color at the top of the color stack
    string c_pop() {
        // if the stack is empty, throw an error; likely a c_push() command is missing
        if (color_stack.empty()) {
            throw out_of_range("Invalid c_pop(); stack size is 0");
        }

        current_color = color_stack.back();
        color_stack.pop_back();

        return escape(current_color);
    }

    string s_set(string st = "regular") {
        // find() returns .end() iterator when key does not exist
        current_style = (STYLE_BANK.find(st) == STYLE_BANK.end()) ? "regular" : st;
        return escape(current_style);
    }

    // push the current output text color to the color stack
    string s_push(string st = "regular") {
        style_stack.push_back(current_style);
        return s_set(st);
    }

    // set the output text color to the color at the top of the color stack
    string s_pop() {
        // if the stack is empty, throw an error; likely a c_push() command is missing
        if (style_stack.empty()) {
            throw out_of_range("Invalid s_pop(); stack size is 0");
        }

        current_style = style_stack.back();
        style_stack.pop_back();

        return escape(current_style);
    }

    void reset() {
        color_stack = vector<string>();
        style_stack = vector<string>();
        current_color = "";
        current_style = "";
    }

    // implements a special print() method which parses strings for color/style
    // tags, making the process of printing styled text much simpler
    void print(string msg, int cnt = 1) {
        msg.insert(0, "[default]");
        msg.append("[/default]");

        const regex tag("\\[[\\/a-z-_]*\\]");

        // this loop replaces user-friendly tags with their escape-sequence equivalent
        smatch m;
        while (regex_search(msg, m, tag)) {
            string t = m.str().substr(1, m.str().length() - 2);

            // closing tag
            if (t[0] == '/') {
                string code = STYLE_BANK.at(t.substr(1));
                if (stoi(code) >= 30) msg.replace(m.position(), m.length(), s_set(current_style) + c_pop());
                else msg.replace(m.position(), m.length(), s_pop() + c_set(current_color));
            }

                // opening tag
            else {
                string code = STYLE_BANK.at(t);
                // color tag
                if (stoi(code) >= 30) msg.replace(m.position(), m.length(), c_push(t));
                else msg.replace(m.position(), m.length(), s_push(t)); // style tag
            }
        }

        for (unsigned int i = 0; i < cnt; i++) {
            cout << msg;
        }
    }
};

#endif //GLEEPYGLORPYTRAIL_STYLEBANK_H
