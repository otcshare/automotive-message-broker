#define GCC_VERSION (__GNUC__ * 10000 \
							   + __GNUC_MINOR__ * 100 \
							   + __GNUC_PATCHLEVEL__)

#if GCC_VERSION < 40600
#define nullptr 0
#endif
