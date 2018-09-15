#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <collada/int_array.h>
#include <collada/context.h>
#include <xml/xml.h>

void __fill_float(const unsigned len, float *p, const char *buf)
{
        const char *ptr = buf;
        char * endptr;
        unsigned  i = 0;
next:
        if(!ptr || !*ptr || i >= len) goto end;

        switch (*ptr) {
                case '1': case '2': case '3':
                case '4': case '5': case '6':
                case '7': case '8': case '9':
                case '0': case '+': case '-':
                        p[i] = (float)strtod(ptr, &endptr);
                        i++;
                        ptr = endptr;
                        goto next;
                default:
                        ptr++;
                        goto next;
        }

end:
        ;
}

static void __parse_geometry(struct dae_context *p,
        struct xml_node *xn_geometry)
{
        struct dae_geometry *geometry = dae_geometry_new();
        struct xml_attribute    *attr_id, *attr_count,
                                *attr_semantic, *attr_offset;
        struct xml_node *xn_float_array, *xn_p,
                        *xn_triangles, *xn_input;
        struct array *searchs = array_new(1);
        struct float_array      *positions = float_array_new(),
                                *texcoords = float_array_new(),
                                *normals = float_array_new(),
                                *colors = float_array_new();
        struct int_array *ps = int_array_new();
        unsigned i, j, v,
                 count = 0,
                 inputs = 0;
        signed  positions_offset = 0,
                normals_offset = 0,
                texcoords_offset = 0,
                colors_offset = 0;

        attr_id = xml_node_get_attribute(xn_geometry, KEYSIZE_L("id"));
        string_cat(geometry->id,
                STRING_CAT_CHARS(attr_id->value, attr_id->value_len),
                NULL
        );
        hash_table_set(p->geometries, &geometry->base,
                geometry->id->ptr, geometry->id->len);
        ref_dec(&geometry->base);

        xml_node_search(xn_geometry, "//float_array", searchs);
        for(i = 0; i < searchs->len; ++i) {
                xn_float_array = cast(array_get(searchs, i),
                        struct xml_node, base);
                attr_count = xml_node_get_attribute(xn_float_array,
                        KEYSIZE_L("count"));
                attr_id = xml_node_get_attribute(xn_float_array,
                        KEYSIZE_L("id"));
                count = atoi(attr_count->value);
                if(strstr(attr_id->value, "positions-array")) {
                        float_array_reserve(positions, count);
                        float_array_add_string(positions,
                                xn_float_array->value);
                } else if(strstr(attr_id->value, "normals-array")) {
                        float_array_reserve(normals, count);
                        float_array_add_string(normals,
                                xn_float_array->value);
                } else if(strstr(attr_id->value, "map-0-array")) {
                        float_array_reserve(texcoords, count);
                        float_array_add_string(texcoords,
                                xn_float_array->value);
                } else if(strstr(attr_id->value, "colors-Col-array")) {
                        float_array_reserve(colors, count);
                        float_array_add_string(colors,
                                xn_float_array->value);
                }
        }

        xml_node_search(xn_geometry, "//triangles", searchs);
        xn_triangles = cast(array_get(searchs, 0), struct xml_node, base);
        attr_count = xml_node_get_attribute(xn_triangles,
                KEYSIZE_L("count"));
        count = atoi(attr_count->value);

        xml_node_search(xn_geometry, "//p", searchs);
        xn_p = cast(array_get(searchs, 0), struct xml_node, base);

        xml_node_search(xn_triangles, "/input", searchs);
        int_array_reserve(ps, count * searchs->len * 3);
        int_array_add_string(ps, xn_p->value);

        inputs = 0;
        positions_offset = normals_offset = texcoords_offset = colors_offset = -1;
        for(i = 0; i < searchs->len; ++i) {
                xn_input = cast(array_get(searchs, i), struct xml_node ,base);
                attr_semantic = xml_node_get_attribute(xn_input, KEYSIZE_L("semantic"));
                attr_offset = xml_node_get_attribute(xn_input, KEYSIZE_L("offset"));
                if(strstr(attr_semantic->value, "VERTEX")) {
                        positions_offset = atoi(attr_offset->value);
                } else if(strstr(attr_semantic->value, "NORMAL")) {
                        normals_offset = atoi(attr_offset->value);
                } else if(strstr(attr_semantic->value, "TEXCOORD")) {
                        texcoords_offset = atoi(attr_offset->value);
                } else if(strstr(attr_semantic->value, "COLOR")) {
                        colors_offset = atoi(attr_offset->value);
                }
                inputs++;
        }

