#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cjson/cJSON.h>
#include "health.h"
#include "performance.h"
#include "output.h"

// Helper to get alerts data
static alert_info_t* get_alerts_data(http_client_t *client) {
    static alert_info_t alerts[128];
    static int count = 0;
    
    // Only collect once
    if (count == 0 && client) {
        count = health_get_alerts(client, alerts, 128);
    }
    return alerts;
}

// Output health status in plain text format
void output_health_txt(health_status_t *status, node_info_t *nodes, int node_count,
                       disk_info_t *disks, int disk_count, pool_info_t *pools, int pool_count,
                       port_info_t *ports, int port_count, volume_info_t *volumes, int volume_count,
                       host_info_t *hosts, int host_count, fc_port_info_t *fc_ports, int fc_port_count) {
    printf("# IBM Flashsystem Health Report\n");
    printf("# Generated: %s", ctime(&status->timestamp));
    printf("# System Status: %s\n", 
           status->overall == HEALTH_OK ? "OK" : 
           status->overall == HEALTH_WARNING ? "WARNING" : "CRITICAL");
    printf("# %s\n\n", status->message);
    
    // Node status
    printf("=== NODES ===\n");
    for (int i = 0; i < node_count; i++) {
        printf("NODE %s id=%s status=%s role=%s type=%s\n",
               nodes[i].name, nodes[i].id, nodes[i].status, nodes[i].role,
               nodes[i].node_type);
    }
    printf("\n");
    
    // Disk status
    printf("=== DISKS ===\n");
    for (int i = 0; i < disk_count; i++) {
        printf("DISK %s id=%s status=%s type=%s capacity=%.0fGB fail_risk=%s\n",
               disks[i].name, disks[i].id, disks[i].status, disks[i].type,
               disks[i].capacity / (1024.0 * 1024.0 * 1024.0),
               disks[i].fail_risk);
    }
    printf("\n");
    
    // Host status
    printf("=== HOSTS ===\n");
    for (int i = 0; i < host_count; i++) {
        printf("HOST %s id=%s status=%s type=%s\n",
               hosts[i].name, hosts[i].id, hosts[i].status, hosts[i].host_type);
    }
    printf("\n");
    
    // FC Port status
    printf("=== FC PORTS ===\n");
    for (int i = 0; i < fc_port_count; i++) {
        printf("FC_PORT %s_%s id=%s node=%s status=%s speed=%s\n",
               fc_ports[i].node_name, fc_ports[i].port_id,
               fc_ports[i].port_id, fc_ports[i].node_name,
               fc_ports[i].status, fc_ports[i].port_speed);
    }
    printf("\n");
    
    // Storage pool status
    printf("=== POOLS ===\n");
    for (int i = 0; i < pool_count; i++) {
        double used_pct = pools[i].capacity_total > 0 ? 
            (pools[i].capacity_used / pools[i].capacity_total) * 100.0 : 0.0;
        printf("POOL %s id=%s status=%s capacity_used=%.0fGB capacity_total=%.0fGB usage=%.0f%%\n",
               pools[i].name, pools[i].id, pools[i].status,
               pools[i].capacity_used / (1024.0 * 1024.0 * 1024.0),
               pools[i].capacity_total / (1024.0 * 1024.0 * 1024.0),
               used_pct);
    }
    printf("\n");
    
    // Volume status
    printf("=== VOLUMES ===\n");
    for (int i = 0; i < volume_count; i++) {
        printf("VOLUME %s id=%s status=%s type=%s capacity=%.0fGB used=%.0fGB\n",
               volumes[i].name, volumes[i].id, volumes[i].status,
               volumes[i].vtype,
               volumes[i].capacity / (1024.0 * 1024.0 * 1024.0),
               volumes[i].used / (1024.0 * 1024.0 * 1024.0));
    }
    printf("\n");
    
    // Host status (endpoint not available)
    printf("=== HOSTS ===\n");
    printf("No hosts collected (endpoint not available)\n");
    printf("\n");
    
    // Enclosure status (endpoint not available)
    printf("=== ENCLOSURES ===\n");
    printf("No enclosures collected (endpoint not available)\n");
    printf("\n");
    
    // Alert status (alerts are collected separately in main.c)
    printf("=== ALERTS ===\n");
    if (status->alert_count > 0) {
        printf("Alert count: %d (details available via JSON or restit output)\n", status->alert_count);
    } else {
        printf("No alerts\n");
    }
    printf("\n");
}

