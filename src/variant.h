#ifndef PLAWNEKJXQML_VARIANT_H
#define PLAWNEKJXQML_VARIANT_H

#include <plawnekjx-core.h>
#include <QVariantMap>

namespace Plawnekjx
{
    QVariantMap parseParametersDict(GHashTable *dict);
    QVariant parseVariant(GVariant *v);
};

#endif
