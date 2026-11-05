/*
 * json_parser.h - JSON Parser Interface for IBM FlashSystem Health Monitoring
 * 
 * This header defines data structures and function prototypes for parsing
 * JSON responses from the FlashSystem REST API. It supports:
 * - Node/Controller information
 * - Disk information
 * - Storage pool information
 * - Volume information
 * - Host information
 * - Enclosure information
 * - Alert/event information
 * - Performance statistics
 */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <cjson/cJSON.h>

// Data structures for parsed information

/**
 * Node/Controller information
 * Represents a FlashSystem controller node with its status and configuration.
 */
typedef struct {
    char *id;           // Node ID
    char *name;         // Node name
    char *status;       // Status (online, offline, etc.)
    char *role;         // Role (config_node, member)
    char *location;     // Physical location
    char *node_type;    // Node type
    char *protocol;     // Protocol type
    int port;           // Port number
    char *ip_address;   // IP address
    int speed;          // Speed in Mbps
    double cpu_usage;   // CPU usage percentage
    double memory_usage;// Memory usage percentage
    int uptime;         // Uptime in seconds
    int storage_managed;// Storage managed flag
    int fc_count;       // FC port count
    int eth_count;      // Ethernet port count
    int mdisk_count;    // Managed disk count
} node_info_t;

/**
 * Port information
 * Represents an I/O port with its connection details and statistics.
 */
typedef struct {
    char *id;           // Port ID
    char *name;         // Port name
    char *port_type;    // Port type (fc, iscsi, sas)
    int node_id;        // Parent node ID
    char *node_name;    // Parent node name
    char *status;       // Status (online, offline)
    int speed;          // Speed in Mbps
    char *fc_port_id;   // FC port ID
    char *wwnn;         // World wide node name
    char *wwpn;         // World wide port name
    char *fabric;       // Fabric name
    int zoning_enabled; // Zoning enabled flag
    long long rx_bytes; // Received bytes
    long long tx_bytes; // Transmitted bytes
    int rx_frames;      // Received frames
    int tx_frames;      // Transmitted frames
} port_info_t;

/**
 * Disk information
 * Represents a physical disk with its capacity and health status.
 */
typedef struct {
    char *id;           // Disk ID
    char *mdisk_id;     // Managed disk ID
    char *name;         // Disk name
    char *status;       // Status (online, offline, etc.)
    char *location;     // Physical location
    int speed;          // Speed in RPM
    char *type;         // Disk type
    double capacity;    // Capacity in bytes
    double used;        // Used capacity in bytes
    double available;   // Available capacity in bytes
    int temperature;    // Temperature in Celsius
    char *fail_risk;    // Failure risk indicator ("yes" or empty)
    char *model;        // Disk model
    char *serial;       // Disk serial number
} disk_info_t;

/**
 * Flash Core Module (FCM) information
 * Represents a Flash Core Module with its capacity and compression stats.
 */
typedef struct {
    char *id;                   // FCM ID
    char *name;                 // FCM name
    double capacity;            // FCM capacity in bytes
    int tech_type;              // Technology type (0=flash, 1=hdd, etc.)
    int total_count;            // Total drives in FCM
    int compressed;             // Compression enabled
    int deduplicated;           // Deduplication enabled
    double physical_capacity;   // Physical capacity in bytes
    char *transport_protocol;   // Transport protocol (nvme, sas, etc.)
} fcm_info_t;

/**
 * Storage pool information
 * Represents a storage pool with capacity and tier information.
 */
typedef struct {
    char *id;           // Pool ID
    char *name;         // Pool name
    char *status;       // Status (online, offline)
    char *pool_type;    // Pool type
    char *tier;         // Storage tier (tier0_flash, tier1, etc.)
    double capacity_total;     // Total capacity in bytes
    double capacity_used;      // Used capacity in bytes
    double capacity_available; // Available capacity in bytes
    double capacity_savings;   // Capacity savings percentage
    int compression_active;    // Compression active flag
    int deduplication_active;  // Deduplication active flag
    int volume_count;          // Number of volumes in pool
    int easy_tier;             // EasyTier enabled flag
    char *easy_tier_status;    // EasyTier status
    // FCM-level compression stats
    double compression_virtual_capacity;       // Virtual capacity with compression
    double compression_compressed_capacity;    // Compressed capacity
    double compression_uncompressed_capacity;  // Uncompressed capacity
} pool_info_t;

/**
 * Volume information
 * Represents a storage volume with its properties.
 */
typedef struct {
    char *id;           // Volume ID
    char *name;         // Volume name
    char *status;       // Status (online, offline)
    char *mdisk_id;     // Managed disk ID
    char *mdisk_name;   // Managed disk name
    char *pool_id;      // Pool ID
    char *pool_name;    // Pool name
    double capacity;    // Capacity in bytes
    double used;        // Used capacity in bytes
    char *vtype;        // Volume type
    int rsize;          // Retention size
    int compressed;     // Compressed flag
    int deduplicated;   // Deduplicated flag
    int easy_tier;      // EasyTier enabled flag
    int mirror_count;   // Mirror count
    int host_count;     // Host count
} volume_info_t;

