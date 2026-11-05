/*
 * http_client.h - HTTP Client Interface for IBM FlashSystem REST API
 * 
 * This header defines the HTTP client structure and functions used to
 * communicate with the IBM FlashSystem REST API. It provides functions
 * for initialization, authentication, and making HTTP requests.
 * 
 * The client uses libcurl for HTTPS communication and handles:
 * - Token-based authentication
 * - Username/password authentication
 * - SSL/TLS connections (with optional certificate verification)
 * - Proxy bypass for direct network access
 */

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stddef.h>

// HTTP client structure
typedef struct {
    char *base_url;       // Base URL for API endpoints (https://host:port/rest/v1)
    char *host;           // Hostname/IP address
    char *token;          // Authentication token
    char *username;       // Username for authentication
    char *password;       // Password for authentication
    int port;             // REST API port (default: 7443)
    int verify_ssl;       // Whether to verify SSL certificates
    char *last_error;     // Last error message
    size_t last_error_size; // Size of error message buffer
} http_client_t;

/**
 * Initialize HTTP client
 * 
 * Creates a new HTTP client instance with the specified host and port.
 * Initializes libcurl global state.
 * 
 * @param host FlashSystem hostname or IP address
 * @param port REST API port (typically 7443)
 * @return Pointer to http_client_t on success, NULL on failure
 */
http_client_t *http_client_init(const char *host, int port);

/**
 * Free HTTP client resources
 * 
 * Releases all memory allocated for the HTTP client and cleans up
 * libcurl global state.
 * 
 * @param client HTTP client to free (may be NULL)
 */
void http_client_free(http_client_t *client);

/**
 * Set authentication credentials
 * 
 * Stores username and password for authentication. These are used
 * when the token is not available or needs to be refreshed.
 * 
 * @param client HTTP client
 * @param username Username for authentication
 * @param password Password for authentication
 * @return 0 on success, -1 on failure
 */
int http_client_set_auth(http_client_t *client, const char *username, const char *password);

/**
 * Set authentication token
 * 
 * Sets the authentication token to be used for subsequent requests.
 * The token should be obtained via auth_get_token() or provided externally.
 * 
 * @param client HTTP client
 * @param token Authentication token string
 * @return 0 on success, -1 on failure
 */
int http_client_set_token(http_client_t *client, const char *token);

/**
 * Perform GET request
 * 
 * Sends an HTTP GET request to the specified endpoint.
 * 
 * @param client HTTP client
 * @param endpoint API endpoint (e.g., "/lsnode")
 * @return Response string on success, NULL on failure (caller must free)
 */
char *http_get(http_client_t *client, const char *endpoint);

/**
 * Perform POST request with JSON body
 * 
 * Sends an HTTP POST request to the specified endpoint with optional JSON body.
 * 
 * @param client HTTP client
 * @param endpoint API endpoint (e.g., "/auth")
 * @param json_body JSON request body (may be NULL)
 * @return Response string on success, NULL on failure (caller must free)
 */
char *http_post(http_client_t *client, const char *endpoint, const char *json_body);

/**
 * Check if HTTP status code indicates success
 * 
 * @param status_code HTTP status code
 * @return 1 if success (2xx), 0 otherwise
 */
int http_success(int status_code);

/**
 * Get last error message
 * 
 * @param client HTTP client
 * @return Error message string (may be NULL)
 */
const char *http_error(http_client_t *client);

#endif // HTTP_CLIENT_H
