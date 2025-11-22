#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static FILE *internet_log = NULL;
static FILE *transport_log = NULL;
static FILE *application_log = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

int init_logger(const char *log_dir) {
    char filepath[512];

    // Create internet layer log
    snprintf(filepath, sizeof(filepath), "%s/camada_internet.csv", log_dir);
    internet_log = fopen(filepath, "w");
    if (!internet_log) {
        perror("Error creating internet layer log");
        return -1;
    }
    fprintf(internet_log, "Data/Hora,Protocolo,IP Origem,IP Destino,Protocolo Encapsulado,Info Extra,Tamanho(bytes)\n");
    fflush(internet_log);

    // Create transport layer log
    snprintf(filepath, sizeof(filepath), "%s/camada_transporte.csv", log_dir);
    transport_log = fopen(filepath, "w");
    if (!transport_log) {
        perror("Error creating transport layer log");
        fclose(internet_log);
        return -1;
    }
    fprintf(transport_log, "Data/Hora,Protocolo,IP Origem,Porta Origem,IP Destino,Porta Destino,Tamanho(bytes)\n");
    fflush(transport_log);

    // Create application layer log
    snprintf(filepath, sizeof(filepath), "%s/camada_aplicacao.csv", log_dir);
    application_log = fopen(filepath, "w");
    if (!application_log) {
        perror("Error creating application layer log");
        fclose(internet_log);
        fclose(transport_log);
        return -1;
    }
    fprintf(application_log, "Data/Hora,Protocolo,Informacoes\n");
    fflush(application_log);

    printf("Log files initialized in %s/\n", log_dir);
    return 0;
}

void log_internet_layer(const InternetLayerInfo *info) {
    if (!internet_log || !info->protocol[0]) {
        return;
    }

    pthread_mutex_lock(&log_mutex);
    fprintf(internet_log, "%s,%s,%s,%s,%d,%s,%u\n",
            info->timestamp,
            info->protocol,
            info->src_ip,
            info->dest_ip,
            info->encapsulated_proto,
            info->extra_info,
            info->packet_size);
    fflush(internet_log);
    pthread_mutex_unlock(&log_mutex);
}

void log_transport_layer(const TransportLayerInfo *info) {
    if (!transport_log || !info->protocol[0]) {
        return;
    }

    pthread_mutex_lock(&log_mutex);
    fprintf(transport_log, "%s,%s,%s,%u,%s,%u,%u\n",
            info->timestamp,
            info->protocol,
            info->src_ip,
            info->src_port,
            info->dest_ip,
            info->dest_port,
            info->packet_size);
    fflush(transport_log);
    pthread_mutex_unlock(&log_mutex);
}

void log_application_layer(const ApplicationLayerInfo *info) {
    if (!application_log || !info->protocol[0]) {
        return;
    }

    pthread_mutex_lock(&log_mutex);
    fprintf(application_log, "%s,%s,%s\n",
            info->timestamp,
            info->protocol,
            info->info);
    fflush(application_log);
    pthread_mutex_unlock(&log_mutex);
}

void close_logger() {
    pthread_mutex_lock(&log_mutex);
    
    if (internet_log) {
        fclose(internet_log);
        internet_log = NULL;
    }
    
    if (transport_log) {
        fclose(transport_log);
        transport_log = NULL;
    }
    
    if (application_log) {
        fclose(application_log);
        application_log = NULL;
    }
    
    pthread_mutex_unlock(&log_mutex);
    printf("Log files closed\n");
}
