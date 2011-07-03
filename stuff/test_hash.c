 #include <libhashish.h>

int main(void) {
  hi_handle_t *hi_handle;
  /*const */char *key;
  /*const */char *data;
  const char *data_ptr;
  int ret;

  /* initialize hashish handle */
  hi_init_str(&hi_handle, 23);

  /* insert an key/data pair */
  key = "12"; data = "data element";
  fprintf(stdout, "Ajoute: %s Data: %s\n", key, data);
  ret = hi_insert_str(hi_handle, key, data);
  key = "34"; data = "data element2";
  fprintf(stdout, "Ajoute: %s Data: %s\n", key, data);
  ret = hi_insert_str(hi_handle, key, data);
  key = "56"; data = "data element3";
  fprintf(stdout, "Ajoute: %s Data: %s\n", key, data);
  ret = hi_insert_str(hi_handle, key, data);
  key = "78"; data = "data element4";
  fprintf(stdout, "Ajoute: %s Data: %s\n", key, data);
  ret = hi_insert_str(hi_handle, key, data);
  key = "90"; data = "data element5";
  fprintf(stdout, "Ajoute: %s Data: %s\n", key, data);
  ret = hi_insert_str(hi_handle, key, data);

  key = "78";
  
  /* search for a pair with a string key and store result */
  hi_get_str(hi_handle, key, (void**)&data_ptr);

  fprintf(stdout, "Key: %s Data: %s\n", key, data_ptr);

  /* free the hashish handle */
  hi_fini(hi_handle);

  return 0;
}