        if(positions_offset >= 0) {
                geometry->vertices = float_array_new();
                float_array_reserve(geometry->vertices, ps->len / inputs * 3);

                geometry->vertices_indexes = int_array_new();
                int_array_reserve(geometry->vertices_indexes, ps->len / inputs);
        }
        if(normals_offset >= 0) {
                geometry->normals = float_array_new();
                float_array_reserve(geometry->normals, ps->len / inputs * 3);
        }
        if(texcoords_offset >= 0) {
                geometry->texcoords = float_array_new();
                float_array_reserve(geometry->texcoords, ps->len / inputs * 2);
        }
        if(colors_offset >= 0) {
                geometry->colors = float_array_new();
                float_array_reserve(geometry->colors, ps->len / inputs * 4);
        }


        for(i = 0; i < ps->len; i += inputs) {
                if(positions_offset >= 0) {
                        j = ps->ptr[i + positions_offset];
                        int_array_add_int(geometry->vertices_indexes, j);

                        j *= 3;
                        float_array_add_float(geometry->vertices,
                                positions->ptr[j]);
                        float_array_add_float(geometry->vertices,
                                positions->ptr[j + 1]);
                        float_array_add_float(geometry->vertices,
                                positions->ptr[j + 2]);
                }
                if(normals_offset >= 0) {
                        j = ps->ptr[i + normals_offset];

                        j *= 3;
                        float_array_add_float(geometry->normals,
                                normals->ptr[j]);
                        float_array_add_float(geometry->normals,
                                normals->ptr[j + 1]);
                        float_array_add_float(geometry->normals,
                                normals->ptr[j + 2]);
                }
                if(texcoords_offset >= 0) {
                        j = ps->ptr[i + texcoords_offset];

                        j *= 2;
                        float_array_add_float(geometry->texcoords,
                                texcoords->ptr[j]);
                        float_array_add_float(geometry->texcoords,
                                texcoords->ptr[j + 1]);
                }
                if(colors_offset >= 0) {
                        j = ps->ptr[i + colors_offset];

                        j *= 4;
                        float_array_add_float(geometry->colors,
                                colors->ptr[j]);
                        float_array_add_float(geometry->colors,
                                colors->ptr[j + 1]);
                        float_array_add_float(geometry->colors,
                                colors->ptr[j + 2]);
                        float_array_add_float(geometry->colors,
                                colors->ptr[j + 3]);
                }
        }


        ref_dec(&ps->base);
        ref_dec(&searchs->base);
        ref_dec(&positions->base);
        ref_dec(&texcoords->base);
        ref_dec(&normals->base);
        ref_dec(&colors->base);
}

static void __parse_library_geometries(struct dae_context *p,
        struct xml_node *xn)
{
        struct array *geometries = array_new(1);
        unsigned i;

        xml_node_search(xn, "//library_geometries/geometry", geometries);
        for(i = 0; i < geometries->len; ++i) {
                __parse_geometry(p, cast(array_get(geometries, i),
                        struct xml_node, base));
        }

        ref_dec(&geometries->base);
}

static void __parse_controller(struct dae_context *p,
        struct xml_node *xn_controller)
{
        struct dae_skin *skin = dae_skin_new();
        struct dae_geometry *geometry;
        struct array *searchs = array_new(1);
        struct float_array      *weights = float_array_new(),
                                *v_bones_id = float_array_new(),
                                *v_weights = float_array_new();
        struct int_array *vcount = int_array_new();
        struct int_array *v = int_array_new();
        struct xml_attribute    *attr_id,
                                *attr_source,
                                *attr_count;
        struct xml_node *xn_skin, *xn_bind_shape_matrix,
                        *xn_name_array, *xn_float_array,
                        *xn_vcount, *xn_v, *xn_vertex_weights;
        const char *start, *end;
        struct string *s_temp;
        unsigned i, j, count, vcount_i;
        signed vcount_max, ii;

