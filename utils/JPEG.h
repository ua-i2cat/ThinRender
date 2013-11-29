/* 
 * From http://www.stanford.edu/~acoates/decompressJpegFromMemory.txt
 * 
 * With a few modifications it allows us to use libjpeg reading a compressed
 * picture from memory instead of using a FILE pointer.
 * 
 */

#include "../libs/libjpeg/jpeglib.h"
#include "../libs/libjpeg/jerror.h"

const static JOCTET EOI_BUFFER[1] = { JPEG_EOI };
struct my_source_mgr {
    struct jpeg_source_mgr pub;
    const JOCTET *data;
    size_t       len;
};

static void my_init_source(j_decompress_ptr cinfo) {}

static boolean my_fill_input_buffer(j_decompress_ptr cinfo) {
    my_source_mgr* src = (my_source_mgr*)cinfo->src;
    // No more data.  Probably an incomplete image;  just output EOI.
    src->pub.next_input_byte = EOI_BUFFER;
    src->pub.bytes_in_buffer = 1;
    return TRUE;
}
static void my_skip_input_data(j_decompress_ptr cinfo, long num_bytes) {
    my_source_mgr* src = (my_source_mgr*)cinfo->src;
    if (src->pub.bytes_in_buffer < num_bytes) {
        // Skipping over all of remaining data;  output EOI.
        src->pub.next_input_byte = EOI_BUFFER;
        src->pub.bytes_in_buffer = 1;
    } else {
        // Skipping over only some of the remaining data.
        src->pub.next_input_byte += num_bytes;
        src->pub.bytes_in_buffer -= num_bytes;
    }
}
static void my_term_source(j_decompress_ptr cinfo) {}

static void my_set_source_mgr(j_decompress_ptr cinfo, const char* data, size_t len) {
    my_source_mgr* src;
    if (cinfo->src == 0) { // if this is first time;  allocate memory
        cinfo->src = (struct jpeg_source_mgr *)(*cinfo->mem->alloc_small)
            ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(my_source_mgr));
    }
    src = (my_source_mgr*) cinfo->src;
    src->pub.init_source = my_init_source;
    src->pub.fill_input_buffer = my_fill_input_buffer;
    src->pub.skip_input_data = my_skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart; // default
    src->pub.term_source = my_term_source;
    // fill the buffers
    src->data = (const JOCTET *)data;
    src->len = len;
    src->pub.bytes_in_buffer = len;
    src->pub.next_input_byte = src->data;
}

void parseJPEG(const char* data, size_t len, int* size, int* w, int* h) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    my_set_source_mgr(&cinfo, data, len);

    (void) jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);
    *w = (int) cinfo.output_width;
    *h = (int) cinfo.output_height;
    *size = (int) cinfo.output_components
                * cinfo.output_width
                * cinfo.output_height;
                //* 2;

    jpeg_destroy_decompress(&cinfo);
}

int decompressJPEG(const char* data, size_t len, char* out) {
    struct jpeg_decompress_struct cinfo; 
    struct jpeg_error_mgr jerr;
    char* out_buffer = out;
    int size;

    // Setup decompression structure
    cinfo.err = jpeg_std_error(&jerr); 
    jpeg_create_decompress(&cinfo); 
    my_set_source_mgr(&cinfo, data,len);

    // read info from header.
    (void) jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);
   
    int row_stride = cinfo.output_width * cinfo.output_components;// * 2;
    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);


    out_buffer = out_buffer + cinfo.output_height * row_stride - row_stride;
    while (cinfo.output_scanline < cinfo.output_height) {
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);
        memcpy(out_buffer, buffer[0], row_stride);
        size += row_stride;
        out_buffer -= row_stride;
    }

    out = out_buffer;
    jpeg_destroy_decompress(&cinfo);
    return size;
}
