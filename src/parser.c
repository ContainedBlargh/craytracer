#include "parser.h"
#include "fail.h"
#include "hashmap.h"
#include "json.h"
#include "util.h"
#include <math.h>
#include <string.h>

static const char *explain_error(u64 error)
{
    switch (error)
    {
    case json_parse_error_none:
        return "No error";
    case json_parse_error_expected_comma_or_closing_bracket:
        return "Expected comma or closing bracket";
    case json_parse_error_expected_colon:
        return "Expected colon between name and value";
    case json_parse_error_expected_opening_quote:
        return "Expected opening quote for string";
    case json_parse_error_invalid_string_escape_sequence:
        return "Invalid escape sequence in string";
    case json_parse_error_invalid_number_format:
        return "Invalid number format";
    case json_parse_error_invalid_value:
        return "Invalid value";
    case json_parse_error_premature_end_of_buffer:
        return "Premature end of buffer before object/array was complete";
    case json_parse_error_invalid_string:
        return "Malformed string";
    case json_parse_error_allocator_failed:
        return "Memory allocation failed";
    case json_parse_error_unexpected_trailing_characters:
        return "Unexpected trailing characters";
    case json_parse_error_unknown:
        return "Unknown error";
    default:
        return "Unrecognized error code";
    }
}

Vec3 parse_vec3(struct json_object_element_s *current)
{
    Vec3 output = vec3(0, 0, 0);
    struct json_object_s *vector_obj = json_value_as_object(current->value);
    for (u32 j = 0; j < vector_obj->length; j++)
    {
        if (strncasecmp("x", current->name->string, current->name->string_size) == 0)
        {
            struct json_number_s *x = json_value_as_number(current->value);
            if (x == NULL)
            {
                failwithf("Parser error, expected to be a number, but got the type '%llu'\n", current->value->type);
            }
            output.x = atof(x->number);
        }
        else if (strncasecmp("y", current->name->string, current->name->string_size) == 0)
        {
            struct json_number_s *y = json_value_as_number(current->value);
            if (y == NULL)
            {
                failwithf("Parser error, expected to be a number, but got the type '%llu'\n", current->value->type);
            }
            output.y = atof(y->number);
        }
        else if (strncasecmp("z", current->name->string, current->name->string_size) == 0)
        {
            struct json_number_s *z = json_value_as_number(current->value);
            if (z == NULL)
            {
                failwithf("Parser error, expected to be a number, but got the type '%llu'\n", current->value->type);
            }
            output.z = atof(z->number);
        }
    }
    return output;
}

Camera *parse_camera(struct json_object_element_s *current)
{
    Vec3 position = vec3(0, 0, 0);
    Vec3 direction = vec3(0, 0, 0);
    struct json_object_s *camera_obj = json_value_as_object(current->value);
    struct json_object_element_s *current_inner = camera_obj->start;
    for (u32 j = 0; j < camera_obj->length; j++)
    {
        if (strncasecmp("position", current_inner->name->string, current_inner->name->string_size) == 0)
        {
            position = parse_vec3(current_inner);
        }
        if (strncasecmp("direction", current_inner->name->string, current_inner->name->string_size) == 0)
        {
            direction = parse_vec3(current_inner);
        }
        current_inner = current_inner->next;
    }
    Camera *camera = new_camera(position, direction);
    return camera;
}

Scene *parse_scene(char *path)
{
    Scene *scene = new_scene();

    char *contents = read_file(path);
    struct json_parse_result_s result;
    struct json_value_s *root = json_parse_ex(
        contents, strlen(contents), json_parse_flags_allow_multi_line_strings | json_parse_flags_allow_trailing_comma,
        NULL, NULL, &result);
    if (root == NULL)
    {
        failwithf("Failed to parse json: %s on line [%zu]\n", explain_error(result.error), result.error_line_no);
    }
    struct json_object_s *obj = json_value_as_object(root);
    if (obj == NULL)
    {
        failwith("Failed to convert root into an object!\n");
    }
    struct json_object_element_s *current = obj->start;
    for (u32 i = 0; i < obj->length; i++)
    {
        const char *label = current->name->string;
        u64 label_size = current->name->string_size;
        if (strncasecmp("camera", label, label_size) == 0)
        {
            parse_camera(current);
        } else if (strncasecmp("lights", label, label_size) == 0) {
            
        }
        current = current->next;
    }
    free(root);
    free(contents);
    return scene;
}