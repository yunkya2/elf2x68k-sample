#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char **argv) {
    const char *hostname;
    struct hostent *host_info;
    struct in_addr **addr_list;

    if (argc > 2) {
        fprintf(stderr, "Usage: %s [hostname]\n", argv[0]);
        fprintf(stderr, "If no hostname is specified, ntp.nict.jp will be used.\n");
        return 1;
    }
    
    if (argc == 2) {
        hostname = argv[1];
    } else {
        hostname = "ntp.nict.jp";  // デフォルトのNTPサーバー
        printf("Using default NTP server: %s\n", hostname);
    }

    // Resolve hostname to IP
    host_info = gethostbyname(hostname);
    if (host_info == NULL) {
        fprintf(stderr, "Error: Unable to resolve hostname.\n");
        return 1;
    }

    printf("Hostname: %s\n", host_info->h_name);
    for (char **alias = host_info->h_aliases; *alias != NULL; alias++) {
        printf("Alias: %s\n", *alias);
    }
    printf("Address Type: %s\n", (host_info->h_addrtype == AF_INET) ? "IPv4" : "Unknown");
    printf("Address Length: %d\n", host_info->h_length);

    printf("IP Addresses:\n");
    addr_list = (struct in_addr **)host_info->h_addr_list;
    for (int i = 0; addr_list[i] != NULL; i++) {
        printf("  %s\n", inet_ntoa(*addr_list[i]));
    }
    if (addr_list[0] == NULL) {
        printf("No IP addresses found.\n");
        exit(1);
    }

    setenv("TZ", "JST-9", 1); // Set timezone to Japan Standard Time
    tzset(); // Apply the timezone setting

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(123); // SNTP port
    addr.sin_addr = *addr_list[0];

    for (int i = 0; i < 20; i++) {
        uint32_t buf[17];
        memset(buf, 0, sizeof(buf));
        buf[0] = htonl(0x1b000000); // SNTP request, version 3, mode 3 (client)

        sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *)&addr, addr_len);
        recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&addr, &addr_len);

        time_t t = ntohl(buf[10]) - 2208988800U; // Convert SNTP time to Unix time
        struct tm *timeinfo = localtime(&t);
        printf("Current time: %s", asctime(timeinfo));
        usleep(500 * 1000);
    }
    close(fd);

    return 0;
}
