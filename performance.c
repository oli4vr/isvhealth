#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cjson/cJSON.h>
#include "http_client.h"
#include "performance.h"

// Get performance statistics directory listing
static int perf_get_stats_files(http_client_t *client, char ***files, int *count) {
    char *response = NULL;
    cJSON *json = NULL;
    cJSON *array = NULL;
    int num_files = 0;
    char **file_list = NULL;
    int result = -1;
    
    response = http_post(client, "/lsdumps", "{\"prefix\": \"/dumps/iostats\"}");
    if (!response) {
        fprintf(stderr, "Failed to get stats directory: %s\n", http_error(client));
        goto cleanup;
    }
    
    json = cJSON_Parse(response);
    if (!json || !cJSON_IsArray(json)) {
        fprintf(stderr, "Failed to parse lsdumps response\n");
        goto cleanup;
    }
    
    array = json;
    num_files = cJSON_GetArraySize(array);
    if (num_files == 0) {
        result = 0;
        goto cleanup;
    }
    
    file_list = (char **)calloc(num_files, sizeof(char *));
    if (!file_list) {
        fprintf(stderr, "Failed to allocate file list\n");
        goto cleanup;
    }
    
    for (int i = 0; i < num_files; i++) {
        cJSON *item = cJSON_GetArrayItem(array, i);
        if (item && cJSON_IsObject(item)) {
            cJSON *filename = cJSON_GetObjectItem(item, "filename");
            if (filename && cJSON_IsString(filename)) {
                file_list[i] = strdup(filename->valuestring);
            }
        }
    }
    
    result = 0;
    *files = file_list;
    *count = num_files;
    
cleanup:
    if (response) free(response);
    if (json && result != 0) cJSON_Delete(json);
    return result;
}

// Download and parse a single stats file
static cJSON *perf_download_stats(http_client_t *client, const char *filename) {
    char *response = NULL;
    cJSON *json = NULL;
    char *body = NULL;
    
    // Build JSON body
    asprintf(&body, "{\"prefix\": \"/dumps/iostats\", \"filename\": \"%s\"}", filename);
    
    response = http_post(client, "/download", body);
    free(body);
    
    if (!response) {
        fprintf(stderr, "Failed to download stats: %s\n", http_error(client));
        return NULL;
    }
    
    // Parse the response
    json = cJSON_Parse(response);
    free(response);
    
    return json;
}

// Get node statistics
int perf_get_node_stats(http_client_t *client, node_stats_t *stats, int max_count) {
    char **files;
    int file_count;
    cJSON *xml_json;
    
    if (perf_get_stats_files(client, &files, &file_count) != 0) {
        return -1;
    }
    
    int count = 0;
    for (int i = 0; i < file_count && count < max_count; i++) {
        if (files[i] && strstr(files[i], "Nn_stats") != NULL) {
            cJSON *json = perf_download_stats(client, files[i]);
            if (json) {
                // Parse Nn_stats JSON
                cJSON *data = cJSON_GetArrayItem(json, 0);
                if (data) {
                    stats[count].timestamp = time(NULL);
                    stats[count].node_id = json_get_int(data, "id", 0);
                    stats[count].node_name = strdup(json_get_string(data, "name", ""));
                    stats[count].cpu_percent = json_get_double(data, "cpu_percent", 0);
                    stats[count].memory_percent = json_get_double(data, "memory_percent", 0);
                    stats[count].read_io = json_get_int(data, "read_io", 0);
                    stats[count].write_io = json_get_int(data, "write_io", 0);
                    stats[count].read_latency = json_get_int(data, "read_latency", 0);
                    stats[count].write_latency = json_get_int(data, "write_latency", 0);
                    stats[count].queue_depth = json_get_int(data, "queue_depth", 0);
                    count++;
                }
                cJSON_Delete(json);
            }
        }
    }
    
    // Free files
    for (int i = 0; i < file_count; i++) {
        if (files[i]) free(files[i]);
    }
    free(files);
    
    return count;
}

// Get disk statistics
int perf_get_disk_stats(http_client_t *client, disk_stats_t *stats, int max_count) {
    char **files;
    int file_count;
    
    if (perf_get_stats_files(client, &files, &file_count) != 0) {
        return -1;
    }
    
    int count = 0;
    for (int i = 0; i < file_count && count < max_count; i++) {
        if (files[i] && strstr(files[i], "Nd_stats") != NULL) {
            cJSON *json = perf_download_stats(client, files[i]);
            if (json) {
                // Parse Nd_stats JSON
                cJSON *data = cJSON_GetArrayItem(json, 0);
                if (data) {
                    stats[count].timestamp = time(NULL);
                    stats[count].disk_id = json_get_int(data, "id", 0);
                    stats[count].disk_name = strdup(json_get_string(data, "name", ""));
                    stats[count].read_io = json_get_int(data, "read_io", 0);
                    stats[count].write_io = json_get_int(data, "write_io", 0);
                    stats[count].read_bytes = json_get_int(data, "read_bytes", 0);
                    stats[count].write_bytes = json_get_int(data, "write_bytes", 0);
                    stats[count].read_latency = json_get_int(data, "read_latency", 0);
                    stats[count].write_latency = json_get_int(data, "write_latency", 0);
                    stats[count].queue_depth = json_get_int(data, "queue_depth", 0);
                    stats[count].temp = json_get_int(data, "temp", 0);
                    count++;
                }
                cJSON_Delete(json);
            }
        }
    }
    
    for (int i = 0; i < file_count; i++) {
        if (files[i]) free(files[i]);
    }
    free(files);
    
    return count;
}