/**
 * Host information
 * Represents a host connected to the FlashSystem.
 */
typedef struct {
    char *id;           // Host ID
    char *name;         // Host name
    char *status;       // Status (online, offline)
    char *host_type;    // Host type
    char *os_type;      // Operating system type
    int partition_count;// Partition count
    int vdisk_count;    // Virtual disk count
    char *host_cluster_id;     // Cluster ID
    char *host_cluster_name;   // Cluster name
} host_info_t;

/**
 * Enclosure information
 * Represents a hardware enclosure with its components.
 */
typedef struct {
    char *id;           // Enclosure ID
    char *name;         // Enclosure name
    char *status;       // Status (online, offline)
    char *location;     // Physical location
    char *type;         // Enclosure type
    char *node_id;      // Parent node ID
    char *node_name;    // Parent node name
    int power_supplies; // Power supply count
    int fans;           // Fan count
    int temperatures;   // Temperature sensor count
    int drive_slots;    // Drive slot count
} enclosure_info_t;

/**
 * Alert/Event information
 * Represents a system alert or event with severity and details.
 */
typedef struct {
    char *id;           // Event ID
    char *severity;     // Severity (critical, error, warning, info)
    char *status;       // Status (active, cleared)
    int event_id;       // Event number
    char *event_time;   // Event timestamp
    char *location;     // Location
    char *component_type;   // Component type
    char *component_id;     // Component ID
    char *message;          // Event message
} alert_info_t;

/**
 * Performance statistics
 * Represents system performance metrics.
 */
typedef struct {
    char *stat_name;        // Statistic name
    double stat_current;    // Current value
    double stat_peak;       // Peak value
    long long stat_peak_time; // Peak timestamp
} perf_stat_t;

/**
 * Fibre Channel port information
 * Represents a Fibre Channel port with its connection details and status.
 */
typedef struct {
    char *id;           // Port ID
    char *fc_io_port_id;// FC I/O port ID
    char *port_id;      // Port identifier
    char *port_type;    // Port type (fc, iscsi, sas)
    int node_id;        // Parent node ID
    char *node_name;    // Parent node name
    char *status;       // Status (active, inactive, offline)
    char *port_speed;   // Port speed (e.g., "32Gb", "16Gb")
    char *wwpn;         // World wide port name
    char *nportid;      // N-Port ID
    int fabric_id;      // Fabric ID
    char *fabric_name;  // Fabric name
    int connected;      // Connected flag
    int zoning_enabled; // Zoning enabled flag
} fc_port_info_t;

// JSON parsing utility functions

/**
 * Parse an integer value from JSON object
 * 
 * @param obj JSON object
 * @param key Key to look up
 * @param default_val Default value if key not found
 * @return Integer value or default
 */
int json_get_int(cJSON *obj, const char *key, int default_val);

/**
 * Parse a string value from JSON object
 * 
 * @param obj JSON object
 * @param key Key to look up
 * @param default_val Default value if key not found
 * @return String value or default
 */
const char *json_get_string(cJSON *obj, const char *key, const char *default_val);

/**
 * Parse a double value from JSON object
 * 
 * @param obj JSON object
 * @param key Key to look up
 * @param default_val Default value if key not found
 * @return Double value or default
 */
double json_get_double(cJSON *obj, const char *key, double default_val);

/**
 * Parse a boolean value from JSON object
 * 
 * @param obj JSON object
 * @param key Key to look up
 * @param default_val Default value if key not found
 * @return Boolean value (1/0) or default
 */
int json_get_bool(cJSON *obj, const char *key, int default_val);

/**
 * Get array size from JSON object
 * 
 * @param obj JSON object
 * @param key Key containing array
 * @return Array size or 0
 */
int json_get_array_size(cJSON *obj, const char *key);

/**
 * Get array item from JSON object
 * 
 * @param obj JSON object
 * @param key Key containing array
 * @param index Array index
 * @return Array item or NULL
 */
cJSON *json_get_array_item(cJSON *obj, const char *key, int index);

// Health status parsing functions

/**
 * Parse node information from JSON response
 * 
 * @param response JSON response array
 * @param nodes Output node array
 * @param max_count Maximum nodes to parse
 * @return Number of nodes parsed, or -1 on error
 */
int json_parse_nodes(cJSON *response, node_info_t *nodes, int max_count);

/**
 * Parse port information from JSON response
 * 
 * @param response JSON response array
 * @param ports Output port array
 * @param max_count Maximum ports to parse
 * @return Number of ports parsed, or -1 on error
 */
