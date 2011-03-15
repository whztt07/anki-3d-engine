#ifndef IMAGE_H
#define IMAGE_H

#include "Vec.h"
#include "StdTypes.h"


/// Image class.
/// Used in Texture::load. Supported types: TGA and PNG
class Image
{
	public:
		/// The acceptable color types of AnKi
		enum ColorType
		{
			CT_R, ///< Red only
			CT_RGB, ///< RGB
			CT_RGBA /// RGB plus alpha
		};

		/// Do nothing
		Image() {}

		/// Load an image
		/// @param[in] filename The image file to load
		/// @exception Exception
		Image(const char* filename) {load(filename);}
		
		/// Do nothing
		~Image() {}

		/// @name Accessors
		/// @{
		uint getWidth() const {return width;}
		uint getHeight() const {return height;}
		ColorType getColorType() const {return type;}
		const Vec<uchar>& getData() const {return data;}
		/// @}
		
		/// Load an image file
		/// @param[in] filename The file to load
		/// @exception Exception
		void load(const char* filename);

	private:
		uint width; ///< Image width
		uint height; ///< Image height
		ColorType type; ///< Image color type
		Vec<uchar> data; ///< Image data

		/// @name TGA headers
		/// @{
		static uchar tgaHeaderUncompressed[12];
		static uchar tgaHeaderCompressed[12];
		/// @}

		/// Load a TGA
		/// @param[in] filename The file to load
		/// @exception Exception
		void loadTga(const char* filename);

		/// Used by loadTga
		/// @param[in] fs The input
		/// @param[out] bpp Bits per pixel
		/// @exception Exception
		void loadUncompressedTga(std::fstream& fs, uint& bpp);

		/// Used by loadTga
		/// @param[in] fs The input
		/// @param[out] bpp Bits per pixel
		/// @exception Exception
		void loadCompressedTga(std::fstream& fs, uint& bpp);

		/// Load PNG. Dont throw exception because libpng is in C
		/// @param[in] filename The file to load
		/// @param[out] err The error message
		/// @return true on success
		bool loadPng(const char* filename, std::string& err) throw();
};


#endif