// Output health status in JSON format
void output_health_json(health_status_t *status, node_info_t *nodes, int node_count,
                        disk_info_t *disks, int disk_count, pool_info_t *pools, int pool_count,
                        port_info_t *ports, int port_count, volume_info_t *volumes, int volume_count,
                        host_info_t *hosts, int host_count, fc_port_info_t *fc_ports, int fc_port_count) {
    printf("{\n");
    printf("  \"timestamp\": %ld,\n", status->timestamp);
    printf("  \"system_status\": \"%s\",\n",
           status->overall == HEALTH_OK ? "ok" :
           status->overall == HEALTH_WARNING ? "warning" : "critical");
    printf("  \"message\": \"%s\",\n", status->message);
    printf("  \"summary\": {\n");
    printf("    \"node_count\": %d,\n", status->node_count);
    printf("    \"disk_count\": %d,\n", status->disk_count);
    printf("    \"volume_count\": %d,\n", status->volume_count);
    printf("    \"host_count\": %d,\n", status->host_count);
    printf("    \"enclosure_count\": %d,\n", status->enclosure_count);
    printf("    \"alert_count\": %d\n", status->alert_count);
    printf("  },\n");
    
    // Nodes
    printf("  \"nodes\": [\n");
    for (int i = 0; i < node_count; i++) {
        printf("    {\n");
        printf("      \"id\": \"%s\",\n", nodes[i].id);
        printf("      \"name\": \"%s\",\n", nodes[i].name);
        printf("      \"status\": \"%s\",\n", nodes[i].status);
        printf("      \"role\": \"%s\",\n", nodes[i].role);
        printf("      \"location\": \"%s\",\n", nodes[i].location);
        printf("      \"type\": \"%s\"\n", nodes[i].node_type);
        printf("    }%s\n", i < node_count - 1 ? "," : "");
    }
    printf("  ],\n");
    
    // Disks
    printf("  \"disks\": [\n");
    for (int i = 0; i < disk_count; i++) {
        printf("    {\n");
        printf("      \"id\": \"%s\",\n", disks[i].id);
        printf("      \"name\": \"%s\",\n", disks[i].name);
        printf("      \"status\": \"%s\",\n", disks[i].status);
        printf("      \"type\": \"%s\",\n", disks[i].type);
        printf("      \"capacity\": %.0f,\n", disks[i].capacity / (1024.0 * 1024.0 * 1024.0));
        printf("      \"fail_risk\": \"%s\"\n", disks[i].fail_risk);
        printf("    }%s\n", i < disk_count - 1 ? "," : "");
    }
    printf("  ],\n");
    
    // Pools
    printf("  \"pools\": [\n");
    for (int i = 0; i < pool_count; i++) {
        double used_pct = pools[i].capacity_total > 0 ? 
            (pools[i].capacity_used / pools[i].capacity_total) * 100.0 : 0.0;
        printf("    {\n");
        printf("      \"id\": \"%s\",\n", pools[i].id);
        printf("      \"name\": \"%s\",\n", pools[i].name);
        printf("      \"status\": \"%s\",\n", pools[i].status);
        printf("      \"capacity_used\": %.0f,\n", pools[i].capacity_used / (1024.0 * 1024.0 * 1024.0));
        printf("      \"capacity_total\": %.0f,\n", pools[i].capacity_total / (1024.0 * 1024.0 * 1024.0));
        printf("      \"usage_pct\": %.1f\n", used_pct);
        printf("    }%s\n", i < pool_count - 1 ? "," : "");
    }
    printf("  ],\n");
    
    // Volumes
    printf("  \"volumes\": [\n");
    for (int i = 0; i < volume_count; i++) {
        printf("    {\n");
        printf("      \"id\": \"%s\",\n", volumes[i].id);
        printf("      \"name\": \"%s\",\n", volumes[i].name);
        printf("      \"status\": \"%s\",\n", volumes[i].status);
        printf("      \"type\": \"%s\",\n", volumes[i].vtype);
        printf("      \"capacity\": %.0f,\n", volumes[i].capacity / (1024.0 * 1024.0 * 1024.0));
        printf("      \"used\": %.0f\n", volumes[i].used / (1024.0 * 1024.0 * 1024.0));
        printf("    }%s\n", i < volume_count - 1 ? "," : "");
    }
    printf("  ],\n");
    
    // Hosts
    printf("  \"hosts\": [\n");
    for (int i = 0; i < host_count; i++) {
        printf("    {\n");
        printf("      \"id\": \"%s\",\n", hosts[i].id);
        printf("      \"name\": \"%s\",\n", hosts[i].name);
        printf("      \"status\": \"%s\",\n", hosts[i].status);
        printf("      \"type\": \"%s\"\n", hosts[i].host_type);
        printf("    }%s\n", i < host_count - 1 ? "," : "");
    }
    printf("  ],\n");
    
    // FC Ports
    printf("  \"fc_ports\": [\n");
    for (int i = 0; i < fc_port_count; i++) {
        printf("    {\n");
        printf("      \"id\": \"%s\",\n", fc_ports[i].port_id);
        printf("      \"node\": \"%s\",\n", fc_ports[i].node_name);
        printf("      \"status\": \"%s\",\n", fc_ports[i].status);
        printf("      \"speed\": \"%s\",\n", fc_ports[i].port_speed);
        printf("      \"wwpn\": \"%s\"\n", fc_ports[i].wwpn);
        printf("    }%s\n", i < fc_port_count - 1 ? "," : "");
    }
    printf("  ],\n");
    
    // Enclosures (endpoint not available)
    printf("  \"enclosures\": [],\n");
    
    // Alerts
    printf("  \"alert_count\": %d,\n", status->alert_count);
    printf("  \"alerts\": []\n");
    printf("}\n");
}

