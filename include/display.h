#ifndef DISPLAY_H
#define DISPLAY_H

#include "statistics.h"

// Initialize display (terminal setup)
void init_display();

// Update display with current statistics
void update_display();

// Display protocol counters
void display_protocol_counters(const ProtocolCounters *counters);

// Display client statistics
void display_client_statistics();

// Cleanup display
void cleanup_display();

#endif // DISPLAY_H
