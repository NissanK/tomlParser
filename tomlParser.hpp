#include <iostream>
#include <toml.hpp> // Using the toml11 library

struct PathElement {
    enum class Type { Key, Index } type; // Type of path element: Key or Index
    std::string key; // Key for the path element
    int index; // Index for the path element

    // Constructor for key type path element
    PathElement(const std::string& k) : type(Type::Key), key(k), index(-1) {}

    // Constructor for index type path element
    PathElement(int i) : type(Type::Index), key(""), index(i) {}
};

class TomlParser {
private:
    toml::value tomlData; // Parsed TOML data

public:
    // Constructor that parses TOML file at given file path
    TomlParser(const std::string filePath) {
        tomlData = toml::parse(filePath);
    }

    // Converts a vector of PathElement objects to a string representation of the path
    std::string path_to_string(const std::vector<PathElement>& path) const {
        std::string result;
        for (const auto& elem : path) {
            if (elem.type == PathElement::Type::Key) {
                result += elem.key + ".";
            } else if (elem.type == PathElement::Type::Index) {
                result += "[" + std::to_string(elem.index) + "].";
            }
        }
        if (!result.empty()) {
            result.pop_back();
        }
        return result;
    }

    // Retrieves the TOML value at the specified path
    toml::value get_toml_value(const std::vector<PathElement>& path) {
        const toml::value* current = &tomlData;

        for (const auto& elem : path) {
            if (elem.type == PathElement::Type::Key) {
                if (!current->is_table() || current->as_table().find(elem.key) == current->as_table().end()) {
                    throw std::runtime_error("Path not found: " + path_to_string(path));
                }
                current = &current->at(elem.key);
            } else if (elem.type == PathElement::Type::Index) {
                if (!current->is_array() || elem.index < 0 || elem.index >= current->as_array().size()) {
                    throw std::runtime_error("Index out of bounds: " + path_to_string(path));
                }
                current = &current->as_array().at(elem.index);
            }
        }

        return *current;
    }

    // Retrieves all elements from a TOML table
    std::vector<std::pair<std::string, toml::value>> get_all_elements(const toml::value &val) {
        std::vector<std::pair<std::string, toml::value>> elements;

        if (val.is_table()) {
            const auto& table = val.as_table();
            for (auto it = table.begin(); it != table.end(); ++it) {
                elements.push_back(std::make_pair(it->first, it->second));
            }
        }
        return elements;
    }

    // Prints the elements of a TOML table, array or anything else with indentation, useful for debugging
    void print_elements(const toml::value &elem, int indent = 0) {
        std::string indentStr(indent, ' ');

        if (elem.is_table()) {
            std::cout << indentStr << "Table: " << std::endl;
            auto elements = get_all_elements(elem);
            for (const auto& pair : elements) {
                std::cout << indentStr << "    Key: " << pair.first << ", Value: ";
                print_elements(pair.second, indent + 4);
            }
        } else if (elem.is_array()) {
            std::cout << indentStr << "Array: " << std::endl;
            const auto& array = elem.as_array();
            for (const auto& item : array) {
                print_elements(item, indent + 4);
            }
        } else if (elem.is_string()) {
            std::cout << indentStr << "String: " << elem.as_string() << std::endl;
        } else if (elem.is_integer()) {
            std::cout << indentStr << "Integer: " << elem.as_integer() << std::endl;
        } else if (elem.is_floating()) {
            std::cout << indentStr << "Float: " << std::fixed << std::setprecision(20) << elem.as_floating() << std::endl;
        } else if (elem.is_boolean()) {
            std::cout << indentStr << "Boolean: " << elem.as_boolean() << std::endl;
        } else if (elem.is_local_date()) {
            std::cout << indentStr << "Date: " << elem.as_local_date() << std::endl;
        } else if (elem.is_local_time()) {
            std::cout << indentStr << "Time: " << elem.as_local_time() << std::endl;
        } else if (elem.is_local_datetime()) {
            std::cout << indentStr << "Datetime: " << elem.as_local_datetime() << std::endl;
        } else if (elem.is_offset_datetime()) {
            std::cout << indentStr << "Date: " << elem.is_offset_datetime() << std::endl;
        }
    }
};