// Output performance data in plain text format
void output_perf_txt(perf_stats_t *stats) {
    printf("# IBM Flashsystem Performance Statistics\n");
    printf("# Generated: %s", ctime(&stats->timestamp));
    printf("\n");
    
    // Node statistics
    printf("=== NODE STATISTICS ===\n");
    for (int i = 0; i < stats->node_count; i++) {
        printf("NODE %s cpu=%.0f%% memory=%.0f%% read_io=%d write_io=%d\n",
               stats->nodes[i].node_name,
               stats->nodes[i].cpu_percent, stats->nodes[i].memory_percent,
               stats->nodes[i].read_io, stats->nodes[i].write_io);
    }
    printf("\n");
    
    // Disk statistics
    printf("=== DISK STATISTICS ===\n");
    for (int i = 0; i < stats->disk_count; i++) {
        printf("DISK %s read_io=%d write_io=%d read_bytes=%lld write_bytes=%lld temp=%dC\n",
               stats->disks[i].disk_name,
               stats->disks[i].read_io, stats->disks[i].write_io,
               stats->disks[i].read_bytes, stats->disks[i].write_bytes,
               stats->disks[i].temp);
    }
    printf("\n");
    
    // Volume statistics
    printf("=== VOLUME STATISTICS ===\n");
    for (int i = 0; i < stats->volume_count; i++) {
        printf("VOLUME %s read_io=%d write_io=%d read_bytes=%lld write_bytes=%lld\n",
               stats->volumes[i].volume_name,
               stats->volumes[i].read_io, stats->volumes[i].write_io,
               stats->volumes[i].read_bytes, stats->volumes[i].write_bytes);
    }
    printf("\n");
    
    // FC port statistics
    printf("=== FC PORT STATISTICS ===\n");
    for (int i = 0; i < stats->fc_port_count; i++) {
        printf("PORT %s rx_bytes=%lld tx_bytes=%lld rx_errors=%d tx_errors=%d\n",
               stats->fc_ports[i].port_name,
               stats->fc_ports[i].rx_bytes, stats->fc_ports[i].tx_bytes,
               stats->fc_ports[i].rx_errors, stats->fc_ports[i].tx_errors);
    }
    printf("\n");
    
    // Host statistics
    printf("=== HOST STATISTICS ===\n");
    for (int i = 0; i < stats->host_count; i++) {
        printf("HOST %s read_io=%d write_io=%d read_bytes=%lld write_bytes=%lld\n",
               stats->hosts[i].host_name,
               stats->hosts[i].read_io, stats->hosts[i].write_io,
               stats->hosts[i].read_bytes, stats->hosts[i].write_bytes);
    }
    printf("\n");
}

