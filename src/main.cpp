#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    // 1) Créer la socket serveur
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

    // 2) Configurer l'adresse du serveur1
    sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // 3) Lier la socket au port
    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Erreur lors du bind de la socket" << std::endl;
        close(server_fd);
        return 1;
    }

    // 4 ) Écouter les connexions entrantes
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
            
            // Réponse HTTP simple
            const char* response = 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: 96\r\n"
                "\r\n"
                "<html><body><h1>Serveur C++</h1>"
                "<p>Bravo ! Votre serveur fonctionne !</p>"
                "</body></html>\r\n";
            
            send(client_fd, response, strlen(response), 0);
        }
        
        // Fermer la connexion client
        close(client_fd);
        std::cout << "Connexion fermée" << std::endl;
    }

    close(server_fd);
    return 0;
}