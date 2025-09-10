#define FP_COMPONENT "example-capture"

#include <stdio.h>
#include <libfprint-2/fprint.h>
#include <glib-unix.h>

#include "storage.h"
#include "utilities.h"

typedef struct CaptureData {
  GMainLoop    *loop;
  GCancellable *cancellable;     //Vai receber g_cancellable_new() permitindo interromper operações longas ou assíncronas
  unsigned int  sigint_handler;  //Manipulador callback executado sob SIGINT, SIGTERM, etc. Especifica prioridade e fonte do sinal
  int           ret_value;
  const char   *filename;
} CaptureData;


/*****************************************************************************/
/**  Quando um CaptureData sair fora de escopo, será eliminado pela Glib2    */
/**  graças à macro G_DEFINE_AUTOPTR_CLEANUP_FUNC                            */
/*****************************************************************************/
static void capture_data_free (CaptureData *capture_data) {
  g_clear_handle_id (&capture_data->sigint_handler, g_source_remove);
  g_clear_object (&capture_data->cancellable);
  g_main_loop_unref (capture_data->loop);
  g_free (capture_data);
}
G_DEFINE_AUTOPTR_CLEANUP_FUNC (CaptureData, capture_data_free)

/*****************************************************************************/
/**                                                                          */
/*****************************************************************************/
static void on_device_closed (FpDevice *dev, GAsyncResult *res, void *user_data) {
  CaptureData *capture_data = user_data;

  g_autoptr(GError) error = NULL;

  fp_device_close_finish (dev, res, &error);

  if (error)
    g_warning ("Failed closing device %s", error->message);

  g_main_loop_quit (capture_data->loop);
}

/*****************************************************************************/
/**                                                                          */
/*****************************************************************************/
static void capture_quit (FpDevice    *dev,
                          CaptureData *capture_data) {
  if (!fp_device_is_open (dev)) {
      g_main_loop_quit (capture_data->loop);
      return;
  }

  fp_device_close (dev, NULL, (GAsyncReadyCallback) on_device_closed, capture_data);
}

/*****************************************************************************/
/** Essa é função callback da captura                                        */
/*****************************************************************************/
static void dev_capture_cb (FpDevice     *dev,
                            GAsyncResult *res,
                            void         *user_data) {
  g_autoptr(GError) error = NULL;
  CaptureData *capture_data = user_data;
  FpImage *image = NULL;

  g_clear_object (&capture_data->cancellable);

  image = fp_device_capture_finish (dev, res, &error);
  if (!image) {
      g_warning ("Error capturing data: %s", error->message);
      capture_quit (dev, capture_data);
      return;
  }

  save_image_to_pgm (image, capture_data->filename);

  capture_quit (dev, capture_data); //Aqui também faz o fechamento do dispositivo
}

/*****************************************************************************/
/** Inicia a captura de uma digital.                                         */
/** Quando o processo termina, executa a função callback dev_capture_cb()    */
/*****************************************************************************/
static void start_capture (FpDevice *dev, CaptureData *capture_data) {
  fp_device_capture (dev, TRUE, capture_data->cancellable, (GAsyncReadyCallback) dev_capture_cb, capture_data);
}

/*****************************************************************************/
/** Essa é a função callback de fp_open_device()                             */
/** A abertura do dispositivo pode ter tido sucesso ou falhado, então        */
/** deve-se consultar o resultado com fp_device_open_finish()                */
/*****************************************************************************/
static void on_device_opened (FpDevice *dev, GAsyncResult *res, void *user_data) {
  CaptureData *capture_data = user_data;

  g_autoptr(GError) error = NULL;

  //Essa é a função que permite saber como terminou o fp_device_open()
  //Para que não haja bug aqui nesse código, ela deve "sincronizar" com fp_device_open()
  //Ou seja, ficar bloqueada até que fp_device_open() tenha terminado o processo de abertura...
  if (!fp_device_open_finish (dev, res, &error)) {
      g_warning ("Failed to open device: %s", error->message);
      capture_quit (dev, capture_data);   //vai fazer o encerramento, fechar o device com fp_device_close()
      return;
  }

  g_print ("Opened device. ");

  start_capture (dev, capture_data);     //Aqui inicia a real captura da digital
}

/*****************************************************************************/
/**                                                                          */
/*****************************************************************************/
static gboolean sigint_cb (void *user_data) {
  CaptureData *capture_data = user_data;

  g_cancellable_cancel (capture_data->cancellable);  //Notifia que é para cancelar sob ctrl-c

  return G_SOURCE_CONTINUE;
}

static gboolean timeout_callback(void *user_data) {
    CaptureData *capture_data = user_data;
    g_cancellable_cancel (capture_data->cancellable);  //Notifia que é para cancelar sob ctrl-c
    return TRUE;
}


static GCancellable *cancellable;

