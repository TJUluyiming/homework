#include "SymbolTable.h"

using namespace std;

SymbolTable::SymbolTable() : nextId(1) {}

void SymbolTable::addKeyword(const string& name) {
    if (has(name)) return;
    entries.emplace_back(name, 2, 0, 0, nextId++);
    index[name] = static_cast<int>(entries.size()) - 1;
}

void SymbolTable::addIdentifier(const string& name, int line, int col) {
    if (has(name)) return;
    entries.emplace_back(name, 1, line, col, nextId++);
    index[name] = static_cast<int>(entries.size()) - 1;
}

void SymbolTable::addInteger(const string& name, int line, int col) {
    if (has(name)) return;
    entries.emplace_back(name, 3, line, col, nextId++);
    index[name] = static_cast<int>(entries.size()) - 1;
}

void SymbolTable::addFloat(const string& name, int line, int col) {
    if (has(name)) return;
    entries.emplace_back(name, 4, line, col, nextId++);
    index[name] = static_cast<int>(entries.size()) - 1;
}

bool SymbolTable::has(const string& name) const {
    return index.find(name) != index.end();
}

void SymbolTable::writeTo(ostream& out) const {
    out << "ID\tName\tType\tLine\tCol" << endl;
    out << "----------------------------------------" << endl;

    for (const Entry& entry : entries) {
        string typeName;
        switch (entry.type) {
            case 1: typeName = "IDN"; break;
            case 2: typeName = "KW"; break;
            case 3: typeName = "INT"; break;
            case 4: typeName = "FLOAT"; break;
            default: typeName = "UNKNOWN";
        }
        out << entry.id << "\t" << entry.name << "\t" << typeName << "\t" << entry.line << "\t"
            << entry.col << endl;
    }
}