// Output performance data in JSON format
void output_perf_json(perf_stats_t *stats) {
    printf("{\n");
    printf("  \"timestamp\": %ld,\n", stats->timestamp);
    
    // Node statistics
    printf("  \"nodes\": [\n");
    for (int i = 0; i < stats->node_count; i++) {
        printf("    {\n");
        printf("      \"id\": %d,\n", stats->nodes[i].node_id);
        printf("      \"name\": \"%s\",\n", stats->nodes[i].node_name);
        printf("      \"cpu_percent\": %.1f,\n", stats->nodes[i].cpu_percent);
        printf("      \"memory_percent\": %.1f,\n", stats->nodes[i].memory_percent);
        printf("      \"read_io\": %d,\n", stats->nodes[i].read_io);
        printf("      \"write_io\": %d\n", stats->nodes[i].write_io);
        printf("    }%s\n", i < stats->node_count - 1 ? "," : "");
    }
    printf("  ],\n");
    
    // Disk statistics
    printf("  \"disks\": [\n");
    for (int i = 0; i < stats->disk_count; i++) {
        printf("    {\n");
        printf("      \"id\": %d,\n", stats->disks[i].disk_id);
        printf("      \"name\": \"%s\",\n", stats->disks[i].disk_name);
        printf("      \"read_io\": %d,\n", stats->disks[i].read_io);
        printf("      \"write_io\": %d,\n", stats->disks[i].write_io);
        printf("      \"read_bytes\": %lld,\n", stats->disks[i].read_bytes);
        printf("      \"write_bytes\": %lld\n", stats->disks[i].write_bytes);
        printf("    }%s\n", i < stats->disk_count - 1 ? "," : "");
    }
    printf("  ],\n");
    
    // FC port statistics
    printf("  \"fc_ports\": [\n");
    for (int i = 0; i < stats->fc_port_count; i++) {
        printf("    {\n");
        printf("      \"id\": %d,\n", stats->fc_ports[i].port_id);
        printf("      \"name\": \"%s\",\n", stats->fc_ports[i].port_name);
        printf("      \"rx_bytes\": %lld,\n", stats->fc_ports[i].rx_bytes);
        printf("      \"tx_bytes\": %lld,\n", stats->fc_ports[i].tx_bytes);
        printf("      \"rx_errors\": %d,\n", stats->fc_ports[i].rx_errors);
        printf("      \"tx_errors\": %d\n", stats->fc_ports[i].tx_errors);
        printf("    }%s\n", i < stats->fc_port_count - 1 ? "," : "");
    }
    printf("  ]\n");
    printf("}\n");
}

