#include <compress.h>

// 压缩图像
void compress(const void* rgb, int width, int height, void** jpeg, size_t* outSize)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	
	jpeg_mem_dest(&cinfo, (unsigned char**)jpeg, outSize);
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	size_t rowSize = cinfo.input_components * cinfo.image_width;
	JSAMPARRAY buffer = cinfo.mem->alloc_sarray((j_common_ptr)(&cinfo), JPOOL_IMAGE, rowSize, 1);

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 80, 1);

	jpeg_start_compress(&cinfo, 1);

	for(uint8_t* ptr = (uint8_t*)(rgb); cinfo.next_scanline < cinfo.image_height; ptr += rowSize)
    {
		memcpy(buffer[0], ptr, rowSize);
        jpeg_write_scanlines(&cinfo, buffer, 1);
    }

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
}

