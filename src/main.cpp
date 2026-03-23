
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <string>

int main() {
    //Créer la socket serveur
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1) {
        std::cerr << "Erreur lors de la création de la socket" << std::endl;
        return 1;
    }

    int opt = 1; 
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Erreur lors de la configuration de la socket" << std::endl;
        close(server_fd);
        return 1;
    }

    // Configurer l'adresse du serveur
    sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Lier la socket au port
    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Erreur lors du bind de la socket" << std::endl;
        close(server_fd);
        return 1;
    }

    // Écouter les connexions entrantes
    if(listen(server_fd, 5) < 0) {
        std::cerr << "Erreur lors de l'écoute des connexions" << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "Notre serveur est en écoute sur le port 8080..." << std::endl;
    std::cout << "Pour vous connecter: telnet localhost 8080 ou curl http://localhost:8080" << std::endl;
    
    while (true) {
        // Accepter une nouvelle connexion
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        
        if(client_fd < 0) {
            std::cerr << "Erreur lors de l'acceptation de la connexion" << std::endl;
            continue;
        }
        
        // Afficher l'adresse du client
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "Nouveau client connecté: " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
        
        // Lire la requête du client
        char buffer[1024] = {0};
        int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        
        if(bytes_read > 0) {
            std::cout << "Requête reçue:\n" << buffer << std::endl;
            
            // Lire le fichier index.html
            std::ifstream file("../src/index.html");
            
            // Vérifier si le fichier existe
            if(!file.is_open()) {
                std::cerr << "Erreur: impossible d'ouvrir index.html" << std::endl;
                const char* error_response = 
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/html\r\n"
                    "\r\n"
                    "<html><body><h1>Page non trouvée</h1></body></html>";
                send(client_fd, error_response, strlen(error_response), 0);
            } else {
                // Lire tout le contenu du fichier
                std::stringstream buffer_stream;
                buffer_stream << file.rdbuf();
                std::string html_content = buffer_stream.str();
                file.close();
                
                // Construire la réponse HTTP
                std::string response = 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html; charset=UTF-8\r\n"
                    "Content-Length: " + std::to_string(html_content.length()) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" +
                    html_content;
                
                send(client_fd, response.c_str(), response.length(), 0);
                std::cout << "Page HTML envoyée (" << html_content.length() << " octets)" << std::endl;
            }
        } else {
            std::cerr << "Erreur ou connexion fermée par le client" << std::endl;
            close(client_fd);
            continue;
        }
        
        // Fermer la connexion client
        close(client_fd);
        std::cout << "Connexion fermée" << std::endl;
    }

    close(server_fd);
    return 0;
}