// Output health and performance together
void output_combined_txt(health_status_t *health, perf_stats_t *perf,
                         node_info_t *nodes, int node_count,
                         disk_info_t *disks, int disk_count,
                         pool_info_t *pools, int pool_count,
                         port_info_t *ports, int port_count) {
    printf("IBM Flashsystem Health & Performance Report\n");
    printf("=============================================\n");
    printf("Generated: %s", ctime(&health->timestamp));
    printf("System Status: %s - %s\n\n",
           health->overall == HEALTH_OK ? "OK" : 
           health->overall == HEALTH_WARNING ? "WARNING" : "CRITICAL",
           health->message);
    
    // Summary
    printf("Summary:\n");
    printf("  Nodes: %d\n", node_count);
    printf("  Disks: %d\n", disk_count);
    printf("  Pools: %d\n", pool_count);
    printf("\n");
    
    // Health
    printf("--- Health Status ---\n");
    for (int i = 0; i < node_count; i++) {
        printf("NODE %s: %s\n", nodes[i].name, nodes[i].status);
    }
    for (int i = 0; i < disk_count; i++) {
        printf("DISK %s: %s\n", disks[i].name, disks[i].status);
    }
    printf("\n");
    
    // Performance
    printf("--- Performance ---\n");
    if (perf && perf->node_count > 0) {
        printf("Avg Node CPU: %.0f%%, Avg Node Mem: %.0f%%\n",
               perf->node_count > 0 ? 
               (double)perf->nodes[0].cpu_percent : 0,
               perf->node_count > 0 ?
               (double)perf->nodes[0].memory_percent : 0);
    }
    if (perf && perf->disk_count > 0) {
        long long total_read = 0, total_write = 0;
        for (int i = 0; i < perf->disk_count; i++) {
            total_read += perf->disks[i].read_bytes;
            total_write += perf->disks[i].write_bytes;
        }
        printf("Disk Throughput: %.0f MB/s read, %.0f MB/s write\n",
               total_read / 1024.0 / 1024.0, total_write / 1024.0 / 1024.0);
    }
}

void output_combined_json(health_status_t *health, perf_stats_t *perf,
                          node_info_t *nodes, int node_count,
                          disk_info_t *disks, int disk_count) {
    printf("{\n");
    printf("  \"health\": {\n");
    printf("    \"timestamp\": %ld,\n", health->timestamp);
    printf("    \"status\": \"%s\",\n",
           health->overall == HEALTH_OK ? "ok" :
           health->overall == HEALTH_WARNING ? "warning" : "critical");
    printf("    \"message\": \"%s\"\n", health->message);
    printf("  },\n");
    printf("  \"performance\": {\n");
    printf("    \"timestamp\": %ld,\n", perf->timestamp);
    printf("    \"node_count\": %d,\n", perf->node_count);
    printf("    \"disk_count\": %d\n", perf->disk_count);
    printf("  }\n");
    printf("}\n");
}

// Health status to restit value (0=OK, 1=WARNING, 2=CRITICAL)
static int health_to_restit_value(health_status_t_enum status) {
    switch (status) {
        case HEALTH_OK: return 0;
        case HEALTH_WARNING: return 1;
        case HEALTH_CRITICAL: return 2;
        default: return 1;
    }
}

