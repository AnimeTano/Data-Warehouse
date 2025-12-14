#include <iostream>
#include <fstream>
#include <string>
#include "Manager/Manager.h"


std::string getPassword(){
    std::ifstream file("../bin/config.txt");
    std::string line;

    if (file.fail()){
        std::cout << "Error with read password from file" << "\n";
        return line;
    }

    std::getline(file, line);

    file.close();
    return line;
}


int main() {
    std::string password = getPassword();

    try {
        std::string conn_str = "dbname=datawarehouse user=postgres password= " + password + " host=localhost port=5432";
        Manager manager(conn_str);

        std::cout << "Loading data\n";
        manager.loadData();

        std::cout << "Analytics queries\n";
        manager.analytics();


    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}