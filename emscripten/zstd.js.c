#include "zstd.h"	 /* ZSTD version numbers */

#if defined (__cplusplus)
extern "C" {
#endif

	size_t ZStdCompress(int dstPtr, int _maxDstSize, int srcPtr, int _srcSize, int compressionLevel) {
		// size_t ZSTD_compress(void* dst, size_t maxDstSize, const void* src, size_t srcSize, int compressionLevel)
		void *dst = (void*)dstPtr;
		size_t maxDstSize = (size_t)_maxDstSize;
		void *src = (void*)srcPtr;
		size_t srcSize = (size_t)_srcSize;

		size_t compressedSize = ZSTD_compress(dst, maxDstSize, src, srcSize, compressionLevel);
		return compressedSize;
	}

	size_t ZStdDecompress(int dstPtr, int _maxDstSize, int srcPtr, int _srcSize) {
		// size_t ZSTD_decompress(void* dst, size_t maxDstSize, const void* src, size_t srcSize)
		void *dst = (void*)dstPtr;
		size_t maxDstSize = (size_t)_maxDstSize;
		void *src = (void*)srcPtr;
		size_t srcSize = (size_t)_srcSize;

		size_t decompressedSize = ZSTD_decompress(dst, maxDstSize, src, srcSize);
		return decompressedSize;
	}


	// Streaming compression

	static size_t const buffInSize = ZSTD_BLOCKSIZE_MAX;
	static size_t const buffOutSize = (ZSTD_COMPRESSBOUND(ZSTD_BLOCKSIZE_MAX)) + 3 + 4;
	static char buffIn[buffInSize];
	static char buffOut[buffOutSize];
	static ZSTD_CStream* cctx = NULL;
	static ZSTD_EndDirective mode = ZSTD_e_continue;
	static ZSTD_inBuffer input = { buffIn, buffInSize, 0 };
	static ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };
	static size_t remaining = 1;

	static size_t buffParams[4] = { buffInSize, (size_t)buffIn, buffOutSize, (size_t)buffOut };

	size_t ZStdCompressStreamStart(int compressionLevel) {
		if (cctx == NULL) {
			cctx = ZSTD_createCStream();
		}
		input.pos = 0;
		output.pos = 0;
		ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, compressionLevel);
		return (size_t)buffParams;
	}

	int ZStdCompressStreamBlock(int blockBytes, int lastBlock) {
		if (blockBytes > buffInSize) return -1;
		input.pos = 0;
		output.pos = 0;
		remaining = 1;
		mode = lastBlock == 1 ? ZSTD_e_end : ZSTD_e_continue;
		input.size = blockBytes;
		return 0;
	}

	size_t ZStdCompressStreamContinue() {
        output.pos = 0;
		remaining = ZSTD_compressStream2(cctx, &output, &input, mode);
		return output.pos;
	}

	int ZStdCompressStreamAtEnd() {
		if (mode == ZSTD_e_end) {
			return remaining == 0;
		} else {
			return input.pos == input.size;
		}
	}


	// Streaming decompression

#if defined (__cplusplus)
}
#endif
