using System;
using System.Net;
using System.Threading.Tasks;
using ARSoft.Tools.Net.Dns;

class Program
{
    static async Task Main(string[] args)
    {
        // IP-адрес или домен вашего DNS-сервера sbox-dns.ru
        // Для примера определим его IP (замените на реальный IP, если он известен)
        IPAddress dnsServerAddress = IPAddress.Parse("127.0.0.1"); // Укажите IP sbox-dns.ru
        
        // Имя, которое хотим разрешить
        string domainToResolve = "google.com";

        // Создаем DNS-клиент, указывая наш сервер
        DnsClient client = new DnsClient(dnsServerAddress, 5000); // 5000 мс таймаут

        try
        {
            Console.WriteLine($"Отправка запроса на {dnsServerAddress} для домена {domainToResolve}...");
            
            // Отправляем запрос типа A (IPv4)
            DnsMessage response = await client.ResolveAsync(DomainName.Parse(domainToResolve), RecordType.A);

            if (response == null || response.ReturnCode != ReturnCode.NoError)
            {
                Console.WriteLine($"Ошибка ответа от сервера: {response?.ReturnCode}");
            }
            else
            {
                foreach (var record in response.AnswerRecords)
                {
                    if (record is ARecord aRecord)
                    {
                        Console.WriteLine($"Успешно! IP адрес: {aRecord.Address}");
                    }
                }
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Произошла ошибка при подключении: {ex.Message}");
        }
    }
}