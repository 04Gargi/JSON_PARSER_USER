#include "json_parser.h"
int n =0;
int i =0;
int datatype[10] ;
#define json_limit 256
#define size 50

static inline int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static inline int is_number(char c) {
    return c >= '0' && c <= '9';
}

void append_token(token *tokens, int *token_count, token_type type, const char *val) {
    if (*token_count >= size) {
        printf("Error: Exceeded maximum token count\r\n");
        exit(1);
    }

    tokens[*token_count].type = type;
    tokens[*token_count].value = (char*)malloc(strlen(val) + 1);

    // tokens[*token_count].value = (char*)malloc(strlen(val));

    strcpy(tokens[*token_count].value, val);
    // printf("token type: %d and token value: %s\n", tokens[*token_count].type, tokens[*token_count].value);

    if (tokens[*token_count].value == NULL) {
        printf("Error: Memory allocation failed for token value\r\n");
        exit(1);
    }


    if (type == TOKEN_COLON) {
        n++; 
    }
    if(type == TOKEN_STRING){
        datatype[i++] = 0;
    }else if(type == TOKEN_NUMBER){
        datatype[i++] = 1;
    }else if(type == TOKEN_NULL){
        datatype[i++] = 2;
    }
    (*token_count)++;
}

token* lexer(const char *input, int *token_count) {
    int pos = 0;
    *token_count = 0;
    static token tokens[size];

    if (strlen(input) > json_limit) {
        printf("Error: Input JSON exceeds maximum allowed size\r\n");
        exit(1);
    }

    while (input[pos] != '*' && input[pos] != '\0') {
        char ch = input[pos];

        if (ch == '{') {
            append_token(tokens, token_count, TOKEN_LEFT_BRACE, "{");
            pos++;
        } else if (ch == '}') {
            append_token(tokens, token_count, TOKEN_RIGHT_BRACE, "}");
            pos++;
        } else if (ch == '[') {
            append_token(tokens, token_count, TOKEN_LEFT_BRACKET, "[");
            pos++;
        } else if (ch == ']') {
            append_token(tokens, token_count, TOKEN_RIGHT_BRACKET, "]");
            pos++;
        } else if (ch == ':') {
            append_token(tokens, token_count, TOKEN_COLON, ":");
            pos++;
        } else if (ch == ',') {
            append_token(tokens, token_count, TOKEN_COMMA, ",");
            pos++;
        } else if (ch == '"') {
            int start = ++pos;
            while (input[pos] != '"' && input[pos] != '*' && input[pos] != '\0') {
                pos++;
            }
            int len = pos - start;
            char sval[len + 1];
            strncpy(sval, &input[start], len);
            sval[len] = '\0';
            append_token(tokens, token_count, TOKEN_STRING, sval);
            pos++;
        } else if (is_whitespace(ch)) {
            pos++;
        } else if (is_number(ch) || ch == '-') {
            // printf("inside number case\n");

            int start = pos;
            while (is_number(input[pos]) || input[pos] == '.') {
                pos++;
            }
            int len = pos - start;
            char numval[len + 1];
            strncpy(numval, &input[start], len);
            numval[len] = '\0';
            // printf("extracted number is : %s\n", numval);
            char num[20];
            append_token(tokens, token_count, TOKEN_NUMBER, numval);
        } else if (strncmp(&input[pos], "null", 4) == 0) {
            append_token(tokens, token_count, TOKEN_NULL, "null");
            pos += 4;
        } else {
            printf("Unknown character: %c\r\n", ch);
            exit(1);
        }
    }

    // for(int i = 0; i < 50; i++){
    //     printf("token type: %d     token val: %s \n", tokens[i].type, tokens[i].value);
    // }
    return tokens;
}

value parse_value(token tok) {
    value val = { 0 };
    switch (tok.type) {
    case TOKEN_STRING:
        val.type = VALUE_STRING;
        val.as_union.string = strdup(tok.value);
        break;
    case TOKEN_NUMBER:
        val.type = VALUE_INT;
        val.as_union.integer = atoi(tok.value);
        break;
    case TOKEN_NULL:
        val.type = VALUE_NULL;
        break;
    default:
        printf("Error: Unexpected token type %d\r\n", tok.type);
        exit(1);
    }
    return val;
}

