#include "network/http.h"
#include "fmt/fmt.h"
#include "json/json.h"
#include "string/std_string.h"

typedef struct {
    int id;
    char* name;
    int age;
} User;

static User users[10];
static size_t user_count = 0;

User* find_user_by_id(int id) {
    for (size_t i = 0; i < user_count; i++) {
        if (users[i].id == id) {
            return &users[i];
        }
    }
    return NULL;
}

int extract_id_from_path(const char* path) {
    const char* last_slash = strrchr(path, '/');
    if (last_slash) {
        return atoi(last_slash + 1);
    }
    return -1;
}

void handle_get_users(HttpRequest* req, HttpResponse* res) {
    (void)req;
    JsonElement* users_array = json_create(JSON_ARRAY);
    for (size_t i = 0; i < user_count; i++) {
        JsonElement* user_obj = json_create(JSON_OBJECT);

        JsonElement* id_element = json_create(JSON_NUMBER);
        id_element->value.number_val = users[i].id;
        json_set_element(user_obj, "id", id_element);

        JsonElement* name_element = json_create(JSON_STRING);
        name_element->value.string_val = string_strdup(users[i].name);
        json_set_element(user_obj, "name", name_element);

        JsonElement* age_element = json_create(JSON_NUMBER);
        age_element->value.number_val = users[i].age;
        json_set_element(user_obj, "age", age_element);

        json_add_to_array(users_array, user_obj);
    }

    JsonElement* response_json = json_create(JSON_OBJECT);
    json_set_element(response_json, "users", users_array);

    http_set_status(res, 200, "OK");
    http_set_json_body(res, response_json);
}

void handle_add_user(HttpRequest* req, HttpResponse* res) {
    if (req->json_body) {
        JsonElement* id_element = json_get_element(req->json_body, "id");
        JsonElement* name_element = json_get_element(req->json_body, "name");
        JsonElement* age_element = json_get_element(req->json_body, "age");

        if (id_element && id_element->type == JSON_NUMBER &&
            name_element && name_element->type == JSON_STRING &&
            age_element && age_element->type == JSON_NUMBER &&
            user_count < 10) {

            int id = (int)id_element->value.number_val;
            if (find_user_by_id(id) == NULL) {
                User new_user;
                new_user.id = id;
                new_user.name = string_strdup(name_element->value.string_val);
                new_user.age = (int)age_element->value.number_val;

                users[user_count++] = new_user;

                http_set_status(res, 201, "User Created");

                JsonElement* response_json = json_create(JSON_OBJECT);
                JsonElement* message_element = json_create(JSON_STRING);

                message_element->value.string_val = string_strdup("User added successfully");

                json_set_element(response_json, "message", message_element);
                http_set_json_body(res, response_json);
            } 
            else {
                http_send_error(res, 400, "User already exists with this ID");
            }
        } 
        else {
            http_send_error(res, 400, "Bad Request: Invalid user data or user limit reached");
        }
    } 
    else {
        http_send_error(res, 400, "Bad Request: Expected JSON body");
    }
}

void handle_update_user(HttpRequest* req, HttpResponse* res) {
    int id = extract_id_from_path(req->path);

    if (id != -1 && req->json_body) {
        User* user = find_user_by_id(id);
        
        if (user) {
            JsonElement* name_element = json_get_element(req->json_body, "name");
            JsonElement* age_element = json_get_element(req->json_body, "age");

            if (name_element && name_element->type == JSON_STRING &&
                age_element && age_element->type == JSON_NUMBER) {

                user->name = string_strdup(name_element->value.string_val);
                if (user->name == NULL) {
                    http_send_error(res, 500, "Internal Server Error: Memory allocation failed");
                    return;
                }
                user->age = (int)age_element->value.number_val;
                http_set_status(res, 200, "User Updated");

                JsonElement* response_json = json_create(JSON_OBJECT);
                JsonElement* message_element = json_create(JSON_STRING);

                message_element->value.string_val = string_strdup("User updated successfully");

                json_set_element(response_json, "message", message_element);
                http_set_json_body(res, response_json);
            } 
            else {
                http_send_error(res, 400, "Bad Request: Invalid user data");
            }
        } 
        else {
            http_send_error(res, 404, "User Not Found");
        }
    } 
    else {
        http_send_error(res, 400, "Bad Request: Missing user ID or JSON body");
    }
}

void handle_delete_user(HttpRequest* req, HttpResponse* res) {
    int id = extract_id_from_path(req->path);
    if (id != -1) {
        for (size_t i = 0; i < user_count; i++) {
            if (users[i].id == id) {
                free(users[i].name);
                for (size_t j = i; j < user_count - 1; j++) {
                    users[j] = users[j + 1];
                }
                user_count--;

                http_set_status(res, 200, "User Deleted");

                JsonElement* response_json = json_create(JSON_OBJECT);
                JsonElement* message_element = json_create(JSON_STRING);

                message_element->value.string_val = string_strdup("User deleted successfully");
                json_set_element(response_json, "message", message_element);
                http_set_json_body(res, response_json);

                return;
            }
        }
        http_send_error(res, 404, "User Not Found");
    } 
    else {
        http_send_error(res, 400, "Bad Request: Missing user ID");
    }
}

int main() {
    http_register_route("/users", HTTP_GET, handle_get_users);
    http_register_route("/users", HTTP_POST, handle_add_user);
    http_register_route("/users/{id}", HTTP_PUT, handle_update_user);
    http_register_route("/users/{id}", HTTP_DELETE, handle_delete_user);

    fmt_printf("Starting HTTP server on port 8051...\n");
    http_start_server(8051);

    return 0;
}
