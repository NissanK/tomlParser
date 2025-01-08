#include "tomlParser.hpp"
#include <map>

int main() {
    try {
        TomlParser tomlParser = TomlParser("alphaConfig.toml"); // accessing the config file

        // global toml values below
        toml::value nameToml = tomlParser.get_toml_value({PathElement("name")}); //string
        toml::value ageToml = tomlParser.get_toml_value({PathElement("age")}); // int
        toml::value activeToml = tomlParser.get_toml_value({PathElement("active")}); // bool
        toml::value floatingToml = tomlParser.get_toml_value({PathElement("floating")}); // float
        toml::value languagesToml = tomlParser.get_toml_value({PathElement("languages")}); // array of strings
        toml::value offsetDatetimeToml = tomlParser.get_toml_value({PathElement("offset_datetime")});
        toml::value localDatetimeToml = tomlParser.get_toml_value({PathElement("local_datetime")});
        toml::value localDateToml = tomlParser.get_toml_value({PathElement("local_date")});
        toml::value localTimeToml = tomlParser.get_toml_value({PathElement("local_time")});
        toml::value ownerToml = tomlParser.get_toml_value({PathElement("owner")}); // Retreives the table owner
        
        // get_toml_value generates a generic toml::value object which has to be extracted for correct value
        std::string name = nameToml.as_string();
        int age = ageToml.as_integer();
        bool active = activeToml.as_boolean();
        float floating = floatingToml.as_floating();
        toml::offset_datetime offsetDatetime = offsetDatetimeToml.as_offset_datetime();
        toml::local_datetime localDatetime = localDatetimeToml.as_local_datetime();
        toml::local_date localDate = localDateToml.as_local_date();
        toml::local_time localTime = localTimeToml.as_local_time();
        std::vector<toml::value> languages = languagesToml.as_array(); // toml::value will have to be extracted afterwards for use
        
        std::map<std::string, toml::value> ownerTable; // extracting tables into map
        for (const auto& entry : ownerToml.as_table()) {
            ownerTable[entry.first] = entry.second;
        }

        tomlParser.print_elements(name); // all of these types get typecasted to toml::value
        tomlParser.print_elements(age);
        tomlParser.print_elements(active);
        tomlParser.print_elements(floating);
        tomlParser.print_elements(offsetDatetime);
        tomlParser.print_elements(localDatetime);
        tomlParser.print_elements(localDate);
        tomlParser.print_elements(localTime);
        tomlParser.print_elements(languages);
        tomlParser.print_elements(ownerTable);

        // Table operations
        std::string name_from_table = ownerToml.at("name").as_string();
        // accessing name in order table
        std::vector<toml::value> array_in_table = ownerToml.at("array_in_table").as_array(); 
        // accessing the array, notice it returns toml::value
        
        int ageToml_from_table = tomlParser.get_toml_value({PathElement("owner"), PathElement("age")}).as_integer(); 
        // multiple nested pathlement can be passed in get_toml_value.
        // If value exists, it will return the value otherwise appropriate runtime_error will be thrown.

        // Processing Config Files Examples
        std::vector<PathElement> path = {PathElement("alphaset")};

        toml::value alphasetTable = tomlParser.get_toml_value(path); // alphaset table
        std::vector<std::pair<std::string, toml::value>> alphasetVec = tomlParser.get_all_elements(alphasetTable);
        
        // gets all the elements of alphasetTable -> alphasetVec = [{alphaset_0: {}}, {alphaset_1, {}}, {alphaset_2, {}}]

        for(int i=0; i<alphasetVec.size(); i++){
            toml::value alphasetXTable = alphasetVec[i].second;
            std::vector<std::pair<std::string, toml::value>> alphasetXVec = tomlParser.get_all_elements(alphasetXTable);
            // getting all iterable values for table alphaset_x
            // we can also use .at("key") to get a particular value
            // alphasetXTable = {alpha1, alpha2, alpha3}

            for(int j=0; j<alphasetXVec.size(); j++){
                toml::value alphaXTable = alphasetXVec[j].second;
                std::vector<std::pair<std::string, toml::value>> alphaXVec = tomlParser.get_all_elements(alphaXTable);
                // alphaXTable = {name, param1, param2}

                for (const auto& keyValue : alphaXVec) {
                    std::cout << "Key: " << keyValue.first <<std::endl;
                    std::cout<< "Value: ";
                    tomlParser.print_elements({keyValue.second}, 12);
                }
            }
        }

        // getting the value delta from employees 2D table using get_toml_element
        std::string delta = tomlParser.get_toml_value({PathElement("employees"), PathElement(0),
         PathElement("name"), PathElement(0), PathElement(1)}).as_string();
        
        //getting the value role=manager from employees 2D table using get_toml_element
        std::string manager = tomlParser.get_toml_value({PathElement("employees"), PathElement(1), PathElement("role")}).as_string();

        //print the entire structure of employees with indentation using print_all_elements
        toml::value employees = tomlParser.get_toml_value({PathElement("employees")});
        tomlParser.print_elements(employees);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;

}