// void parse_object(value obj [], token *tokens, int *token_index) {
void parse_object(value * obj, token *tokens, int *token_index) {
    if (tokens[*token_index].type != TOKEN_LEFT_BRACE) {
        printf("Syntax error: Expected opening brace\r\n");
        exit(1);
    }
    (*token_index)++;

    while (tokens[*token_index].type != TOKEN_RIGHT_BRACE) {
        for(int i = 0;i<n;i++){
        token key_token = tokens[*token_index];
        if (key_token.type != TOKEN_STRING) {
            printf("Syntax error: Expected string key\r\n");
            exit(1);
        }
        obj[i].key = strdup(key_token.value);
        
        (*token_index)++;

        if (tokens[*token_index].type != TOKEN_COLON) {
            printf("Syntax error: Expected colon\r\n");
            exit(1);
        }
        (*token_index)++;

        token value_token = tokens[*token_index];
        value parsed_value = parse_value(value_token);
        obj[i].type = parsed_value.type;

        if (parsed_value.type == VALUE_STRING) {
            // obj->as_union.string = strdup(parsed_value.as_union.string);
            obj[i].as_union.string = strdup(parsed_value.as_union.string);
            free(parsed_value.as_union.string);
        } else if (parsed_value.type == VALUE_INT) {
            // printf("\ninside parser for int\n");
            // obj->as_union.integer = parsed_value.as_union.integer;
            // printf("parsed int val: %d\n", parsed_value.as_union.integer);
            obj[i].as_union.integer = parsed_value.as_union.integer;
        } else {
            // obj->as_union.null = 0;
            obj[i].as_union.null = 0;
        }
        (*token_index)++;

        if (tokens[*token_index].type == TOKEN_COMMA) {
            (*token_index)++;
        } else if (tokens[*token_index].type != TOKEN_RIGHT_BRACE) {
            printf("Syntax error: Expected comma or closing brace\r\n");
            exit(1);
        }

        // obj++;
    }
}
    (*token_index)++;
}

// value * parse_json(const char *json_string) {
//     int token_count = 0;
//     token *tokens = lexer(json_string, &token_count);

//     int token_index = 0;
//     value result [n] ;
//     parse_object(result, tokens, &token_index);

//     for (int i = 0; i < token_count; i++) {
//         free(tokens[i].value);
//     }

//     return result;
// }

void parse_json(const char *json_string, value ** result,int *out_count) {
    int token_count = 0;
    token *tokens = lexer(json_string, &token_count);
    
    // int sizeofarr = sizeof(tokens)/token_count;

    // for(int i = 0;i < sizeofarr; i++){
    //     printf("%s\n", tokens[i]);
    // }



    int token_index = 0;
    *out_count = n;

    *result = realloc(*result,n*sizeof(value));

    parse_object(*result, tokens, &token_index);
    // for (int j = 0; j < *out_count; j++) {
    //     if (datatype[j] == 0) {
    //         printf("Key=%s, Value=%s\n", (*result)[j].key, (*result)[j].as_union.string);
    //     } else if (datatype[j] == 1) {
    //         printf("Key=%s, Value=%d\n", (*result)[j].key, (*result)[j].as_union.integer);
    //     } else if (datatype[j] == 2) {
    //         printf("Key=%s, Value=null\n", (*result)[j].key);
    //     }
    // }

    for (int i = 0; i < token_count; i++) {
        free(tokens[i].value);
    }

    *out_count = n; 
}

void userfuncn(value *result, int cnt, const char* key) {
    bool found = false;
    for (int i = 0; i < cnt; i++) {
        if (strcmp(result[i].key, key) == 0) {
            found = true;
            if (datatype[i] == 0) {
                printf("Key=%s, Value=%s\n", result[i].key, result[i].as_union.string);
            } else if (datatype[i] == 1) {
                printf("Key=%s, Value=%d\n", result[i].key, result[i].as_union.integer);
            } else if (datatype[i] == 2) {
                printf("Key=%s, Value=null\n", result[i].key);
            }
            break;
        }
    }
    if (!found) {
        printf("Key \"%s\" not found.\n", key);
    }
}

int *dt(){
    return datatype;
}

