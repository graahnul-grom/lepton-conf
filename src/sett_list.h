#ifndef LEPTON_CONF_SETT_LIST_H_
#define LEPTON_CONF_SETT_LIST_H_

/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2021 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

gchar*
settings_list_to_string( GList* lst );

void
settings_list_save_string( const gchar* str, const gchar* name );

void
settings_list_save( GList* lst, const gchar* name );

gboolean
settings_list_add( GList** lst, gchar* item );

void
settings_list_load( GList** lst, const gchar* name );

void
settings_list_clear( GList** lst );


#endif // LEPTON_CONF_SETT_LIST_H_

