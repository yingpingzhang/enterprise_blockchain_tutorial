#ifndef INCLUDE_H_DL
#define INCLUDE_H_DL

#ifdef WIN32

#ifdef __cplusplus
extern "C" {
#endif

#define RTLD_LAZY 0x1

	/**
	 * Open DLL, returning a handle.
	 * @param file DLL filename (UTF-8).
	 * @param mode mode flags (ignored)..
	 */
	void* dlopen(const char* file, int mode);
	/**
	 * Close DLL.
	 * @param handle Handle from dlopen().
	 */
	int dlclose(void* handle);
	/**
	 * Look up symbol exported by DLL.
	 * @param handle Handle from dlopen().
	 * @param name Name of exported symbol (ASCII).
	 */
	void* dlsym(void* handle, const char* name);
	/**
	 * Return message describing last error.
	 */
	char* dlerror(void);

#endif // WIN32

#ifdef __cplusplus
}
#endif

#endif INCLUDE_H_DL