        attr_id = xml_node_get_attribute(xn_controller, KEYSIZE_L("id"));
        string_cat(skin->id,
                STRING_CAT_CHARS(attr_id->value, attr_id->value_len),
                NULL
        );
        hash_table_set(p->skines, &skin->base,
                skin->id->ptr, skin->id->len);
        ref_dec(&skin->base);

        xml_node_search(xn_controller, "/skin", searchs);
        xn_skin = cast(array_get(searchs, 0), struct xml_node, base);
        attr_source = xml_node_get_attribute(xn_skin, KEYSIZE_L("source"));
        geometry = cast(hash_table_get(p->geometries,
                attr_source->value + 1, attr_source->value_len - 1),
                struct dae_geometry, base);
        float_array_assign(&skin->vertices, geometry->vertices);
        float_array_assign(&skin->normals, geometry->normals);
        float_array_assign(&skin->texcoords, geometry->texcoords);
        float_array_assign(&skin->colors, geometry->colors);

        xml_node_search(xn_skin, "/bind_shape_matrix", searchs);
        xn_bind_shape_matrix = cast(array_get(searchs, 0), struct xml_node, base);
        __fill_float(16, skin->transform.m, xn_bind_shape_matrix->value);
        mat4_transpose(&skin->transform);

        xml_node_search(xn_skin, "//Name_array", searchs);
        xn_name_array = cast(array_get(searchs, 0), struct xml_node, base);
        start = xn_name_array->value;
        end = start;
        while(*start) {
                if(isspace(*start)) {
                        start++;
                        end = start;
                } else {
                        switch (*end) {
                                case ' ': case '\t': case '\n':
                                case '\v': case '\f': case '\r':
                                case '\0':
                                        if(end > start) {
                                                s_temp = string_new();
                                                string_cat_chars(s_temp, start, end - start);
                                                array_push(skin->bone_names, &s_temp->base);
                                                ref_dec(&s_temp->base);
                                        }
                                        start = end;
                                        break;
                                default:
                                        end++;
                                        break;

                        }
                }
        }

        xml_node_search(xn_skin, "//float_array", searchs);
        for(i = 0; i < searchs->len; ++i) {
                xn_float_array = cast(array_get(searchs, i), struct xml_node, base);
                attr_id = xml_node_get_attribute(xn_float_array, KEYSIZE_L("id"));
                attr_count = xml_node_get_attribute(xn_float_array, KEYSIZE_L("count"));
                count = atoi(attr_count->value);
                if(strstr(attr_id->value, "bind_poses-array")) {
                        float_array_reserve(skin->inverse_bind_poses, count);
                        float_array_add_string(skin->inverse_bind_poses,
                                xn_float_array->value);
                        for(j = 0; j < count; j += 16) {
                                mat4_transpose(
                                        (union mat4 *)(skin->inverse_bind_poses->ptr + j));
                        }
                } else if(strstr(attr_id->value, "weights-array")) {
                        float_array_reserve(weights, count);
                        float_array_add_string(weights, xn_float_array->value);
                }
        }

        xml_node_search(xn_skin, "//vertex_weights", searchs);
        xn_vertex_weights = cast(array_get(searchs, 0), struct xml_node, base);
        attr_count = xml_node_get_attribute(xn_vertex_weights, KEYSIZE_L("count"));
        count = atoi(attr_count->value);

        int_array_reserve(vcount, count);
        int_array_reserve(v, count * 2);

        xml_node_search(xn_vertex_weights, "//vcount", searchs);
        xn_vcount = cast(array_get(searchs, 0), struct xml_node, base);

        xml_node_search(xn_vertex_weights, "//v", searchs);
        xn_v = cast(array_get(searchs, 0), struct xml_node, base);

        vcount_max = int_array_add_string(vcount, xn_vcount->value);
        int_array_add_string(v, xn_v->value);

        float_array_reserve(v_bones_id, count * vcount_max);
        float_array_reserve(v_weights, count * vcount_max);

        for(i = 0, ii = -1; i < vcount->len; ++i) {
                vcount_i = vcount->ptr[i];

                for(j = 0; j < vcount_i; ++j) {
                        ii++;
                        float_array_add_float(v_bones_id, v->ptr[ii]);
                        ii++;
                        float_array_add_float(v_weights, v->ptr[ii]);
                }

                while(j < vcount_max) {
                        float_array_add_float(v_bones_id, 0.0f);
                        float_array_add_float(v_weights, 0.0f);
                        j++;
                }
        }