int json_parse_ports(cJSON *response, port_info_t *ports, int max_count);

/**
 * Parse disk information from JSON response
 * 
 * @param response JSON response array
 * @param disks Output disk array
 * @param max_count Maximum disks to parse
 * @return Number of disks parsed, or -1 on error
 */
int json_parse_disks(cJSON *response, disk_info_t *disks, int max_count);

/**
 * Parse pool information from JSON response
 * 
 * @param response JSON response array
 * @param pools Output pool array
 * @param max_count Maximum pools to parse
 * @return Number of pools parsed, or -1 on error
 */
int json_parse_pools(cJSON *response, pool_info_t *pools, int max_count);

/**
 * Parse volume information from JSON response
 * 
 * @param response JSON response array
 * @param volumes Output volume array
 * @param max_count Maximum volumes to parse
 * @return Number of volumes parsed, or -1 on error
 */
int json_parse_volumes(cJSON *response, volume_info_t *volumes, int max_count);

/**
 * Parse host information from JSON response
 * 
 * @param response JSON response array
 * @param hosts Output host array
 * @param max_count Maximum hosts to parse
 * @return Number of hosts parsed, or -1 on error
 */
int json_parse_hosts(cJSON *response, host_info_t *hosts, int max_count);

/**
 * Parse enclosure information from JSON response
 * 
 * @param response JSON response array
 * @param enclosures Output enclosure array
 * @param max_count Maximum enclosures to parse
 * @return Number of enclosures parsed, or -1 on error
 */
int json_parse_enclosures(cJSON *response, enclosure_info_t *enclosures, int max_count);

/**
 * Parse alert information from JSON response
 * 
 * @param response JSON response array
 * @param alerts Output alert array
 * @param max_count Maximum alerts to parse
 * @return Number of alerts parsed, or -1 on error
 */
int json_parse_alerts(cJSON *response, alert_info_t *alerts, int max_count);

/**
 * Parse FCM (Flash Core Module) information from JSON response
 * 
 * @param response JSON response array
 * @param fcms Output FCM array
 * @param max_count Maximum FCMs to parse
 * @return Number of FCMs parsed, or -1 on error
 */
int json_parse_fcms(cJSON *response, fcm_info_t *fcms, int max_count);

/**
 * Parse performance statistics from JSON response
 * 
 * @param response JSON response array
 * @param stats Output statistics array
 * @param max_count Maximum stats to parse
 * @return Number of stats parsed, or -1 on error
 */
int json_parse_perf_stats(cJSON *response, perf_stat_t *stats, int max_count);

/**
 * Parse Fibre Channel port information from JSON response
 * 
 * @param response JSON response array
 * @param ports Output port array
 * @param max_count Maximum ports to parse
 * @return Number of ports parsed, or -1 on error
 */
int json_parse_fc_ports(cJSON *response, fc_port_info_t *ports, int max_count);

// Free functions - Release allocated memory

/**
 * Free node information memory
 * 
 * @param nodes Node array
 * @param count Number of nodes
 */
void node_info_free(node_info_t *nodes, int count);

/**
 * Free port information memory
 * 
 * @param ports Port array
 * @param count Number of ports
 */
void port_info_free(port_info_t *ports, int count);

/**
 * Free disk information memory
 * 
 * @param disks Disk array
 * @param count Number of disks
 */
void disk_info_free(disk_info_t *disks, int count);

/**
 * Free pool information memory
 * 
 * @param pools Pool array
 * @param count Number of pools
 */
void pool_info_free(pool_info_t *pools, int count);

/**
 * Free volume information memory
 * 
 * @param volumes Volume array
 * @param count Number of volumes
 */
void volume_info_free(volume_info_t *volumes, int count);

/**
 * Free host information memory
 * 
 * @param hosts Host array
 * @param count Number of hosts
 */
void host_info_free(host_info_t *hosts, int count);

/**
 * Free enclosure information memory
 * 
 * @param enclosures Enclosure array
 * @param count Number of enclosures
 */
void enclosure_info_free(enclosure_info_t *enclosures, int count);

/**
 * Free alert information memory
 * 
 * @param alerts Alert array
 * @param count Number of alerts
 */
void alert_info_free(alert_info_t *alerts, int count);

/**
 * Free FCM information memory
 * 
 * @param fcms FCM array
 * @param count Number of FCMs
 */
void fcm_info_free(fcm_info_t *fcms, int count);

/**
 * Free performance statistics memory
 * 
 * @param stats Statistics array
 * @param count Number of stats
 */
void perf_stat_free(perf_stat_t *stats, int count);

/**
 * Free FC port information memory
 * 
 * @param ports FC port array
 * @param count Number of ports
 */
void fc_port_info_free(fc_port_info_t *ports, int count);

#endif // JSON_PARSER_H
