# isvhealth - IBM FlashSystem Health Monitoring CLI Tool

A command-line tool to collect health status and performance metrics from IBM FlashSystem storage arrays via the REST API.

## Features

- **Health Status Collection**: Nodes, disks, pools, FCMs (Flash Core Modules), volumes, hosts, FC ports, and alerts
- **Performance Metrics**: System-wide performance statistics
- **Multiple Output Formats**:
  - Plain text (human-readable)
  - JSON (machine-readable for monitoring systems)
  - Restit-compatible format (for integration with restit monitoring tool)
- **Token-based Authentication**: Secure authentication with the FlashSystem
- **Minimal Dependencies**: Uses libcurl and libcjson
- **All output modes provide the same complete information**

## Version

v1.1.0

## Installation

### Prerequisites

- libcurl development package
- libcjson development package

**Package installation by distribution:**

| Distribution | Command |
|--------------|---------|
| Debian/Ubuntu | `sudo apt install libcurl4-openssl-dev libjson-c-dev` |
| RHEL/CentOS/Fedora | `sudo dnf install libcurl-devel json-c-devel` |
| openSUSE | `sudo zypper install libcurl-devel json-c-devel` |

### Build

```bash
cd /root/isvhealth
make
```

### Install

```bash
cp isvhealth /usr/local/bin/
```

## Usage

```bash
isvhealth [OPTIONS] <command>
```

### Commands

- `health` - Collect health status (default)
- `all` - Collect both health and performance
- `perf` - Collect performance metrics

### Output Formats

- `--json` - Output in JSON format
- `--restit` - Output in restit-compatible format (requires `health` command)

### Options

| Option | Description |
|--------|-------------|
| `-h, --host <ip>` | FlashSystem IP address or hostname (required) |
| `-u, --username <user>` | Username for authentication |
| `-p, --password <pass>` | Password for authentication |
| `-t, --token <token>` | Use existing authentication token |
| `-P, --port <port>` | REST API port (default: 7443) |
| `-j, --json` | Output in JSON format |
| `-q, --quiet` | Only show critical errors |
| `-v, --verbose` | Verbose output (-vv for debug) |
| `-V, --version` | Show version |
| `-?, --help` | Show help |

### Examples

```bash
# Get health status in plain text
isvhealth --host 192.168.1.100 --username admin health

# Get health status in JSON format
isvhealth -h 192.168.1.100 -u admin -p secret --json health

# Get combined health and performance
isvhealth -h 192.168.1.100 health all

# Output in restit format for monitoring
isvhealth -h 192.168.1.100 --restit > health.sensors
```

## Health Status Values

The restit output format uses numeric values for health status:
- `0` = OK
- `1` = WARNING
- `2` = CRITICAL

### Host Health Monitoring

Hosts are monitored with the following status:
- `0` = OK (host online)
- `2` = CRITICAL (host offline)
- Optional message indicates the issue

### FC Port Health Monitoring

Fibre Channel ports are monitored with the following status:
- `0` = OK (port active)
- `1` = WARNING (port inactive)
- `2` = CRITICAL (port offline/failed)
- Optional message shows port speed and node information

## Output Fields

### Nodes
- id, name, status, role, type

### Disks
- id, status, type, capacity (in GB), fail_risk

### Pools
- id, name, status, capacity_used, capacity_total

### Volumes
- id, name, status, type, capacity, used

### Hosts
- id, name, status, type
- Optional message indicates path issues

### FC Ports
- id, node, status, speed, wwpn

### FCMs (Flash Core Modules)
- id, name, capacity, total_drives, compressed

### System Status
- system_status: overall system health (OK/WARNING/CRITICAL)

## Monitoring Integration

### Restit Integration

The tool can output data in restit-compatible format:

```bash
isvhealth --host 192.168.1.100 --username monitor health --restit
```

Output format: `VALUE SENSORNAME OPTIONAL_MESSAGE`

### JSON Integration

The JSON output is suitable for integration with monitoring systems like:
- Prometheus (via node_exporter textfile collector)
- Zabbix
- Nagios
- Telegraf

## Notes

- The FlashSystem REST API uses POST for all endpoints
- Token expires after 1 hour, the tool handles re-authentication
- Proxy servers may need to be bypassed using `no_proxy` environment variable
- All three output modes (text, JSON, restit) provide the same information