// Output in restit-compatible format
void output_restit(health_status_t *health, node_info_t *nodes, int node_count,
                   disk_info_t *disks, int disk_count, pool_info_t *pools, int pool_count,
                   fcm_info_t *fcms, int fcm_count, perf_stat_t *perf_stats, int perf_stat_count,
                   host_info_t *hosts, int host_count, fc_port_info_t *fc_ports, int fc_port_count) {
    // System health sensor
    printf("%d system_status %s\n", 
           health_to_restit_value(health->overall),
           health->message);
    
    // Node health sensors
    for (int i = 0; i < node_count; i++) {
        int value = strcmp(nodes[i].status, "online") == 0 ? 0 : 2;
        printf("%d node_%s %s\n", value, nodes[i].name, nodes[i].status);
    }
    
    // Disk health sensors
    for (int i = 0; i < disk_count; i++) {
        int value = strcmp(disks[i].status, "online") == 0 ? 0 : 2;
        if (disks[i].fail_risk && strcmp(disks[i].fail_risk, "yes") == 0) {
            value = 2;  // Critical if fail_risk is yes
        }
        printf("%d disk_%s %s\n", value, disks[i].name, disks[i].status);
    }
    
    // Host health sensors with path status
    for (int i = 0; i < host_count; i++) {
        int value = strcmp(hosts[i].status, "online") == 0 ? 0 : 2;
        char message[128] = "";
        if (strcmp(hosts[i].status, "offline") == 0) {
            snprintf(message, sizeof(message), "host offline");
            value = 2;  // Critical if host is offline
        } else if (strcmp(hosts[i].status, "warning") == 0) {
            snprintf(message, sizeof(message), "host warning");
            value = 1;  // Warning
        }
        printf("%d host_%s %s\n", value, hosts[i].name, message[0] ? message : hosts[i].status);
    }
    
    // FC port health sensors
    for (int i = 0; i < fc_port_count; i++) {
        int value = strcmp(fc_ports[i].status, "active") == 0 ? 0 : 2;
        if (strcmp(fc_ports[i].status, "inactive") == 0) {
            value = 1;  // Warning if inactive
        }
        char message[128];
        snprintf(message, sizeof(message), "speed=%s node=%s", 
                 fc_ports[i].port_speed, fc_ports[i].node_name);
        printf("%d fc_%s_%s %s\n", value, fc_ports[i].node_name, fc_ports[i].port_id, message);
    }
    
    // Pool health sensors with usage percentage
    for (int i = 0; i < pool_count; i++) {
        int value = strcmp(pools[i].status, "online") == 0 ? 0 : 2;
        double used_pct = pools[i].capacity_total > 0 ? 
            (pools[i].capacity_used / pools[i].capacity_total) * 100.0 : 0.0;
        printf("%d pool_%s %s capacity_used=%.0fGB capacity_total=%.0fGB usage=%.0f%%\n", 
               value, pools[i].name, pools[i].status,
               pools[i].capacity_used / (1024.0 * 1024.0 * 1024.0),
               pools[i].capacity_total / (1024.0 * 1024.0 * 1024.0),
               used_pct);
        // Add usage percentage sensor
        printf("%.0f pool_%s_pct %.0f\n", used_pct, pools[i].name, used_pct);
    }
    
    // FCM health sensors
    for (int i = 0; i < fcm_count; i++) {
        int value = fcms[i].compressed ? 0 : 1;  // If compressed is yes, OK; otherwise maybe warning
        printf("%d fcm_%s %s capacity=%.0fGB total_drives=%d\n",
               value, fcms[i].name, fcms[i].compressed ? "compressed" : "not_compressed",
               fcms[i].capacity / (1024.0 * 1024.0 * 1024.0),
               fcms[i].total_count);
    }
    
    // Performance stats - key metrics
    for (int i = 0; i < perf_stat_count; i++) {
        if (strcmp(perf_stats[i].stat_name, "compression_cpu_pc") == 0) {
            printf("%.0f compression_cpu_usage %s\n", 
                   perf_stats[i].stat_current, perf_stats[i].stat_name);
        } else if (strcmp(perf_stats[i].stat_name, "cpu_pc") == 0) {
            printf("%.0f cpu_usage %s\n", 
                   perf_stats[i].stat_current, perf_stats[i].stat_name);
        } else if (strcmp(perf_stats[i].stat_name, "fc_mb") == 0) {
            printf("%.0f fc_throughput_mb %s\n", 
                   perf_stats[i].stat_current, perf_stats[i].stat_name);
        } else if (strcmp(perf_stats[i].stat_name, "fc_io") == 0) {
            printf("%.0f fc_io_operations %s\n", 
                   perf_stats[i].stat_current, perf_stats[i].stat_name);
        } else if (strcmp(perf_stats[i].stat_name, "write_cache_pc") == 0) {
            printf("%.0f write_cache_usage %s\n", 
                   perf_stats[i].stat_current, perf_stats[i].stat_name);
        } else if (strcmp(perf_stats[i].stat_name, "total_cache_pc") == 0) {
            printf("%.0f total_cache_usage %s\n", 
                   perf_stats[i].stat_current, perf_stats[i].stat_name);
        }
    }
}
