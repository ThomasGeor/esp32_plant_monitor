#define SNTP_TIME_SERVER "pool.ntp.org"
#define SNTP_MAX_RETRY_COUNT 15

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif

void sntp(void);
char *get_timestamp(void);
