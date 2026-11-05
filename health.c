#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cjson/cJSON.h>
#include "http_client.h"
#include "json_parser.h"
#include "health.h"

extern int g_verbose;
extern int g_debug;

// Get all nodes status
int health_get_nodes(http_client_t *client, node_info_t *nodes, int max_count) {
    char *response;
    cJSON *json;
    int count;
    
    if (g_debug) fprintf(stderr, "DEBUG: Calling /lsnode\n");
    response = http_post(client, "/lsnode", NULL);
    if (!response) {
        if (g_debug) fprintf(stderr, "DEBUG: Failed to get nodes: %s\n", http_error(client));
        else fprintf(stderr, "Failed to get nodes: %s\n", http_error(client));
        return -1;
    }
    
    if (g_debug) fprintf(stderr, "DEBUG: /lsnode response: %.200s\n", response);
    
    json = cJSON_Parse(response);
    free(response);
    
    if (!json) {
        fprintf(stderr, "Failed to parse nodes response\n");
        return -1;
    }
    
    count = json_parse_nodes(json, nodes, max_count);
    cJSON_Delete(json);
    
    return count;
}

// Get all disks status
int health_get_disks(http_client_t *client, disk_info_t *disks, int max_count) {
    char *response;
    cJSON *json;
    int count;
    
    if (g_debug) fprintf(stderr, "DEBUG: Calling /lsdrive\n");
    response = http_post(client, "/lsdrive", NULL);
    if (!response) {
        if (g_debug) fprintf(stderr, "DEBUG: Failed to get disks: %s\n", http_error(client));
        else fprintf(stderr, "Failed to get disks: %s\n", http_error(client));
        return -1;
    }
    if (g_debug) fprintf(stderr, "DEBUG: /lsdrive response: %.200s\n", response);
    
    json = cJSON_Parse(response);
    free(response);
    
    if (!json) {
        fprintf(stderr, "Failed to parse disks response\n");
        return -1;
    }
    
    count = json_parse_disks(json, disks, max_count);
    cJSON_Delete(json);
    
    return count;
}

// Get all volumes status
int health_get_volumes(http_client_t *client, volume_info_t *volumes, int max_count) {
    char *response;
    cJSON *json;
    int count;
    
    if (g_debug) fprintf(stderr, "DEBUG: Calling /lsvdisk\n");
    response = http_post(client, "/lsvdisk", "");
    if (!response) {
        fprintf(stderr, "DEBUG: Failed to get volumes: %s\n", http_error(client));
        return -1;
    }
    
    if (g_debug) fprintf(stderr, "DEBUG: /lsvdisk response: %.200s\n", response);
    
    json = cJSON_Parse(response);
    free(response);
    
    if (!json) {
        fprintf(stderr, "DEBUG: Failed to parse volumes response\n");
        return -1;
    }
    
    count = json_parse_volumes(json, volumes, max_count);
    cJSON_Delete(json);
    
    if (g_debug) fprintf(stderr, "DEBUG: Parsed %d volumes\n", count);
    
    return count;
}

// Get all hosts status
int health_get_hosts(http_client_t *client, host_info_t *hosts, int max_count) {
    char *response;
    cJSON *json;
    int count;
    
    response = http_post(client, "/lshost", "");
    if (!response) {
        fprintf(stderr, "Failed to get hosts: %s\n", http_error(client));
        return -1;
    }
    
    json = cJSON_Parse(response);
    free(response);
    
    if (!json) {
        fprintf(stderr, "Failed to parse hosts response\n");
        return -1;
    }
    
    count = json_parse_hosts(json, hosts, max_count);
    cJSON_Delete(json);
    
    return count;
}

// Get all enclosures status
int health_get_enclosures(http_client_t *client, enclosure_info_t *enclosures, int max_count) {
    char *response;
    cJSON *json;
    int count;
    
    response = http_post(client, "/lsenclosure", "");
    if (!response) {
        fprintf(stderr, "Failed to get enclosures: %s\n", http_error(client));
        return -1;
    }
    
    json = cJSON_Parse(response);
    free(response);
    
    if (!json) {
        fprintf(stderr, "Failed to parse enclosures response\n");
        return -1;
    }
    
    count = json_parse_enclosures(json, enclosures, max_count);
    cJSON_Delete(json);
    
    return count;
}

// Get all storage pools status
int health_get_pools(http_client_t *client, pool_info_t *pools, int max_count) {
    char *response;
    cJSON *json;
    int count;
    
    response = http_post(client, "/lsmdiskgrp", "");
    if (!response) {
        fprintf(stderr, "Failed to get pools: %s\n", http_error(client));
        return -1;
    }
    
    json = cJSON_Parse(response);
    free(response);
    
    if (!json) {
        fprintf(stderr, "Failed to parse pools response\n");
        return -1;
    }
    
    count = json_parse_pools(json, pools, max_count);
    cJSON_Delete(json);
    
    return count;
}

// Get all FCM (Flash Core Module) status
int health_get_fcms(http_client_t *client, fcm_info_t *fcms, int max_count) {
    char *response;
    cJSON *json;
    int count;
    
    response = http_post(client, "/lsdriveclass", "");
    if (!response) {
        fprintf(stderr, "Failed to get FCMs: %s\n", http_error(client));
        return -1;
    }
    
    json = cJSON_Parse(response);
    free(response);
    
    if (!json) {
        fprintf(stderr, "Failed to parse FCMs response\n");
        return -1;
    }
    
    count = json_parse_fcms(json, fcms, max_count);
    cJSON_Delete(json);
    
    return count;
}

