#include "rabbitmq-c/amqp.h"
#include "rabbitmq-c/tcp_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include "jansson.h"
#include "omp_q.h"

#define HOSTNAME "localhost"
#define PORT 5672
#define REQUESTQUEUE "qd_qrequest_reception_queue_137"
#define RESULTQUEUE "qlm_test_dest"
#define QPU "Test Q5"
#define RESPONSESIZE 150

void die_on_error(int x, char const *context)
{
    if (x < 0)
    {
        fprintf(stderr, "%s: %s\n", context, amqp_error_string2(x));
        exit(1);
    }
}

void die_on_amqp_error(amqp_rpc_reply_t x, char const *context)
{
    switch (x.reply_type)
    {
    case AMQP_RESPONSE_NORMAL:
        return;

    case AMQP_RESPONSE_NONE:
        fprintf(stderr, "%s: missing RPC reply type!\n", context);
        break;

    case AMQP_RESPONSE_LIBRARY_EXCEPTION:
        fprintf(stderr, "%s: %s\n", context, amqp_error_string2(x.library_error));
        break;

    case AMQP_RESPONSE_SERVER_EXCEPTION:
        switch (x.reply.id)
        {
        case AMQP_CONNECTION_CLOSE_METHOD:
        {
            amqp_connection_close_t *m =
                (amqp_connection_close_t *)x.reply.decoded;
            fprintf(stderr, "%s: server connection error %uh, message: %.*s\n",
                    context, m->reply_code, (int)m->reply_text.len,
                    (char *)m->reply_text.bytes);
            break;
        }
        case AMQP_CHANNEL_CLOSE_METHOD:
        {
            amqp_channel_close_t *m = (amqp_channel_close_t *)x.reply.decoded;
            fprintf(stderr, "%s: server channel error %uh, message: %.*s\n",
                    context, m->reply_code, (int)m->reply_text.len,
                    (char *)m->reply_text.bytes);
            break;
        }
        default:
            fprintf(stderr, "%s: unknown server error, method id 0x%08X\n",
                    context, x.reply.id);
            break;
        }
        break;
    }

    exit(1);
}

void connectToTheRabbitMQ(amqp_connection_state_t *Connection,
                          amqp_socket_t **Socket)
{
    *Connection = (amqp_new_connection());
    *Socket = amqp_tcp_socket_new(*Connection);
    if (!Socket)
    {
        printf("ERROR!!!!");
    }
    int status = amqp_socket_open(*Socket, HOSTNAME, PORT);
    if (status)
    {
        printf("not opening TCP socket");
    }
    amqp_login(*Connection, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest",
               "guest");
    amqp_channel_open(*Connection, 1);
    amqp_get_rpc_reply(*Connection);
}

char *createTheRequest(struct omp_q_reg *q_reg, int shots)
{

    time_t timer;
    char buffer[26];
    struct tm *tm_info;
    timer = time(NULL);
    tm_info = localtime(&timer);
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    puts(buffer);

    const char *json_template = "{\"task_idd\": %d, \"n_qbits\": %d, \"n_shots\": %d, \"circuit_file\": \"OPENQASM 2.0;\\ninclude \\\"qelib1.inc\\\";\\n%s\", \"circuit_file_type\": \"%s\", \"result_destination\": \"%s\", \"prefered_qpu\": \"%s\", \"prioirity\": %d, \"optimisation_level\": %d, \"transpiler_flag\": %s, \"result_type\": %d, \"submit_time\": \"%s\"}";

    int task_id = 666;
    int n_qbits = q_reg->num_q;
    int n_shots = shots;
    const char *circuit_file = q_reg->qasm_ops;
    const char *circuit_file_type = "qasm";
    const char *result_destination = RESULTQUEUE;
    const char *prefered_qpu = QPU;
    int priority = 0;
    int optimization_level = 0;
    const char *transpiler_flag = "true";
    int result_type = 0;
    const char *submit_time = buffer;

    // Calculate the length of the final string
    int length = snprintf(NULL, 0, json_template, task_id, n_qbits, n_shots, circuit_file, circuit_file_type,
                          result_destination, prefered_qpu, priority, optimization_level, transpiler_flag,
                          result_type, submit_time);

    // Allocate memory for the final string
    char *json_str = (char *)malloc(length + 1);

    // Format the JSON string
    snprintf(json_str, length + 1, json_template, task_id, n_qbits, n_shots, circuit_file, circuit_file_type,
             result_destination, prefered_qpu, priority, optimization_level, transpiler_flag,
             result_type, submit_time);
    printf(json_str);
    printf("\nso we didn't get here?\n");
    return json_str;
}