void cancelScanning() {
  if (cancellable != NULL)
     g_cancellable_cancel (cancellable);
  cancellable = NULL;
} 
/*****************************************************************************/
/**                                                                          */
/*****************************************************************************/
int main2 (int button) {
  g_autoptr(FpContext) ctx = NULL;
  g_autoptr(CaptureData) capture_data = NULL;
  GPtrArray *devices;
  FpDevice *dev;

  setenv ("G_MESSAGES_DEBUG", "all", 0);
  setenv ("LIBUSB_DEBUG", "3", 0);

  ctx = fp_context_new();

  devices = fp_context_get_devices (ctx);
  if (!devices) {
      g_warning ("Impossible to get devices");
      return EXIT_FAILURE;
  }

  dev = discover_device (devices);
  if (!dev){
      g_warning ("No devices detected.");
      return EXIT_FAILURE;
  }

  if (!fp_device_has_feature (dev, FP_DEVICE_FEATURE_CAPTURE)) {
      g_warning ("Device %s doesn't support capture",
                 fp_device_get_name (dev));
      return EXIT_FAILURE;
  }
  gboolean ret;
  g_autoptr(GError) error = NULL;
  capture_data = g_new0 (CaptureData, 1);
  if (button == 1)                                    //Aloca memória para um CaptureData, zerando a estrutura inteira
      capture_data->filename = "finger-1.pgm";
  else
      capture_data->filename = "finger-2.pgm";
  capture_data->ret_value = EXIT_FAILURE;                                    //Valor de retorno inicia com EXIT_FAILURE
  capture_data->loop = g_main_loop_new (NULL, FALSE);                        //Cria um objeto de loop de eventos para rodar as funções que aceitarão sinais e callbacks.
  capture_data->cancellable = g_cancellable_new ();                          //g_cancellable_new() retorna GCancellable, permite cancelar operações longas ou assíncronas
  cancellable = capture_data->cancellable;
  ret = fp_device_open_sync(dev, capture_data->cancellable,&error);
  if (ret) {
     g_warning ("Sucesso na abertura do dispositivo:");
  }
  else {
      g_warning ("Failed to open device: %s", error->message);
      return 0;
  }
  FpImage* image = fp_device_capture_sync(dev, TRUE, capture_data->cancellable, &error);
  if (image) {
     g_warning ("Sucesso na captura da imagem:");
     save_image_to_pgm (image, capture_data->filename);
  }
  else {
      g_warning ("Falha na captura da imagem no dispositivo: %s", error->message);
  }
  
  ret = fp_device_close_sync (dev, NULL, &error);
   if (ret) {
     g_warning ("Sucesso no fechamento do dispositivo:");
  }
  else {
      g_warning ("Failed to close device: %s", error->message);
      return 0;
  }
 
  return 0;  
}

/*****************************************************************************/
/**                                                                          */
/*****************************************************************************/
int main3 () {
  g_autoptr(FpContext) ctx = NULL;
  g_autoptr(CaptureData) capture_data = NULL;
  GPtrArray *devices;
  FpDevice *dev;

  setenv ("G_MESSAGES_DEBUG", "all", 0);
  setenv ("LIBUSB_DEBUG", "3", 0);

  ctx = fp_context_new();

  devices = fp_context_get_devices (ctx);
  if (!devices) {
      g_warning ("Impossible to get devices");
      return EXIT_FAILURE;
  }

  dev = discover_device (devices);
  if (!dev){
      g_warning ("No devices detected.");
      return EXIT_FAILURE;
  }

  if (!fp_device_has_feature (dev, FP_DEVICE_FEATURE_CAPTURE)) {
      g_warning ("Device %s doesn't support capture",
                 fp_device_get_name (dev));
      return EXIT_FAILURE;
  }

  capture_data = g_new0 (CaptureData, 1);                                    //Aloca memória para um CaptureData, zerando a estrutura inteira
  capture_data->ret_value = EXIT_FAILURE;                                    //Valor de retorno inicia com EXIT_FAILURE
  capture_data->loop = g_main_loop_new (NULL, FALSE);                        //Cria um objeto de loop de eventos para rodar as funções que aceitarão sinais e callbacks.
  capture_data->cancellable = g_cancellable_new ();                          //g_cancellable_new() retorna GCancellable, permite cancelar operações longas ou assíncronas
  capture_data->sigint_handler = g_unix_signal_add_full (G_PRIORITY_HIGH,    //Prioridade alta
                                                         SIGINT,             //Normalmente um Ctrl+C
                                                         sigint_cb,          //Função que será chamada quando o sinal ocorrer.
                                                         capture_data,       //O próprio capture_data será passado como parâmetro para sigint_cb()
                                                         NULL);              //Não precisa fazer limpeza, capture_data será eliminado pelo próprio glib

  capture_data->filename = "finger.pgm";
  fp_device_open (dev, capture_data->cancellable,           //fp_device_open é assíncrona, quando conseguir abrir o dispositivo
                  (GAsyncReadyCallback) on_device_opened,   //a função on_device_opened() será chamada com o parâmetro capture_data
                  capture_data);

  g_timeout_add(10000, timeout_callback, capture_data);
  g_main_loop_run (capture_data->loop);
  //g_main_loop_unref(capture_data->loop);

  return capture_data->ret_value;
}