// Get volume statistics
int perf_get_volume_stats(http_client_t *client, volume_stats_t *stats, int max_count) {
    char **files;
    int file_count;
    
    if (perf_get_stats_files(client, &files, &file_count) != 0) {
        return -1;
    }
    
    int count = 0;
    for (int i = 0; i < file_count && count < max_count; i++) {
        if (files[i] && strstr(files[i], "Ng_stats") != NULL) {
            cJSON *json = perf_download_stats(client, files[i]);
            if (json) {
                // Parse Ng_stats JSON
                cJSON *data = cJSON_GetArrayItem(json, 0);
                if (data) {
                    stats[count].timestamp = time(NULL);
                    stats[count].volume_id = json_get_int(data, "id", 0);
                    stats[count].volume_name = strdup(json_get_string(data, "name", ""));
                    stats[count].read_io = json_get_int(data, "read_io", 0);
                    stats[count].write_io = json_get_int(data, "write_io", 0);
                    stats[count].read_bytes = json_get_int(data, "read_bytes", 0);
                    stats[count].write_bytes = json_get_int(data, "write_bytes", 0);
                    stats[count].read_latency = json_get_int(data, "read_latency", 0);
                    stats[count].write_latency = json_get_int(data, "write_latency", 0);
                    stats[count].queue_depth = json_get_int(data, "queue_depth", 0);
                    count++;
                }
                cJSON_Delete(json);
            }
        }
    }
    
    for (int i = 0; i < file_count; i++) {
        if (files[i]) free(files[i]);
    }
    free(files);
    
    return count;
}

// Get FC port statistics
int perf_get_fc_port_stats(http_client_t *client, fc_port_stats_t *stats, int max_count) {
    char **files;
    int file_count;
    
    if (perf_get_stats_files(client, &files, &file_count) != 0) {
        return -1;
    }
    
    int count = 0;
    for (int i = 0; i < file_count && count < max_count; i++) {
        if (files[i] && strstr(files[i], "Nv_stats") != NULL) {
            cJSON *json = perf_download_stats(client, files[i]);
            if (json) {
                // Parse Nv_stats JSON
                cJSON *data = cJSON_GetArrayItem(json, 0);
                if (data) {
                    stats[count].timestamp = time(NULL);
                    stats[count].port_id = json_get_int(data, "id", 0);
                    stats[count].port_name = strdup(json_get_string(data, "name", ""));
                    stats[count].rx_bytes = json_get_int(data, "rx_bytes", 0);
                    stats[count].tx_bytes = json_get_int(data, "tx_bytes", 0);
                    stats[count].rx_frames = json_get_int(data, "rx_frames", 0);
                    stats[count].tx_frames = json_get_int(data, "tx_frames", 0);
                    stats[count].rx_errors = json_get_int(data, "rx_errors", 0);
                    stats[count].tx_errors = json_get_int(data, "tx_errors", 0);
                    stats[count].link_failures = json_get_int(data, "link_failures", 0);
                    count++;
                }
                cJSON_Delete(json);
            }
        }
    }
    
    for (int i = 0; i < file_count; i++) {
        if (files[i]) free(files[i]);
    }
    free(files);
    
    return count;
}

// Get host statistics
int perf_get_host_stats(http_client_t *client, host_stats_t *stats, int max_count) {
    char **files;
    int file_count;
    
    if (perf_get_stats_files(client, &files, &file_count) != 0) {
        return -1;
    }
    
    int count = 0;
    for (int i = 0; i < file_count && count < max_count; i++) {
        if (files[i] && strstr(files[i], "Nm_stats") != NULL) {
            cJSON *json = perf_download_stats(client, files[i]);
            if (json) {
                // Parse Nm_stats JSON
                cJSON *data = cJSON_GetArrayItem(json, 0);
                if (data) {
                    stats[count].timestamp = time(NULL);
                    stats[count].host_id = json_get_int(data, "id", 0);
                    stats[count].host_name = strdup(json_get_string(data, "name", ""));
                    stats[count].read_io = json_get_int(data, "read_io", 0);
                    stats[count].write_io = json_get_int(data, "write_io", 0);
                    stats[count].read_bytes = json_get_int(data, "read_bytes", 0);
                    stats[count].write_bytes = json_get_int(data, "write_bytes", 0);
                    stats[count].read_latency = json_get_int(data, "read_latency", 0);
                    stats[count].write_latency = json_get_int(data, "write_latency", 0);
                    stats[count].queue_depth = json_get_int(data, "queue_depth", 0);
                    count++;
                }
                cJSON_Delete(json);
            }
        }
    }
    
    for (int i = 0; i < file_count; i++) {
        if (files[i]) free(files[i]);
    }
    free(files);
    
    return count;
}

// Get all performance statistics
int perf_get_all(http_client_t *client, perf_stats_t *all_stats) {
    memset(all_stats, 0, sizeof(perf_stats_t));
    
    all_stats->node_count = perf_get_node_stats(client, all_stats->nodes, MAX_NODES);
    all_stats->disk_count = perf_get_disk_stats(client, all_stats->disks, MAX_DISKS);
    all_stats->volume_count = perf_get_volume_stats(client, all_stats->volumes, MAX_VOLUMES);
    all_stats->fc_port_count = perf_get_fc_port_stats(client, all_stats->fc_ports, MAX_FC_PORTS);
    all_stats->host_count = perf_get_host_stats(client, all_stats->hosts, MAX_HOSTS);
    
    return 0;
}
