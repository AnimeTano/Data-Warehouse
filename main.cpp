#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "Manager/Manager.h"


std::string findBinPath() {
    std::string paths[] = {
        "bin/",
        "../bin/",
        "../../bin/",
        "./bin/",
        "C:/Users/AnimeTano/VSC/Projects/Data-Warehouse/bin/"
    };
    
    for (const auto& path : paths) {
        std::string testFile = path + "config.txt";
        std::ifstream file(testFile);
        if (file.is_open()) {
            file.close();
            std::cout << "Found bin/ at: " << path << "\n";
            return path;
        }
    }
    
    std::cerr << "ERROR: Cannot find bin/ directory!" << "\n";
    return "";
}

std::string getPassword(const std::string& binPath) {
    std::ifstream file(binPath + "config.txt");
    std::string password;
    
    if (file.fail()) {
        std::cerr << "Error: Cannot open " << binPath << "config.txt" << "\n";
        std::cout << "Enter PostgreSQL password manually: ";
        std::cin >> password;
        return password;
    }
    
    file >> password;
    file.close();
    
    return password;
}

int main() {
    std::cout << " ====Data Warehouse System==== " << "\n";
    
    std::string binPath = findBinPath();

    if (binPath.empty()) {
        return 1;
    }
    
    std::string password = getPassword(binPath);
    if (password.empty()) {
        return 1;
    }
    
    try {
        std::string conn_str = "dbname=datawarehouse user=postgres password=" + password + " host=localhost port=5432";
        
        Manager manager(conn_str, binPath);
        
        std::cout << "Loading data\n";
        manager.loadData();
        
        std::cout << " ---Analytics--- \n";
        manager.analytics();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    #ifdef _WIN32
        system("pause");
    #endif
    
    return 0;
}