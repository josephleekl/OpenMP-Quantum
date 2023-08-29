#ifndef SIMULATE_LRZ_H
#define SIMULATE_LRZ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rabbitmq-c/amqp.h"
#include "rabbitmq-c/tcp_socket.h"
#include "jansson.h"
#include "omp_q.h"

#define HOSTNAME "localhost"
#define PORT 5672
#define REQUESTQUEUE "qd_qrequest_reception_queue_137"
#define RESULTQUEUE "qlm_test_dest"
#define QPU "Test Q5"
//#define QPU "QLM"
#define RESPONSESIZE 150

struct omp_q_reg;
void lrz_measure(struct omp_q_reg *q_reg, int shots, double meas_probabilities[]) ;

#endif // SIMULATE_LRZ_H
