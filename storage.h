#ifdef __cplusplus
#extern "C" {
#endif

int print_data_save (FpPrint *print,
                     FpFinger finger,
                     gboolean update_fingerprint);
FpPrint * print_data_load (FpDevice *dev,
                           FpFinger  finger);
GPtrArray * gallery_data_load (FpDevice *dev);
FpPrint * print_create_template (FpDevice      *dev,
                                 FpFinger       finger,
                                 const gboolean load_existing);
gboolean print_image_save (FpPrint    *print,
                           const char *path);
gboolean save_image_to_pgm (FpImage    *img,
                            const char *path);

#ifdef __cplusplus
}
#endif
