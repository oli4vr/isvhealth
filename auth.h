/*
 * auth.h - Authentication Module for IBM FlashSystem REST API
 * 
 * This header defines functions for authenticating with the FlashSystem
 * REST API. It handles:
 * - Token-based authentication
 * - Username/password authentication
 * - System information retrieval
 * - Token lifecycle management
 */

#ifndef AUTH_H
#define AUTH_H

#include "http_client.h"

/**
 * Get authentication token
 * 
 * Authenticates with the FlashSystem using the credentials set in the
 * HTTP client and retrieves an authentication token.
 * 
 * @param client HTTP client with username/password set
 * @return Authentication token string on success (caller must free), NULL on failure
 */
char *auth_get_token(http_client_t *client);

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
int auth_login(http_client_t *client, const char *username, const char *password);

/**
 * Get system information
 * 
 * Retrieves system identification information from the FlashSystem.
 * 
 * @param client HTTP client (must be authenticated)
 * @param system_id Pointer to store system ID (may be NULL)
 * @param system_name Pointer to store system name (may be NULL)
 * @param version Pointer to store version string (may be NULL)
 * @return 0 on success, -1 on failure
 */
int auth_get_system_info(http_client_t *client, char **system_id, char **system_name, char **version);

/**
 * Check if client is authenticated
 * 
 * @param client HTTP client
 * @return 1 if authenticated, 0 otherwise
 */
int auth_is_authenticated(http_client_t *client);

/**
 * Logout (invalidate token)
 * 
 * Clears the authentication token from the client.
 * 
 * @param client HTTP client
 */
void auth_logout(http_client_t *client);

#endif // AUTH_H
