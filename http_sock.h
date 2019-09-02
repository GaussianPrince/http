int http_client_create(struct sockaddr_in*addr);
int http_server_create(struct sockaddr_in* addr);
struct addrinfo *http_dns_parse(char *url);