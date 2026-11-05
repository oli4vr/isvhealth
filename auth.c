/*
 * auth.c - Authentication Implementation for IBM FlashSystem REST API
 * 
 * This module implements authentication functionality for the FlashSystem
 * REST API. It handles:
 * - Token retrieval via authentication endpoint
 * - Token storage and management
 * - System information retrieval
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "http_client.h"
#include "json_parser.h"

extern int g_verbose;
extern int g_debug;

/**
 * Get authentication token
 * 
 * Authenticates with the FlashSystem using the credentials set in the
 * HTTP client and retrieves an authentication token.
 * 
 * The function first attempts manual JSON parsing to avoid issues with
 * the simplified cJSON implementation when handling JWT tokens.
 * 
 * @param client HTTP client with username/password set
 * @return Authentication token string on success (caller must free), NULL on failure
 */
char *auth_get_token(http_client_t *client) {
    char *response;
    cJSON *json = NULL;
    char *token = NULL;
    
    response = http_post(client, "/rest/v1/auth", NULL);
    if (!response) {
        fprintf(stderr, "DEBUG: auth_get_token failed - %s\n", http_error(client));
        return NULL;
    }
    
    if (g_verbose || g_debug) fprintf(stderr, "DEBUG: auth response: %s\n", response);
    
    /* Try manual parsing first to avoid cJSON issues with JWT tokens */
    char *token_start = strstr(response, "\"token\":\"");
    if (token_start) {
        token_start += 9;
        char *token_end = strchr(token_start, '"');
        if (token_end && token_end > token_start) {
            size_t token_len = token_end - token_start;
            token = malloc(token_len + 1);
            if (token) {
                memcpy(token, token_start, token_len);
                token[token_len] = '\0';
                free(response);
                return token;
            }
        }
    }
    
    json = cJSON_Parse(response);
    if (!json) {
        fprintf(stderr, "DEBUG: Failed to parse authentication response\n");
        free(response);
        return NULL;
    }
    
    if (!cJSON_IsObject(json)) {
        fprintf(stderr, "DEBUG: Response is not a JSON object\n");
        cJSON_Delete(json);
        free(response);
        return NULL;
    }
    
    cJSON *token_item = cJSON_GetObjectItem(json, "token");
    if (token_item && cJSON_IsString(token_item) && token_item->valuestring) {
        token = strdup(token_item->valuestring);
    } else {
        fprintf(stderr, "DEBUG: No token found in response\n");
        cJSON_Delete(json);
        free(response);
        return NULL;
    }
    
    cJSON_Delete(json);
    free(response);
    
    return token;
}

/**
 * Login with username/password
 * 
 * Performs complete authentication flow: sets credentials, gets token,
 * and stores token in the client.
 * 
 * @param client HTTP client
 * @param username Username for authentication
 * @param password Password for authentication
 * @return 0 on success, -1 on failure
 */
int auth_login(http_client_t *client, const char *username, const char *password) {
    if (http_client_set_auth(client, username, password) != 0) {
        fprintf(stderr, "DEBUG: Failed to set authentication credentials\n");
        return -1;
    }
    
    if (g_verbose || g_debug) fprintf(stderr, "DEBUG: Using username/password authentication (user: %s)\n", username ? username : "");
    
    if (g_debug) fprintf(stderr, "DEBUG: About to call auth_get_token\n");
    char *token = auth_get_token(client);
    if (!token) {
        fprintf(stderr, "DEBUG: auth_get_token failed\n");
        return -1;
    }
    if (g_debug) {
        fprintf(stderr, "DEBUG: Got token, setting it\n");
        fprintf(stderr, "DEBUG: Token length: %lu\n", strlen(token));
    }
    
    if (g_debug) fprintf(stderr, "DEBUG: Calling http_client_set_token\n");
    if (http_client_set_token(client, token) != 0) {
        free(token);
        fprintf(stderr, "DEBUG: Failed to set token\n");
        return -1;
    }
    
    if (g_verbose || g_debug) fprintf(stderr, "DEBUG: auth_login succeeded\n");
    free(token);
    return 0;
}

/**
 * Get system information
 * 
 * Retrieves system identification information from the FlashSystem
 * using the lslicense endpoint.
 * 
 * @param client HTTP client (must be authenticated)
 * @param system_id Pointer to store system ID (may be NULL)
 * @param system_name Pointer to store system name (may be NULL)
 * @param version Pointer to store version string (may be NULL)
 * @return 0 on success, -1 on failure
 */
int auth_get_system_info(http_client_t *client, char **system_id, char **system_name, char **version) {
    char *response;
    cJSON *json;
    
    if (g_debug) fprintf(stderr, "DEBUG: Calling /lslicense\n");
    // Use POST instead of GET - FlashSystem API requires POST for all endpoints
    response = http_post(client, "/lslicense", NULL);
    if (!response) {
        if (g_debug) fprintf(stderr, "DEBUG: /lslicense failed\n");
        return -1;
    }
    
    if (g_debug) fprintf(stderr, "DEBUG: /lslicense response: %s\n", response);
    
    json = cJSON_Parse(response);
    free(response);
    
    if (!json) {
        if (g_debug) fprintf(stderr, "DEBUG: Failed to parse lslicense\n");
        return -1;
    }
    
    cJSON *array = cJSON_GetArrayItem(json, 0);
    if (array && cJSON_IsObject(array)) {
        if (system_id) {
            cJSON *item = cJSON_GetObjectItem(array, "id");
            *system_id = item && cJSON_IsString(item) ? strdup(item->valuestring) : strdup("");
        }
        if (system_name) {
            cJSON *item = cJSON_GetObjectItem(array, "name");
            *system_name = item && cJSON_IsString(item) ? strdup(item->valuestring) : strdup("");
        }
        if (version) {
            cJSON *item = cJSON_GetObjectItem(array, "code_level");
            if (item && cJSON_IsString(item)) {
                *version = strdup(item->valuestring);
            } else {
                item = cJSON_GetObjectItem(array, "version");
                *version = item && cJSON_IsString(item) ? strdup(item->valuestring) : strdup("");
            }
        }
    }
    
    cJSON_Delete(json);
    return 0;
}

/**
 * Check if client is authenticated
 * 
 * @param client HTTP client
 * @return 1 if authenticated (has valid token), 0 otherwise
 */
int auth_is_authenticated(http_client_t *client) {
    return client && client->token != NULL;
}

/**
 * Logout (invalidate token)
 * 
 * Clears the authentication token from the client structure.
 * 
 * @param client HTTP client
 */
void auth_logout(http_client_t *client) {
    if (client) {
        if (client->token) {
            free(client->token);
            client->token = NULL;
        }
    }
}
