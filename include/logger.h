#ifndef LOGGER_H
#define LOGGER_H

#include "packet_parser.h"

// Initialize log files
int init_logger(const char *log_dir);

// Log internet layer packet
void log_internet_layer(const InternetLayerInfo *info);

// Log transport layer packet
void log_transport_layer(const TransportLayerInfo *info);

// Log application layer packet
void log_application_layer(const ApplicationLayerInfo *info);

// Close log files
void close_logger();

#endif // LOGGER_H
