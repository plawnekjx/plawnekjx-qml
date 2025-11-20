#ifndef PLAWNEKJXQML_PLAWNEKJXFWD_H
#define PLAWNEKJXQML_PLAWNEKJXFWD_H

// FIXME: Investigate compatibility issue between Qt and Plawnekjx headers.

extern "C"
{
    typedef struct _PlawnekjxApplication PlawnekjxApplication;
    typedef struct _PlawnekjxCrash PlawnekjxCrash;
    typedef struct _PlawnekjxDevice PlawnekjxDevice;
    typedef struct _PlawnekjxDeviceManager PlawnekjxDeviceManager;
    typedef struct _PlawnekjxIcon PlawnekjxIcon;
    typedef struct _PlawnekjxProcess PlawnekjxProcess;
    typedef struct _PlawnekjxScript PlawnekjxScript;
    typedef struct _PlawnekjxSession PlawnekjxSession;
    typedef struct _PlawnekjxSpawnOptions PlawnekjxSpawnOptions;

    typedef struct _GAsyncResult GAsyncResult;
    typedef struct _GBytes GBytes;
    typedef struct _GError GError;
    typedef struct _GObject GObject;
    typedef struct _GSource GSource;

    typedef void *gpointer;
    typedef int gint;
    typedef gint gboolean;
    typedef char gchar;
}

#endif