        skin->bones_per_vertex = vcount_max;
        if(vcount_max <= 4) {
                skin->bones_upload = 4;
        } else if(vcount_max <= 8) {
                skin->bones_upload = 8;
        } else if(vcount_max <= 12) {
                skin->bones_upload = 12;
        }

        float_array_reserve(skin->bones_id, geometry->vertices_indexes->len * skin->bones_upload);
        float_array_reserve(skin->weights, geometry->vertices_indexes->len * skin->bones_upload);

        for(i = 0; i < geometry->vertices_indexes->len; ++i) {
                ii = geometry->vertices_indexes->ptr[i];

                for(j = 0; j < skin->bones_per_vertex; ++j) {
                        float_array_add_float(skin->bones_id, v_bones_id->ptr[ii * skin->bones_per_vertex + j]);
                        float_array_add_float(skin->weights, v_weights->ptr[ii * skin->bones_per_vertex + j]);
                }

                while(j < skin->bones_upload) {
                        float_array_add_float(skin->bones_id, 0.0f);
                        float_array_add_float(skin->weights, 0.0f);
                        j++;
                }
        }

        ref_dec(&searchs->base);
        ref_dec(&weights->base);
        ref_dec(&vcount->base);
        ref_dec(&v->base);
        ref_dec(&v_bones_id->base);
        ref_dec(&v_weights->base);
}

static void __parse_library_controllers(struct dae_context *p,
        struct xml_node *xn)
{
        struct array *controllers = array_new(1);
        unsigned i;

        xml_node_search(xn, "//library_controllers/controller",
                controllers);
        for(i = 0; i < controllers->len; ++i) {
                __parse_controller(p, cast(array_get(controllers, i),
                        struct xml_node, base));
        }

        ref_dec(&controllers->base);
}

static struct dae_bone *__parse_bone(struct dae_amature *p, struct xml_node *xn)
{
        struct dae_bone *bone = dae_bone_new(),
                        *bone_c;
        struct array *searchs = array_new(1);
        struct xml_attribute *attr_id, *attr_sid;
        struct xml_node *xn_matrix, *xn_node;
        unsigned i;

        attr_id = xml_node_get_attribute(xn, KEYSIZE_L("id"));
        string_cat_chars(bone->id, attr_id->value, attr_id->value_len);

        attr_sid = xml_node_get_attribute(xn, KEYSIZE_L("sid"));
        string_cat_chars(bone->sid, attr_sid->value, attr_sid->value_len);

        dae_amature_add_bone(p, bone);

        xml_node_search(xn, "/matrix", searchs);
        if(searchs->len) {
                xn_matrix = cast(array_get(searchs, 0), struct xml_node, base);
                __fill_float(16, bone->transform.m, xn_matrix->value);
                mat4_transpose(&bone->transform);
        }

        xml_node_search(xn, "/node", searchs);
        for(i = 0; i < searchs->len; ++i) {
                xn_node = cast(array_get(searchs, i), struct xml_node, base);
                bone_c = __parse_bone(p, xn_node);
                dae_bone_add_bone(bone, bone_c);
        }

        ref_dec(&bone->base);
        ref_dec(&searchs->base);

        return bone;
}

static void __parse_node_amature(struct dae_context *p,
        struct xml_node *xn_node)
{
        struct dae_amature *amature = dae_amature_new();
        struct array *searchs = array_new(1);
        struct xml_node *xn_matrix, *xn_bone;
        struct xml_attribute *attr_id;
        unsigned i;

        attr_id = xml_node_get_attribute(xn_node, KEYSIZE_L("id"));
        string_cat_chars(amature->id, attr_id->value, attr_id->value_len);
        hash_table_set(p->amatures, &amature->base,
                amature->id->ptr, amature->id->len);

        xml_node_search(xn_node, "/matrix", searchs);
        if(searchs->len) {
                xn_matrix = cast(array_get(searchs, 0), struct xml_node, base);
                __fill_float(16, amature->transform.m, xn_matrix->value);
                mat4_transpose(&amature->transform);
        }

