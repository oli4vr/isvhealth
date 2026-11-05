/*
 * output.h - Output Formatting Interface for IBM FlashSystem Health Monitoring
 * 
 * This header defines functions for formatting health and performance data
 * for display. It supports:
 * - Plain text output (human-readable)
 * - JSON output (machine-readable for monitoring systems)
 * - Restit-compatible output
 * - Combined health and performance reports
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include <time.h>
#include "health.h"
#include "performance.h"
#include "json_parser.h"

/**
 * Output health status in plain text format
 * 
 * Displays health information in a human-readable format with sections
 * for nodes, disks, pools, volumes, hosts, enclosures, and alerts.
 * 
 * @param status Overall system health status
 * @param nodes Node information array
 * @param node_count Number of nodes
 * @param disks Disk information array
 * @param disk_count Number of disks
 * @param pools Pool information array (may be NULL)
 * @param pool_count Number of pools
 * @param ports Port information array (may be NULL)
 * @param port_count Number of ports
 * @param volumes Volume information array
 * @param volume_count Number of volumes
 * @param hosts Host information array
 * @param host_count Number of hosts
 * @param fc_ports FC port information array
 * @param fc_port_count Number of FC ports
 */
void output_health_txt(health_status_t *status, node_info_t *nodes, int node_count,
                       disk_info_t *disks, int disk_count, pool_info_t *pools, int pool_count,
                       port_info_t *ports, int port_count, volume_info_t *volumes, int volume_count,
                       host_info_t *hosts, int host_count, fc_port_info_t *fc_ports, int fc_port_count);

/**
 * Output health status in JSON format
 * 
 * Outputs health information as structured JSON suitable for
 * integration with monitoring systems like Prometheus, Zabbix, etc.
 * 
 * @param status Overall system health status
 * @param nodes Node information array
 * @param node_count Number of nodes
 * @param disks Disk information array
 * @param disk_count Number of disks
 * @param pools Pool information array (may be NULL)
 * @param pool_count Number of pools
 * @param ports Port information array (may be NULL)
 * @param port_count Number of ports
 * @param volumes Volume information array
 * @param volume_count Number of volumes
 * @param hosts Host information array
 * @param host_count Number of hosts
 * @param fc_ports FC port information array
 * @param fc_port_count Number of FC ports
 */
void output_health_json(health_status_t *status, node_info_t *nodes, int node_count,
                        disk_info_t *disks, int disk_count, pool_info_t *pools, int pool_count,
                        port_info_t *ports, int port_count, volume_info_t *volumes, int volume_count,
                        host_info_t *hosts, int host_count, fc_port_info_t *fc_ports, int fc_port_count);

/**
 * Output performance data in plain text format
 * 
 * Displays performance statistics in a human-readable format with sections
 * for nodes, disks, ports, and hosts.
 * 
 * @param stats Performance statistics
 */
void output_perf_txt(perf_stats_t *stats);

/**
 * Output performance data in JSON format
 * 
 * Outputs performance statistics as structured JSON.
 * 
 * @param stats Performance statistics
 */
void output_perf_json(perf_stats_t *stats);

/**
 * Output combined health and performance in plain text format
 * 
 * Displays both health status and performance metrics together
 * for a comprehensive system view.
 * 
 * @param health System health status
 * @param perf Performance statistics
 * @param nodes Node information
 * @param node_count Number of nodes
 * @param disks Disk information
 * @param disk_count Number of disks
 * @param pools Pool information (may be NULL)
 * @param pool_count Number of pools
 * @param ports Port information (may be NULL)
 * @param port_count Number of ports
 */
void output_combined_txt(health_status_t *health, perf_stats_t *perf,
                         node_info_t *nodes, int node_count,
                         disk_info_t *disks, int disk_count,
                         pool_info_t *pools, int pool_count,
                         port_info_t *ports, int port_count);

/**
 * Output combined health and performance in JSON format
 * 
 * Outputs both health status and performance metrics as structured JSON.
 * 
 * @param health System health status
 * @param perf Performance statistics
 * @param nodes Node information
 * @param node_count Number of nodes
 * @param disks Disk information
 * @param disk_count Number of disks
 */
void output_combined_json(health_status_t *health, perf_stats_t *perf,
                          node_info_t *nodes, int node_count,
                          disk_info_t *disks, int disk_count);

/**
 * Output in restit-compatible format
 * 
 * Outputs health and performance data in the format expected by restit:
 * VALUE SENSORNAME OPTIONAL_MESSAGE
 * 
 * Health sensor values:
 * - 0 = OK
 * - 1 = WARNING  
 * - 2 = CRITICAL
 * 
 * @param health System health status
 * @param nodes Node information
 * @param node_count Number of nodes
 * @param disks Disk information
 * @param disk_count Number of disks
 * @param pools Pool information
 * @param pool_count Number of pools
 * @param fcms FCM information
 * @param fcm_count Number of FCMs
 * @param perf_stats Performance statistics
 * @param perf_stat_count Number of performance stats
 * @param hosts Host information
 * @param host_count Number of hosts
 * @param fc_ports FC port information
 * @param fc_port_count Number of FC ports
 */
void output_restit(health_status_t *health, node_info_t *nodes, int node_count,
                   disk_info_t *disks, int disk_count, pool_info_t *pools, int pool_count,
                   fcm_info_t *fcms, int fcm_count, perf_stat_t *perf_stats, int perf_stat_count,
                   host_info_t *hosts, int host_count, fc_port_info_t *fc_ports, int fc_port_count);

#endif // OUTPUT_H
