#ifndef HEALTH_H
#define HEALTH_H

#include <time.h>
#include "http_client.h"
#include "json_parser.h"

// Health status enums
typedef enum {
    HEALTH_OK = 0,
    HEALTH_WARNING,
    HEALTH_CRITICAL
} health_status_t_enum;

// Overall health status structure
typedef struct {
    time_t timestamp;
    health_status_t_enum overall;
    char message[256];
    int node_count;
    int disk_count;
    int volume_count;
    int host_count;
    int enclosure_count;
    int alert_count;
    // Storage pool info
    int pool_count;
    int fcm_count;
    // Performance stats (disabled for now - complex to implement)
    // int perf_stat_count;
} health_status_t;

// Health status collection functions
int health_get_nodes(http_client_t *client, node_info_t *nodes, int max_count);
int health_get_disks(http_client_t *client, disk_info_t *disks, int max_count);
int health_get_volumes(http_client_t *client, volume_info_t *volumes, int max_count);
int health_get_hosts(http_client_t *client, host_info_t *hosts, int max_count);
int health_get_enclosures(http_client_t *client, enclosure_info_t *enclosures, int max_count);
int health_get_alerts(http_client_t *client, alert_info_t *alerts, int max_count);
int health_get_pools(http_client_t *client, pool_info_t *pools, int max_count);
int health_get_fcms(http_client_t *client, fcm_info_t *fcms, int max_count);
int health_get_fc_ports(http_client_t *client, fc_port_info_t *ports, int max_count);
int health_get_perf_stats(http_client_t *client, perf_stat_t *stats, int max_count);
int health_get_status(http_client_t *client, health_status_t *status);

#endif // HEALTH_H