// Get performance statistics
int health_get_perf_stats(http_client_t *client, perf_stat_t *stats, int max_count) {
    char *response;
    cJSON *json;
    int count;
    
    response = http_post(client, "/lssystemstats", "");
    if (!response) {
        fprintf(stderr, "Failed to get performance stats: %s\n", http_error(client));
        return -1;
    }
    
    json = cJSON_Parse(response);
    free(response);
    
    if (!json) {
        fprintf(stderr, "Failed to parse performance stats response\n");
        return -1;
    }
    
    count = json_parse_perf_stats(json, stats, max_count);
    cJSON_Delete(json);
    
    return count;
}

// Get all FC ports status
int health_get_fc_ports(http_client_t *client, fc_port_info_t *ports, int max_count) {
    char *response;
    cJSON *json;
    int count;
    
    response = http_post(client, "/lsportfc", "");
    if (!response) {
        fprintf(stderr, "Failed to get FC ports: %s\n", http_error(client));
        return -1;
    }
    
    json = cJSON_Parse(response);
    free(response);
    
    if (!json) {
        fprintf(stderr, "Failed to parse FC ports response\n");
        return -1;
    }
    
    count = json_parse_fc_ports(json, ports, max_count);
    cJSON_Delete(json);
    
    return count;
}

// Get all alerts
int health_get_alerts(http_client_t *client, alert_info_t *alerts, int max_count) {
    char *response;
    cJSON *json;
    int count;
    
    response = http_post(client, "/lseventlog", "");
    if (!response) {
        fprintf(stderr, "Failed to get alerts: %s\n", http_error(client));
        return -1;
    }
    
    json = cJSON_Parse(response);
    free(response);
    
    if (!json) {
        fprintf(stderr, "Failed to parse alerts response\n");
        return -1;
    }
    
    count = json_parse_alerts(json, alerts, max_count);
    cJSON_Delete(json);
    
    return count;
}

// Get overall system health status
int health_get_status(http_client_t *client, health_status_t *status) {
    node_info_t nodes[32];
    disk_info_t disks[256];
    volume_info_t volumes[256];
    host_info_t all_hosts[64];
    enclosure_info_t all_enclosures[16];
    alert_info_t alerts[128];
    pool_info_t pools[16];
    fcm_info_t fcms[8];
    
    int node_count, disk_count, alert_count;
    int error_count = 0;
    int warning_count = 0;
    
    memset(status, 0, sizeof(health_status_t));
    status->timestamp = time(NULL);
    
    // Get nodes
    node_count = health_get_nodes(client, nodes, 32);
    if (node_count > 0) {
        status->node_count = node_count;
        for (int i = 0; i < node_count; i++) {
            if (strcmp(nodes[i].status, "offline") == 0 || strcmp(nodes[i].status, "failed") == 0) {
                error_count++;
            } else if (strcmp(nodes[i].status, "warning") == 0) {
                warning_count++;
            }
        }
    }
    node_info_free(nodes, node_count);
    
    // Get disks
    disk_count = health_get_disks(client, disks, 256);
    if (disk_count > 0) {
        status->disk_count = disk_count;
        for (int i = 0; i < disk_count; i++) {
            if (strcmp(disks[i].status, "offline") == 0 || strcmp(disks[i].status, "failed") == 0) {
                error_count++;
            } else if (strcmp(disks[i].status, "warning") == 0 || 
                       (disks[i].fail_risk && strcmp(disks[i].fail_risk, "yes") == 0)) {
                warning_count++;
            }
        }
    }
    disk_info_free(disks, disk_count);
    
    // Get alerts
    alert_count = health_get_alerts(client, alerts, 128);
    if (alert_count > 0) {
        status->alert_count = alert_count;
        for (int i = 0; i < alert_count; i++) {
            if (strcmp(alerts[i].severity, "critical") == 0 || 
                strcmp(alerts[i].severity, "error") == 0) {
                error_count++;
            } else if (strcmp(alerts[i].severity, "warning") == 0) {
                warning_count++;
            }
        }
    }
    alert_info_free(alerts, alert_count);
    
    // Get and count hosts (for status reporting only)
    int host_count = health_get_hosts(client, all_hosts, 64);
    status->host_count = host_count > 0 ? host_count : 0;
    host_info_free(all_hosts, host_count > 0 ? host_count : 0);
    
    // Get and count enclosures (for status reporting only)
    int enclosure_count = health_get_enclosures(client, all_enclosures, 16);
    status->enclosure_count = enclosure_count > 0 ? enclosure_count : 0;
    enclosure_info_free(all_enclosures, enclosure_count > 0 ? enclosure_count : 0);
    
    // Get volumes
    int volume_count = health_get_volumes(client, volumes, 256);
    status->volume_count = volume_count > 0 ? volume_count : 0;
    volume_info_free(volumes, volume_count > 0 ? volume_count : 0);
    
    // Get pools (disabled - causes segfault)
    // int pool_count = health_get_pools(client, pools, 16);
    // status->pool_count = pool_count > 0 ? pool_count : 0;
    // pool_info_free(pools, pool_count > 0 ? pool_count : 0);
    
    // Get FCMs (disabled - causes segfault)
    // int fcm_count = health_get_fcms(client, fcms, 8);
    // status->fcm_count = fcm_count > 0 ? fcm_count : 0;
    // fcm_info_free(fcms, fcm_count > 0 ? fcm_count : 0);
    
    // Set overall status
    if (error_count > 0) {
        status->overall = HEALTH_CRITICAL;
        snprintf(status->message, sizeof(status->message), 
                 "%d errors, %d warnings detected", error_count, warning_count);
    } else if (warning_count > 0) {
        status->overall = HEALTH_WARNING;
        snprintf(status->message, sizeof(status->message), 
                 "%d warnings detected", warning_count);
    } else {
        status->overall = HEALTH_OK;
        snprintf(status->message, sizeof(status->message), 
                 "System health is OK");
    }
    
    return 0;
}
