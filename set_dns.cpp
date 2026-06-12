#include <iostream>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

// Функция для выполнения консольной команды и получения её вывода
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("Ошибка при выполнении popen()");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    // Удаляем лишние символы переноса строки в конце
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

int main() {
    // Проверка на права суперпользователя (root)
    if (getuid() != 0) {
        std::cerr << "[-] Этот скрипт необходимо запускать с правами sudo!\n";
        return 1;
    }

    std::cout << "[*] Поиск активного сетевого интерфейса...\n";
    
    // Получаем имя активного соединения через nmcli
    // Команда выводит имя интерфейса, который сейчас подключен к интернету
    std::string cmd_get_conn = "nmcli -t -f NAME,DEVICE connection show --active | head -n 1 | cut -d: -f1";
    std::string active_conn = exec(cmd_get_conn.c_str());

    if (active_conn.empty()) {
        std::cerr << "[-] Активное сетевое соединение NetworkManager не найдено.\n";
        std::cerr << "[-] Убедитесь, что NetworkManager управляет вашим подключением.\n";
        return 1;
    }

    std::cout << "[+] Найдено активное соединение: \"" << active_conn << "\"\n";
    std::cout << "[*] Установка DNS от xbox-dns.ru...\n";

    // Официальные IP-адреса xbox-dns.ru:
    // Основной: 185.185.134.109, Альтернативный: 185.185.134.111
    std::string dns_servers = "185.185.134.109 185.185.134.111";

    // Формируем команды для NetworkManager
    std::string cmd_set_dns = "nmcli connection modify \"" + active_conn + "\" ipv4.dns \"" + dns_servers + "\"";
    std::string cmd_ignore_auto = "nmcli connection modify \"" + active_conn + "\" ipv4.ignore-auto-dns yes";
    std::string cmd_restart_conn = "nmcli connection up \"" + active_conn + "\"";

    // Выполняем настройку
    if (std::system(cmd_set_dns.c_str()) == 0 && std::system(cmd_ignore_auto.c_str()) == 0) {
        std::cout << "[+] DNS сервера успешно записаны в профиль.\n";
        std::cout << "[*] Перезапуск сетевого соединения для применения настроек...\n";
        
        if (std::system(cmd_restart_conn.c_str()) == 0) {
            std::cout << "[+] Сеть успешно перезапущена! Теперь используется xbox-dns.ru.\n";
        } else {
            std::cerr << "[-] Ошибка при перезапуске соединения. Попробуйте перезагрузить ПК.\n";
        }
    } else {
        std::cerr << "[-] Не удалось изменить настройки DNS.\n";
        return 1;
    }

    return 0;
}
