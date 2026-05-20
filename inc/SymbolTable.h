#ifndef LEXER_SYMBOL_TABLE_H
#define LEXER_SYMBOL_TABLE_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

class SymbolTable {
public:
    struct Entry {
        std::string name;
        int type;
        int line;
        int col;
        int id;

        Entry(const std::string& n, int t, int l, int c, int i)
            : name(n), type(t), line(l), col(c), id(i) {}
    };

    SymbolTable();

    void addKeyword(const std::string& name);
    void addIdentifier(const std::string& name, int line, int col);
    void addInteger(const std::string& name, int line, int col);
    void addFloat(const std::string& name, int line, int col);

    bool has(const std::string& name) const;
    void writeTo(std::ostream& out) const;

private:
    std::vector<Entry> entries;
    std::map<std::string, int> index;
    int nextId;
};

#endif
