#include <stdlib.h>
#include <jpeg/jpeglib.h>
#include <png/png.h>
#include <png/pngstruct.h>
#include <kernel/io.h>
#include <opengl/image.h>

static void __image_free(const struct ref *ref);

struct image *image_new()
{
        return image_init(malloc(sizeof(struct image)));
}

struct image *image_init(struct image *p)
{
        p->base = (struct ref){__image_free, 1};
        p->type = -1;
        p->ptr = NULL;
        p->width = p->height = 0;
        return p;
}

void image_release(struct image *p)
{
        if(p->ptr) {
                free(p->ptr);
                p->ptr = NULL;
        }
}

static void __image_free(const struct ref *ref)
{
        struct image *p = cast(ref, struct image, base);
        image_release(p);
        free(p);
}

/*
 * load JPEG
 */
const static JOCTET EOI_BUFFER[1] = { JPEG_EOI };

struct jpeg_source {
        struct jpeg_source_mgr pub;
        const JOCTET *data;
        size_t       len;
};

static void __jpeg_init_source(j_decompress_ptr cinfo) {

}

static boolean __jpeg_fill_input_buffer(j_decompress_ptr cinfo) {
        struct jpeg_source* src = (struct jpeg_source*)cinfo->src;
        src->pub.next_input_byte = EOI_BUFFER;
        src->pub.bytes_in_buffer = 1;
        return TRUE;
}
static void __jpeg_skip_input_data(j_decompress_ptr cinfo, long num_bytes) {
        struct jpeg_source* src = (struct jpeg_source*)cinfo->src;
        if (src->pub.bytes_in_buffer < num_bytes) {
                src->pub.next_input_byte = EOI_BUFFER;
                src->pub.bytes_in_buffer = 1;
        } else {
                src->pub.next_input_byte += num_bytes;
                src->pub.bytes_in_buffer -= num_bytes;
        }
}
static void __jpeg_term_source(j_decompress_ptr cinfo) {

}

static void __jpeg_set_source_mgr(j_decompress_ptr cinfo,
        const char* data, size_t len) {
        struct jpeg_source* src;
        if (cinfo->src == 0) {
                cinfo->src = (struct jpeg_source_mgr *)
                (*cinfo->mem->alloc_small)
                ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                        sizeof(struct jpeg_source));
        }
        src = (struct jpeg_source*) cinfo->src;
        src->pub.init_source = __jpeg_init_source;
        src->pub.fill_input_buffer = __jpeg_fill_input_buffer;
        src->pub.skip_input_data = __jpeg_skip_input_data;
        src->pub.resync_to_restart = jpeg_resync_to_restart;
        src->pub.term_source = __jpeg_term_source;

        src->data = data;
        src->len = len;
        src->pub.bytes_in_buffer = len;
        src->pub.next_input_byte = src->data;
}

static void __load_jpeg(struct image *p, const char *path)
{
        struct jpeg_decompress_struct cInfo;
        struct jpeg_error_mgr jError;
        cInfo.err = jpeg_std_error(&jError);

        jpeg_create_decompress(&cInfo);

        struct string *content  = file_to_string(path);
        __jpeg_set_source_mgr(&cInfo, content->ptr, content->len);

        char* pTexUint;
        int yy;
        unsigned width, height;
        int pixelSize, lineSize;
        char* lpbtBits;
        JSAMPLE tmp;
        int rectHeight, rectWidth;

        jpeg_read_header(&cInfo, TRUE);
        jpeg_start_decompress(&cInfo);

        width                   = cInfo.output_width;
        height                  = cInfo.output_height;
        pixelSize               = cInfo.output_components;
        lineSize                = width * pixelSize;

        pTexUint                = calloc(3, width * height);
        JSAMPROW buffer[1];
        buffer[0] = pTexUint + width * 3 * (height - 1);

        yy = 0;
        while(cInfo.output_scanline < cInfo.output_height) {
                if(yy >= cInfo.output_height)
                        break;

                jpeg_read_scanlines(&cInfo, buffer, 1);

                yy++;
                if(yy < cInfo.output_height) {
                        buffer[0] -= width * 3;
                }
        }

        jpeg_finish_decompress(&cInfo);
        jpeg_destroy_decompress(&cInfo);

        p->width        = width;
        p->height       = height;
        p->ptr          = pTexUint;
        p->type         = IMAGE_RGB;

        ref_dec(&content->base);
}

/*
 * load png
 */

static void __png_read(png_structp png_ptr, png_bytep data, png_size_t length)
{
        file_read(png_ptr->io_ptr, length, data);
}

static void __load_png(struct image *p, const char *path)
{
        struct file * volatile f  = file_open(path, "r");
        png_byte * volatile image_data = NULL;
        png_bytep * volatile row_pointers = NULL;
        png_byte header[8];

        file_read(f, 8, (char *)header);
        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                                 NULL, NULL);
        if (!png_ptr) {
                goto error;
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
                png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
                goto error;
        }

        png_infop end_info = png_create_info_struct(png_ptr);
        if (!end_info) {
                png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
                goto error;
        }

        if (setjmp(png_jmpbuf(png_ptr))) {
                png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
                goto error;
        }

        png_set_read_fn(png_ptr, f, __png_read);

        png_set_sig_bytes(png_ptr, 8);

        png_read_info(png_ptr, info_ptr);

        int bit_depth, color_type;
        png_uint_32 twidth, theight;

        png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
                 NULL, NULL, NULL);

        if(bit_depth == 16)
                png_set_strip_16(png_ptr);

        if(color_type == PNG_COLOR_TYPE_PALETTE)
                png_set_palette_to_rgb(png_ptr);

        if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
                png_set_expand_gray_1_2_4_to_8(png_ptr);

        if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
                png_set_tRNS_to_alpha(png_ptr);

        if(color_type == PNG_COLOR_TYPE_RGB ||
                color_type == PNG_COLOR_TYPE_GRAY ||
                color_type == PNG_COLOR_TYPE_PALETTE)
                png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

        if(color_type == PNG_COLOR_TYPE_GRAY ||
                color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
                png_set_gray_to_rgb(png_ptr);

        png_set_interlace_handling(png_ptr);

        unsigned width = twidth;
        unsigned height = theight;

        png_read_update_info(png_ptr, info_ptr);

        int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

        image_data = malloc(sizeof(png_byte) * rowbytes * height);
        if (!image_data) {
                png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
                goto error;
        }

        row_pointers = malloc(sizeof(png_bytep) * height);
        if (!row_pointers) {
                png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
                goto error;
        }

        for (int i = 0; i < height; ++i)
                row_pointers[height - 1 - i] = image_data + i * rowbytes;

        png_read_image(png_ptr, row_pointers);

        p->width    = width;
        p->height   = height;
        p->ptr      = image_data;
        p->type     = IMAGE_RGBA;

        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(row_pointers);
        ref_dec(&f->base);
        return;

error:
        if(row_pointers) {
                free(row_pointers);
        }
        if(image_data) {
                free(image_data);
        }
        ref_dec(&f->base);
}

void image_load(struct image *p, const char *file)
{
        struct file *f  = file_open(file, "r");

        png_byte header[8];
        file_read(f, 8, header);

        const int is_png = !png_sig_cmp(header, 0, 8);
        ref_dec(&f->base);

        if (!is_png) {
                __load_jpeg(p, file);
        } else {
                __load_png(p, file);
        }
}