void publishTheString(amqp_connection_state_t *Connection, char *Message,
                      char *Queue)
{
    printf("%s", Message);
    amqp_connection_state_t LocalConnection = *Connection;
    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("text/plain");
    props.delivery_mode = 2;
    die_on_error(amqp_basic_publish(*Connection, 1, amqp_empty_bytes, amqp_cstring_bytes(Queue),
                                    0, 0, &props, amqp_cstring_bytes(Message)),
                 "Publishing");
}

char *startConsume(amqp_connection_state_t *Connection, char *Queue)
{

    amqp_bytes_t queuename;
    printf("%d\n", amqp_cstring_bytes(Queue));
    amqp_queue_declare_ok_t *r = amqp_queue_declare(*Connection, 1, amqp_cstring_bytes(Queue), 0, 0, 0, 0, amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(*Connection), "Declaring queue");
    queuename = amqp_bytes_malloc_dup(r->queue);
    if (queuename.bytes == NULL)
    {
        fprintf(stderr, "Out of memory while copying queue name");
        assert(0);
    }
    char *TheResponse = (char *) malloc(sizeof(char) * RESPONSESIZE);
    amqp_basic_consume(*Connection, 1,
                       amqp_cstring_bytes(Queue),
                       amqp_empty_bytes, 0, 1, 0, amqp_empty_table);

    amqp_rpc_reply_t rpc_reply = amqp_get_rpc_reply(*Connection);

    if (rpc_reply.reply_type != AMQP_RESPONSE_NORMAL)
    {
        fprintf(stderr, "Failed to start consuming messages.\n");
    }

    for (;;)
    {
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;

        amqp_maybe_release_buffers(*Connection);

        res = amqp_consume_message(*Connection, &envelope, NULL, 0);

        if (AMQP_RESPONSE_NORMAL != res.reply_type)
        {
            break;
        }

        printf("Delivery %u\n", (unsigned)envelope.delivery_tag);

        if (envelope.message.properties._flags & AMQP_BASIC_CONTENT_TYPE_FLAG)
        {
            printf("Content-type: %.*s\n",
                   (int)envelope.message.properties.content_type.len,
                   (char *)envelope.message.properties.content_type.bytes);
        }
        printf("----\n");
        printf("%.*s\n", (int)envelope.message.body.len,
               (char *)envelope.message.body.bytes);
        char *temp = (char *)envelope.message.body.bytes;

        strcpy(TheResponse, temp);
        amqp_destroy_envelope(&envelope);
        break;
    }
    return TheResponse;
}

void closeConnections(amqp_connection_state_t *Connection)
{

    amqp_channel_close(*Connection, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(*Connection, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(*Connection);
}

void parseTheResponse(char *response)
{
    json_error_t error;
    json_t *root = json_loads(response, 0, &error);
    json_t *task_id = json_object_get(root, "task_id");
    json_t *results = json_object_get(root, "results");
    json_t *destination = json_object_get(root, "destination");
    json_t *execution_status = json_object_get(root, "execution_status");
    json_t *additional_information =
        json_object_get(root, "additional_information");
    json_t *execution_time = json_object_get(root, "execution_time");
    int value_task_id = json_integer_value(task_id);
    const char *value_result_id = json_string_value(results);
    const char *destination_str = json_string_value(destination);
    int execution = json_boolean_value(execution_status);
    const char *additional_information_str =
        json_string_value(additional_information);
    int execution_time_d = json_integer_value(execution_time);

    printf("Name: %d\n", value_task_id);
    printf("result: %s\n", value_result_id);
    printf("destination: %s\n", destination_str);
    printf("execution: %d\n", execution);
    printf("additional_information : %s\n", additional_information_str);
    printf("execution_time_d : %d\n", execution_time_d);
}


void lrz_measure(struct omp_q_reg *q_reg, int shots) //i.e. execute
{

    amqp_connection_state_t SendConnection, ListenConnection;
    amqp_socket_t *SendSocket = NULL, *Listensocket = NULL;


    printf("%s\n", q_reg->qasm_ops);

    char *TheRequest = createTheRequest(q_reg, shots);
    char *TheResponse = (char *) malloc(1000 * sizeof(char));
    //= createTheResponse(circuit, shots);
    connectToTheRabbitMQ(&SendConnection, &SendSocket);

    connectToTheRabbitMQ(&ListenConnection, &Listensocket);

    publishTheString(&SendConnection, TheRequest, REQUESTQUEUE);
    // publishTheString(&ListenConnection, TheResponse, RESULTQUEUE);
    TheResponse = startConsume(&ListenConnection, RESULTQUEUE);
    closeConnections(&ListenConnection);
    closeConnections(&SendConnection);
    parseTheResponse(TheResponse);
}
