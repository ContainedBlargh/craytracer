#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H
/**
 * @file parser.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module that parses scenes from scene json-files
 * @version 0.1
 * @date 2023-08-27
 *
 * @copyright WingCorp (c) 2023
 * 
 */

#include "scene.h"

Scene *parse_scene(char *path);

#endif