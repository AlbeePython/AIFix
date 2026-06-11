#include <iostream>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int SOCKET;
    const int INVALID_SOCKET = -1;
    const int SOCKET_ERROR = -1;
    #define closesocket close
#endif

const int DNS_PORT = 53;
const char* DNS_SERVER = "127.0.0.1"; // Замените на реальный IP-адрес sbox-dns.ru

int main() {
#ifdef _WIN32
    // Инициализация Winsock для Windows
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка инициализации Winsock" << std::endl;
        return 1;
    }
#endif

    // 1. Создание UDP сокета
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Не удалось создать сокет" << std::endl;
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    // 2. Настройка структуры адреса сервера sbox-dns.ru
    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DNS_PORT);
    
    // Преобразуем IP-адрес из строки
    if (inet_pton(AF_INET, DNS_SERVER, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Некорректный IP-адрес сервера" << std::endl;
        closesocket(clientSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    std::cout << "Сокет успешно создан. Готов к отправке DNS-пакетов на " << DNS_SERVER << ":" << DNS_PORT << std::endl;

    // Здесь формируется сырой DNS-пакет (DNS Header + Question) и отправляется через sendto
    // Пример заглушки отправки:
    /*
    char dnsBuffer[512]; 
    // ... заполнение dnsBuffer структурами DNS ...
    int sendResult = sendto(clientSocket, dnsBuffer, packetSize, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
    */

    // Закрытие сокета
    closesocket(clientSocket);
#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}