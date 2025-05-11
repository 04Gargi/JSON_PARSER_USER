#include "triallib.h"

// int main() {
//     const char* json_string = "{\"Z\": \"F072100\",\"Z1\": \"F072100\"}*";
//     value *root = parse_json(json_string);
//     int result_len = sizeof(root)/ sizeof(*root);
//     printf("Key=%s, Value=%s\n", root.key, root.as_union.string);
//     free(root.key);
//     free(root.as_union.string);
//     return 0;
// }

int main() {
    const char* json_string = "{\"Z1\": \"F072100\",\"Z2\": 100 ,\"Z3\": \"F072100\"}*";
    int count = 0;

    value *root = malloc(sizeof(value));

    parse_json(json_string, &root, &count);
    userfuncn(root,count,"Z3");

    free(root);
    return 0;
}
