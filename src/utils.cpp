#include <fstream>
#include <sstream>
#include <string>

//Fonction de lecture du fichier 
std::string lireFichier(const std::string& chemin) {
    std::ifstream file(chemin);
    if (!file.is_open()) {
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}