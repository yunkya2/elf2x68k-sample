/*
 * HTTP Client Sample for X68000
 * HTTP通信のクライアントサンプル
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <ctype.h>

#define BUFFER_SIZE 4096
#define MAX_RESPONSE_SIZE 16384

#define DEFAULT_HOST    "www.retropc.net"
#define DEFAULT_PATH    "/x68000/"
#define DEFAULT_PORT    80

/*
 * Shift_JISの文字かどうかを判定
 */
int is_sjis_lead_byte(unsigned char c) {
    return ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC));
}

/*
 * ブロック要素のHTMLタグかどうかを判定
 */
int is_block_element(const char *tag_start) {
    const char *block_tags[] = {
        "<div", "<p", "<h1", "<h2", "<h3", "<h4", "<h5", "<h6",
        "<ul", "<ol", "<li", "<dl", "<dt", "<dd", 
        "<table", "<tr", "<td", "<th", "<thead", "<tbody", "<tfoot",
        "<article", "<section", "<header", "<footer", "<nav", "<aside",
        "<blockquote", "<pre", "<address", "<hr", "<br",
        "</div>", "</p>", "</h1>", "</h2>", "</h3>", "</h4>", "</h5>", "</h6>",
        "</ul>", "</ol>", "</li>", "</dl>", "</dt>", "</dd>",
        "</table>", "</tr>", "</td>", "</th>", "</thead>", "</tbody>", "</tfoot>",
        "</article>", "</section>", "</header>", "</footer>", "</nav>", "</aside>",
        "</blockquote>", "</pre>", "</address>", "</hr>", "</br>",
        NULL
    };
    
    int i;
    for (i = 0; block_tags[i] != NULL; i++) {
        int len = strlen(block_tags[i]);
        if (strncmp(tag_start, block_tags[i], len) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
 * 簡単なHTMLパース処理（Shift_JIS対応）
 * HTMLタグを除去してプレーンテキストを抽出
 */
void parse_html(char *html_content) {
    unsigned char *src = (unsigned char*)html_content;
    unsigned char *dst = (unsigned char*)html_content;
    int in_tag = 0;
    int in_script = 0;
    int in_style = 0;
    int consecutive_spaces = 0;
    unsigned char *tag_start = NULL;
    
    while (*src) {
        // Shift_JISの2バイト文字の処理
        if (is_sjis_lead_byte(*src) && *(src + 1)) {
            if (!in_tag && !in_script && !in_style) {
                *dst++ = *src;
                *dst++ = *(src + 1);
                consecutive_spaces = 0;
            }
            src += 2;
            continue;
        }
        
        // スクリプトタグの検出
        if (!in_tag && strncmp((char*)src, "<script", 7) == 0) {
            in_script = 1;
            in_tag = 1;
            tag_start = src;
        } else if (in_script && strncmp((char*)src, "</script>", 9) == 0) {
            in_script = 0;
            src += 9;
            continue;
        }
        
        // スタイルタグの検出
        if (!in_tag && strncmp((char*)src, "<style", 6) == 0) {
            in_style = 1;
            in_tag = 1;
            tag_start = src;
        } else if (in_style && strncmp((char*)src, "</style>", 8) == 0) {
            in_style = 0;
            src += 8;
            continue;
        }
        
        // タグの開始
        if (*src == '<' && !in_script && !in_style) {
            in_tag = 1;
            tag_start = src;
        }
        // タグの終了
        else if (*src == '>' && in_tag && !in_script && !in_style) {
            in_tag = 0;
            
            // ブロック要素の場合は改行を追加
            if (tag_start && is_block_element((char*)tag_start)) {
                // 直前が改行でない場合のみ改行を追加
                if (dst > (unsigned char*)html_content && *(dst - 1) != '\n') {
                    *dst++ = '\n';
                    consecutive_spaces = 0;
                }
            }
            
            src++;
            continue;
        }
        
        // タグ内でない場合は文字をコピー
        if (!in_tag && !in_script && !in_style) {
            // HTMLエンティティの簡単な変換
            if (strncmp((char*)src, "&lt;", 4) == 0) {
                *dst++ = '<';
                src += 4;
                consecutive_spaces = 0;
            } else if (strncmp((char*)src, "&gt;", 4) == 0) {
                *dst++ = '>';
                src += 4;
                consecutive_spaces = 0;
            } else if (strncmp((char*)src, "&amp;", 5) == 0) {
                *dst++ = '&';
                src += 5;
                consecutive_spaces = 0;
            } else if (strncmp((char*)src, "&quot;", 6) == 0) {
                *dst++ = '"';
                src += 6;
                consecutive_spaces = 0;
            } else if (strncmp((char*)src, "&nbsp;", 6) == 0) {
                *dst++ = ' ';
                src += 6;
                consecutive_spaces = 1;
            } else if (isspace(*src)) {
                // 連続する空白文字を1つのスペースに変換
                if (!consecutive_spaces) {
                    *dst++ = ' ';
                    consecutive_spaces = 1;
                }
                src++;
            } else {
                *dst++ = *src++;
                consecutive_spaces = 0;
            }
        } else {
            src++;
        }
    }
    
    *dst = '\0';
}

/*
 * HTTPレスポンスからヘッダーとボディを分離
 */
char* extract_html_body(char *response) {
    char *body = strstr(response, "\r\n\r\n");
    if (body) {
        return body + 4;
    }
    
    body = strstr(response, "\n\n");
    if (body) {
        return body + 2;
    }
    
    return response;
}

/*
 * HTTPステータスコードの解析
 */
int parse_http_status(char *response) {
    char *status_line = response;
    char *space = strchr(status_line, ' ');
    
    if (space) {
        return atoi(space + 1);
    }
    
    return 0;
}

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in server_addr;
    struct hostent *host_entry;
    char buffer[BUFFER_SIZE];
    char response[MAX_RESPONSE_SIZE];
    char request[BUFFER_SIZE];
    char *hostname = NULL;
    char *path = NULL;
    int port = 0;
    int bytes_sent, bytes_received;
    int total_received = 0;
    int status_code;
    char *html_body;
    int raw_mode = 0;  // HTMLをパースしないモード
    int i;
    
    printf("HTTP Client Sample for X68000\n");
    printf("==============================\n");
    
    // コマンドライン引数の解析
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--raw") == 0) {
            raw_mode = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [options] [hostname] [path] [port]\n", argv[0]);
            printf("Options:\n");
            printf("  -r, --raw     Display raw HTML without parsing\n");
            printf("  -h, --help    Show this help message\n");
            printf("Examples:\n");
            printf("  %s www.example.com\n", argv[0]);
            printf("  %s www.example.com /index.html\n", argv[0]);
            printf("  %s www.example.com /index.html 8080\n", argv[0]);
            printf("  %s -r www.example.com  (raw HTML mode)\n", argv[0]);
            return 0;
        } else if (argv[i][0] != '-') {
            // オプションでない引数の処理
            if (hostname == NULL) {
                hostname = argv[i];
            } else if (path == NULL) {
                path = argv[i];
            } else {
                port = atoi(argv[i]);
            }
        }
    }

    // デフォルト値のリセット（オプション処理後）
    hostname = hostname ? hostname : DEFAULT_HOST;
    path = path ? path : DEFAULT_PATH;
    port = (port > 0) ? port : DEFAULT_PORT;

    if (raw_mode) {
        printf("Raw mode enabled - HTML will not be parsed\n");
    }
    printf("Connecting to %s:%d%s\n", hostname, port, path);
    
    // ホスト名の解決
    host_entry = gethostbyname(hostname);
    if (host_entry == NULL) {
        printf("gethostbyname() failed for %s\n", hostname);
        return 1;
    }
    
    printf("Host resolved to: %s\n", inet_ntoa(*(struct in_addr*)host_entry->h_addr_list[0]));
    
    // ソケットの作成
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket() failed");
        return 1;
    }
    
    printf("Socket created successfully\n");
    
    // サーバーアドレスの設定
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr, host_entry->h_addr_list[0], host_entry->h_length);
    
    // サーバーに接続
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect() failed");
        close(client_fd);
        return 1;
    }
    
    printf("Connected to server successfully\n");
    
    // HTTP GETリクエストの作成
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: X68000-HTTP-Client/1.0\r\n"
             "Accept: */*\r\n"
             "Connection: close\r\n"
             "\r\n",
             path, hostname);
    
    printf("Sending HTTP request:\n");
    printf("--------------------\n");
    printf("%s", request);
    printf("--------------------\n\n");
    
    // HTTPリクエストを送信
    bytes_sent = send(client_fd, request, strlen(request), 0);
    if (bytes_sent < 0) {
        perror("send() failed");
        close(client_fd);
        return 1;
    }
    
    printf("Sent %d bytes\n", bytes_sent);
    printf("Receiving HTTP response...\n\n");
    
    // HTTPレスポンスを受信してバッファに蓄積
    memset(response, 0, sizeof(response));
    total_received = 0;
    
    while (total_received < sizeof(response) - 1) {
        bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                printf("Server closed connection\n");
            } else {
                perror("recv() failed");
            }
            break;
        }
        
        // バッファオーバーフローを防ぐ
        if (total_received + bytes_received >= sizeof(response)) {
            bytes_received = sizeof(response) - total_received - 1;
        }
        
        memcpy(response + total_received, buffer, bytes_received);
        total_received += bytes_received;
    }
    
    response[total_received] = '\0';
    
    // HTTPステータスコードの解析
    status_code = parse_http_status(response);
    printf("HTTP Status Code: %d\n", status_code);
    
    if (status_code == 200) {
        printf("Response received successfully!\n\n");
        
        // HTMLボディの抽出
        html_body = extract_html_body(response);
        
        if (raw_mode) {
            // RAWモード：HTMLをパースせずそのまま表示
            printf("Raw HTML Content (no parsing):\n");
            printf("==============================\n");
            printf("%s\n", html_body);
        } else {
            // 通常モード：HTMLをパースして表示
            printf("Raw HTML Content:\n");
            printf("=================\n");
            printf("%.500s", html_body);  // 最初の500文字のみ表示
            if (strlen(html_body) > 500) {
                printf("...\n[Content truncated]\n");
            }
            printf("\n\n");
            
            // HTMLパース処理
            parse_html(html_body);
            
            printf("Parsed Text Content:\n");
            printf("====================\n");
            printf("%s\n", html_body);
        }
        
    } else {
        printf("HTTP Error! Status: %d\n", status_code);
        printf("Raw Response:\n");
        printf("=============\n");
        printf("%s\n", response);
    }
    
    // ソケットを閉じる
    close(client_fd);
    printf("Connection closed\n");
    
    return 0;
}
