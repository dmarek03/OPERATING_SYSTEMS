#ifndef ZAD1_COMMON_H
#define ZAD1_COMMON_H

#define MAX_CLIENTS 40
#define MAX_CLIENT_ID_LEN 64
#define MAX_MESSAGE_STRING_LEN 128

typedef enum{
    LIST,
    TOALL,
    TOONE,
    STOP,
    ALIVE

}request_type_t;

typedef struct {
    request_type_t request_type;
    char sender_client_id[MAX_CLIENT_ID_LEN];

    union {
        struct {
            char target_client_id[MAX_CLIENT_ID_LEN];
            char message[MAX_MESSAGE_STRING_LEN];

        }to_one;

        char to_all[MAX_MESSAGE_STRING_LEN];

        struct {
            char identifiers_list[MAX_CLIENTS][MAX_CLIENT_ID_LEN];
            int list_lengths;
        }list;
    }payload;
}requested_message_t;



#endif //ZAD1_COMMON_H