        xml_node_search(xn_node, "/node", searchs);
        for(i = 0; i < searchs->len; ++i) {
                xn_bone = cast(array_get(searchs, i), struct xml_node, base);
                attr_id = xml_node_get_attribute(xn_bone, KEYSIZE_L("id"));
                hash_table_set(p->amatures, &amature->base,
                        attr_id->value, attr_id->value_len);
                __parse_bone(amature, xn_bone);
        }

        ref_dec(&amature->base);
        ref_dec(&searchs->base);
}

static struct dae_node *__parse_node_visual(struct dae_context *p,
        struct xml_node *xn_node)
{
        struct dae_node *node = dae_node_new(), *node_c;
        struct dae_geometry *geometry = NULL;
        struct dae_skin *skin = NULL;
        struct dae_amature *amature = NULL;
        struct array *searchs = array_new(1);
        struct xml_node *xn_instance, *xn_skeleton,
                *xn_matrix, *xn_child;
        struct xml_attribute *attr_id, *attr_url;
        unsigned i;

        attr_id = xml_node_get_attribute(xn_node, KEYSIZE_L("id"));
        string_cat_chars(node->id, attr_id->value, attr_id->value_len);
        hash_table_set(p->nodes, &node->base,
                node->id->ptr, node->id->len);

        xml_node_search(xn_node, "/instance_controller", searchs);
        if(searchs->len) {
                xn_instance = cast(array_get(searchs, 0), struct xml_node, base);
                attr_url = xml_node_get_attribute(xn_instance, KEYSIZE_L("url"));
                skin =  cast(hash_table_get(p->skines,
                        attr_url->value + 1, attr_url->value_len - 1),
                        struct dae_skin, base);

                xml_node_search(xn_instance, "/skeleton", searchs);
                xn_skeleton = cast(array_get(searchs, 0), struct xml_node, base);
                amature = cast(hash_table_get(p->amatures,
                        xn_skeleton->value + 1, xn_skeleton->value_len - 1),
                        struct dae_amature, base);
        } else {
                xml_node_search(xn_node, "/instance_geometry", searchs);
                if(searchs->len) {
                        xn_instance = cast(array_get(searchs, 0), struct xml_node, base);
                        attr_url = xml_node_get_attribute(xn_instance, KEYSIZE_L("url"));
                        geometry = cast(hash_table_get(p->geometries,
                                attr_url->value + 1, attr_url->value_len - 1),
                                struct dae_geometry, base);
                }
        }

        if(geometry) {
                float_array_assign(&node->vertices, geometry->vertices);
                float_array_assign(&node->normals, geometry->normals);
                float_array_assign(&node->texcoords, geometry->texcoords);
                float_array_assign(&node->colors, geometry->colors);
                xml_node_search(xn_node, "/matrix", searchs);
                if(searchs) {
                        xn_matrix = cast(array_get(searchs, 0),
                                struct xml_node, base);
                        __fill_float(16, node->transform.m, xn_matrix->value);
                        mat4_transpose(&node->transform);
                }
        } else if(skin && amature) {
                float_array_assign(&node->vertices, skin->vertices);
                float_array_assign(&node->normals, skin->normals);
                float_array_assign(&node->texcoords, skin->texcoords);
                float_array_assign(&node->colors, skin->colors);
                float_array_assign(&node->bones_id, skin->bones_id);
                float_array_assign(&node->weights, skin->weights);
                float_array_assign(&node->inverse_bind_poses, skin->inverse_bind_poses);
                array_assign(&node->bone_names, skin->bone_names);
                dae_amature_assign(&node->amature, amature);
                node->bones_per_vertex = skin->bones_per_vertex;
                node->bones_upload = skin->bones_upload;
        }

        xml_node_search(xn_node, "/node", searchs);
        for(i = 0; i < searchs->len; ++i) {
                xn_child = cast(array_get(searchs, i), struct xml_node, base);
                node_c = __parse_node_visual(p, xn_child);
                dae_node_add_child(node, node_c);
        }

        ref_dec(&node->base);
        ref_dec(&searchs->base);
        return node;
}

static void __parse_visual_scene(struct dae_context *p,
        struct xml_node *xn_visual_scene)
{
        struct array    *searchs = array_new(1),
                        *searchs_sub = array_new(1);
        struct xml_node *xn_node;
        unsigned i;

