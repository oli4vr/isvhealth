#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <time.h>
#include "http_client.h"

#define MAX_NODES 32
#define MAX_DISKS 256
#define MAX_VOLUMES 256
#define MAX_FC_PORTS 64
#define MAX_HOSTS 64

// Node statistics
typedef struct {
    time_t timestamp;
    int node_id;
    char *node_name;
    double cpu_percent;
    double memory_percent;
    int read_io;
    int write_io;
    int read_latency;
    int write_latency;
    int queue_depth;
} node_stats_t;

// Disk statistics
typedef struct {
    time_t timestamp;
    int disk_id;
    char *disk_name;
    int read_io;
    int write_io;
    long long read_bytes;
    long long write_bytes;
    int read_latency;
    int write_latency;
    int queue_depth;
    int temp;
} disk_stats_t;

// Volume statistics
typedef struct {
    time_t timestamp;
    int volume_id;
    char *volume_name;
    int read_io;
    int write_io;
    long long read_bytes;
    long long write_bytes;
    int read_latency;
    int write_latency;
    int queue_depth;
} volume_stats_t;

// FC port statistics
typedef struct {
    time_t timestamp;
    int port_id;
    char *port_name;
    long long rx_bytes;
    long long tx_bytes;
    int rx_frames;
    int tx_frames;
    int rx_errors;
    int tx_errors;
    int link_failures;
} fc_port_stats_t;

// Host statistics
typedef struct {
    time_t timestamp;
    int host_id;
    char *host_name;
    int read_io;
    int write_io;
    long long read_bytes;
    long long write_bytes;
    int read_latency;
    int write_latency;
    int queue_depth;
} host_stats_t;

// All performance statistics
typedef struct {
    time_t timestamp;
    int node_count;
    node_stats_t nodes[MAX_NODES];
    int disk_count;
    disk_stats_t disks[MAX_DISKS];
    int volume_count;
    volume_stats_t volumes[MAX_VOLUMES];
    int fc_port_count;
    fc_port_stats_t fc_ports[MAX_FC_PORTS];
    int host_count;
    host_stats_t hosts[MAX_HOSTS];
} perf_stats_t;

// Helper functions for JSON parsing (cJSON already defined in cjson.h)
#include <cjson/cJSON.h>

// Helper functions for JSON parsing - defined in json_parser.c
int json_get_int(cJSON *obj, const char *key, int default_val);
const char *json_get_string(cJSON *obj, const char *key, const char *default_val);
double json_get_double(cJSON *obj, const char *key, double default_val);

// Performance statistics functions
int perf_get_node_stats(http_client_t *client, node_stats_t *stats, int max_count);
int perf_get_disk_stats(http_client_t *client, disk_stats_t *stats, int max_count);
int perf_get_volume_stats(http_client_t *client, volume_stats_t *stats, int max_count);
int perf_get_fc_port_stats(http_client_t *client, fc_port_stats_t *stats, int max_count);
int perf_get_host_stats(http_client_t *client, host_stats_t *stats, int max_count);
int perf_get_all(http_client_t *client, perf_stats_t *all_stats);

#endif // PERFORMANCE_H
