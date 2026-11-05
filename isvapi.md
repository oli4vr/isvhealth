# IBM FlashSystem REST API Reference

This document provides a quick reference for the IBM Spectrum Virtualize REST API used by the `isvhealth` tool.

## Base Information

- **Default Port**: 7443
- **Protocol**: HTTPS
- **API Path**: `/rest/v1/`
- **Authentication**: Token-based via `/rest/v1/auth`
- **Docs**: 
  - [Storage Virtualize RESTful API](https://www.ibm.com/docs/en/flashsystem-5x00/9.1.2?topic=interface-storage-virtualize-restful-api)
  - [Storage Virtualize REST API Reference](https://www.ibm.com/docs/en/flashsystem-5x00/9.1.2?topic=api-storage-virtualize-rest)

## Authentication

### Get Authentication Token

**Endpoint**: `POST /rest/v1/auth`

**Headers**:
```
X-Auth-Username: <username>
X-Auth-Password: <password>
```

**Response**:
```json
{
  "token": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzUxMiJ9..."
}
```

**Notes**:
- Token must be sent as `X-Auth-Token: <token>` in subsequent requests
- Tokens have expiration times (typically 1 hour)
- The Monitor user should have read-only permissions

## Health Status Endpoints

**Important**: IBM FlashSystem API uses `POST` for most endpoints (not `GET`).

| Endpoint | Description | Key Fields | Status |
|----------|-------------|------------|--------|
| `POST /rest/v1/lsnode` | Node/Controller status | name, status, id, hardware | Working |
| `POST /rest/v1/lsdrive` | Drive status | id, status, capacity, tech_type | Working |
| `POST /rest/v1/lsmdisk` | Managed disk status | id, name, status, mode | Check system |
| `POST /rest/v1/lspool` | Storage pool status | id, name, status, capacity | May not exist |
| `POST /rest/v1/lsvdisk` | Volume status | id, name, status, capacity | Check system |
| `POST /rest/v1/lsioport` | I/O port status | id, node_id, type, status | May not exist |
| `POST /rest/v1/lsfcport` | FC port status | id, node_id, status, speed | Check system |
| `POST /rest/v1/lshost` | Host status | id, name, status, type | Check system |
| `POST /rest/v1/lsenclosure` | Enclosure status | id, status, type | Check system |
| `POST /rest/v1/lslicense` | System info | id, name, code_level | Working |
| `POST /rest/v1/lseventlog` | Alerts/Events | id, severity, status, message | Check system |

## Performance Metrics

IBM Spectrum Virtualize uses statistics files for performance data. The API provides access to various statistics.

### Statistics Endpoints

| Endpoint | Description | Status |
|----------|-------------|--------|
| `POST /rest/v1/lssystemstats` | System-level statistics | Check system |
| `POST /rest/v1/lsnodestats` | Node/Controller statistics | Check system |
| `POST /rest/v1/lsdriveclassstats` | Drive class statistics | Check system |
| `POST /rest/v1/lsmdiskgrpstats` | Storage pool stats | Check system |
| `POST /rest/v1/lsvdiskstats` | Volume statistics | Check system |
| `POST /rest/v1/lsportstats` | Port statistics | Check system |
| `POST /rest/v1/lshoststats` | Host statistics | Check system |

### Common Performance Metrics

- **IOPS**: read_io_rate, write_io_rate, total_io_rate
- **Throughput**: read_mb_rate, write_mb_rate
- **Latency**: read_latency, write_latency
- **Cache**: cache_read_hits, cache_write_hits
- **CPU/Memory**: cpu_pc, cpu_busy_time, memory_used_pc

## Response Format

Most endpoints return JSON arrays or objects:

```json
[
  {
    "id": "0",
    "name": "node0",
    "status": "online",
    ...
  }
]
```

Or for single items:
```json
{
  "id": "0",
  "name": "node0",
  "status": "online"
}
```

## Error Responses

```json
{
  "error": [
    {
      "code": "CSMGK0005E",
      "message": "Command 'lspool' does not exist"
    }
  ]
}
```

Common HTTP status codes:
- `200` - Success
- `401` - Unauthorized (invalid/expired token)
- `403` - Forbidden (insufficient permissions)
- `404` - Command not found (endpoint doesn't exist on this system)

## API Quirks

1. **POST instead of GET**: Most endpoints use POST method, not GET
2. **No body required**: POST requests typically don't need a JSON body (use NULL)
3. **Token authentication**: Use X-Auth-Token header for authenticated requests
4. **Version**: Check your FlashSystem version as API may differ between versions
5. **Monitoring user**: Recommended to use a dedicated read-only monitoring user

## Tool Usage

The `isvhealth` tool uses these endpoints:
- Auth: `POST /rest/v1/auth`
- Health: Various `ls*` endpoints as listed above
- System info: `POST /rest/v1/lslicense`
- Performance: Various stats endpoints (not yet fully implemented)