        xml_node_search(xn_visual_scene, "/node", searchs);
        for(i = 0; i < searchs->len; ++i) {
                xn_node = cast(array_get(searchs, i), struct xml_node, base);
                xml_node_search(xn_node, "/node[@type=\"JOINT\"]", searchs_sub);
                if(searchs_sub->len) {
                        __parse_node_amature(p, xn_node);
                } else {
                        __parse_node_visual(p, xn_node);
                }
        }

        ref_dec(&searchs->base);
        ref_dec(&searchs_sub->base);
}

static void __parse_library_visual_scenes(struct dae_context *p,
        struct xml_node *xn)
{
        struct array *searchs = array_new(1);
        unsigned i;

        xml_node_search(xn, "//library_visual_scenes/visual_scene",
                searchs);
        for(i = 0; i < searchs->len; ++i) {
                __parse_visual_scene(p, cast(array_get(searchs, i),
                        struct xml_node, base));
        }

        ref_dec(&searchs->base);
}

static struct dae_bone *__animation_get_bone(struct dae_context *p,
        const char *key, const unsigned key_len)
{
        struct dae_bone *bone = NULL;
        struct list_head *head;
        struct dae_amature *amature;

        head = hash_table_get_iterator(p->amatures);
next_amature:
        amature = cast_null(hash_table_iterator_next(p->amatures, &head),
                struct dae_amature, base);
        if(amature) {
                bone = cast_null(hash_table_get(amature->bones, key, key_len),
                        struct dae_bone, base);
                if(bone) goto finish;
                goto next_amature;
        }

finish:
        return bone;
}

static void __parse_animation(struct dae_context *p, struct xml_node *xn)
{
        struct array *searchs = array_new(1);
        struct xml_node *xn_float_array, *xn_time;
        struct xml_attribute *attr_count, *attr_target;
        unsigned count;
        const char *start, *end;
        struct dae_bone *bone;
        struct float_array *fa;
        unsigned  i;

        xml_node_search(xn, "//channel/@target", searchs);
        attr_target = cast(array_get(searchs, 0), struct xml_attribute, base);
        start = attr_target->value;
        end = strchr(start, '/');
        bone = __animation_get_bone(p, start, end - start);
        if(!bone) goto finish;

        xml_node_search(xn, "//param[@name=\"TRANSFORM\"]/../../../float_array",
                searchs);
        xn_float_array = cast(array_get(searchs, 0), struct xml_node, base);
        attr_count = xml_node_get_attribute(xn_float_array, KEYSIZE_L("count"));
        count = atoi(attr_count->value);
        fa = float_array_new();
        float_array_reserve(fa, count);
        float_array_add_string(fa, xn_float_array->value);
        for(i = 0; i < fa->len; i += 16) {
                mat4_transpose((union mat4 *)(fa->ptr + i));
        }
        float_array_assign(&bone->animation_transforms, fa);
        ref_dec(&fa->base);

        xml_node_search(xn, "//param[@name=\"TIME\"]/../../../float_array",
                searchs);
        xn_float_array = cast(array_get(searchs, 0), struct xml_node, base);
        attr_count = xml_node_get_attribute(xn_float_array, KEYSIZE_L("count"));
        count = atoi(attr_count->value);
        fa = float_array_new();
        float_array_reserve(fa, count);
        float_array_add_string(fa, xn_float_array->value);
        float_array_assign(&bone->animation_times, fa);
        ref_dec(&fa->base);

finish:
        ref_dec(&searchs->base);
}

static void __parse_library_animations(struct dae_context *p,
        struct xml_node *xn)
{
        struct array *searchs = array_new(1);
        unsigned i;

        xml_node_search(xn, "//library_animations/animation",
                searchs);
        for(i = 0; i < searchs->len; ++i) {
                __parse_animation(p, cast(array_get(searchs, i),
                        struct xml_node, base));
        }

        ref_dec(&searchs->base);
}

void dae_context_parse(struct dae_context *p, const char *file)
{
        struct xml_context *ctx = xml_context_new();
        xml_context_parse(ctx, file);
        if(!ctx->root) goto finish;

        __parse_library_geometries(p, ctx->root);
        __parse_library_controllers(p, ctx->root);
        __parse_library_visual_scenes(p, ctx->root);
        __parse_library_animations(p, ctx->root);

finish:
        ref_dec(&ctx->base);
}
